import os
import sys
try:
    from PIL import Image
except:
    raise Exception("Please install PIL e.g. 'sudo apt-get install python3-pil'")
import struct
import argparse
import io
import shutil

def create_color16(r, g, b):
    return (((r << (11 - 3)) & 0xf800) | ((g << (5 - 2)) & 0x07c0) | ((b >> (3)) & 0x001f))

def create_color16be(r, g, b):
    color = create_color16(r, g, b)
    return struct.pack('>H', color)

def split_color16(color16):
    r = (color16 >> (11 - 3)) & 0xf8
    g = (color16 >> (5 - 2)) & 0xfc
    b = (color16 << (3)) & 0xf8
    return r, g, b

class HexDataHeader(object):

    def __init__(self):
        self.data = None
        self.var_namespace = None
        self.var_header_guard = None
        self.var_data_name = None
        self.var_data_size = None
        self.metadata = dict()

    def pack(self):
        # indent
        indent = 0

        # source code text
        text = ''

        # header
        if not self.var_header_guard:
            text += '#pragma once\n'
        else:
            text += '#ifndef {}\n'.format(self.var_header_guard)
            text += '#define {}\n'.format(self.var_header_guard)
        text += '\n'

        # includes
        if not self.var_namespace:
            text += '#include <stdint.h>\n'
        else:
            text += '#include <cstdint>\n'
        text += '\n'

        # non-ardiuno guard
        text += '#include <Arduino.h>\n'
        text += '\n'

        # namesapce
        if self.var_namespace:
            indent += 4
            for namespace in self.var_namespace.split('::'):
                text += 'namespace {} {}\n'.format(namespace, '{')
            text += '\n'

        # metadata
        for var_name, value in self.metadata:
            if isinstance(value, str):
                var_type = 'char *'
                var_value = '"{}";'.format(value)
            elif isinstance(value, int):
                var_type = 'size_t'
                var_value = '{};  // 0x{:08x}'.format(value, value)
            else:
                raise NotImplementedError()
            text += '{}const {} {} = {}\n'.format(' ' * indent, var_type, var_name, var_value)

        # size
        text += '{}const size_t {} = {};  // 0x{:08x}\n'.format(' ' * indent, self.var_data_size, len(self.data), len(self.data))

        # data
        col = 0
        text += '{}const char {}[{}] PROGMEM  = {}\n'.format(' ' * indent, self.var_data_name, len(self.data), '{')
        indent += 4
        text += ' ' * indent
        lines = list()
        for data_byte in self.data:
            line = "'\\x{:02x}',".format(data_byte)
            col += 1
            if col == 16:
                col = 0
                line += '\n{}'.format(' ' * indent)
            lines.append(line)
        text += ''.join(lines)
        text += '};\n'
        text += '\n'

        # namesapce
        if self.var_namespace:
            for namespace in self.var_namespace.split('::'):
                text += '{} // namespace {}\n'.format('}', namespace)
            text += '\n'

        # footer
        if self.var_header_guard:
            text += '#endif //{}\n'.format(self.var_header_guard)
        return text.encode('ascii')


class Png(object):

    @classmethod
    def parse(cls, data):
        obj = cls()
        obj.set_raw(data)
        return obj

    def __init__(self):
        self.img = None

    def set_raw(self, data):
        self.img = Image.open(io.BytesIO(data))
        try:
            r, g, b, a = self.img.split()
        except:
            img_new = Image.new('RGBA', self.img.size, (255, 255, 255, 255))
            img_new.paste(self.img, None)
            self.img = img_new
        r, g, b, a = self.img.split()

    @property
    def width(self):
        width, height = self.img.size
        return width

    @property
    def height(self):
        width, height = self.img.size
        return height

    def force_color16(self):
        width, height = self.img.size
        for y in range(0, height):
            for x in range(0, width):
                xy = (x, y)
                r, g, b, a = self.img.getpixel(xy)
                color16 = create_color16(r, g, b)
                r, g, b = split_color16(color16)
                if a < 0x7f:
                    a = 0
                else:
                    a = 255
                if a == 0:
                    r, g, b = (0, 0, 0)
                self.img.putpixel(xy, (r, g, b, a))

    def create_palette(self):
        palette = list()
        width, height = self.img.size
        for y in range(0, height):
            for x in range(0, width):
                xy = (x, y)
                r, g, b, a = self.img.getpixel(xy)
                color = (r, g, b, a)
                if color not in palette:
                    palette.append(color)
        assert len(palette) != 0
        #for r, g, b, a in palette:
        #    print(hex(create_color16(r, g, b)) + ' ' + hex(a))
        #print(len(palette))
        bits = self.bits_per_pixel_for_palette(palette)
        mask = (1 << bits) - 1
        while len(palette) - 1 < mask:
            palette.append((0, 0, 0, 0))
        assert bits == self.bits_per_pixel_for_palette(palette)
        return palette

    def bits_per_pixel_for_palette(self, palette):
        count = len(palette)
        bits = 0
        while True:
            mask = (1 << bits)
            if (count - 1) < mask:
                break
            bits += 1
        assert bits != 0
        if bits == 1:
            return 1
        if bits <= 2:
            return 2
        if bits <= 4:
            return 4
        if bits <= 8:
            return 8
        raise NotImplementedError

    def pack_data_for_palette(self, palette):
        # create a palette color-to-index lookup table
        palette_lookup = dict()
        index = 0
        for r, g, b, a in palette:
            if (r, g, b, a) not in palette_lookup.keys():
                palette_lookup[(r, g, b, a)] = index
            index += 1

        # get the palette index for each pixel
        palette_index_list = list()
        width, height = self.img.size
        for y in range(0, height):
            for x in range(0, width):
                xy = (x, y)
                r, g, b, a = self.img.getpixel(xy)
                palette_index_list.append(palette_lookup[(r, g, b, a)])

        # calculate bits per pixel and the mask for those bits
        bits = self.bits_per_pixel_for_palette(palette)
        mask = (1 << bits) - 1

        # pack
        data = list()
        values = list()
        values_per_byte = int(8 / bits)
        for palette_index in palette_index_list:
            assert palette_index == palette_index & mask
            values.append(palette_index)
            if len(values) == values_per_byte:
                value = 0
                for i in range(0, values_per_byte):
                    shift = ((values_per_byte - 1 - i) * bits)
                    value |= values[i] << shift
                data.append(struct.pack('B', value))
                values = list()
        assert len(values) == 0
        assert len(data) * values_per_byte == len(palette_index_list)
        data = b''.join(data)
        assert len(data) * values_per_byte == len(palette_index_list)
        return data

def pack_assets(assets, gen):
    file_id = 0
    for root, folders, files in os.walk(assets):
        for file in files:
            #print('----------------')
            path = os.path.join(root, file)
            arc = path[len(assets) + 1:]
            out = os.path.join(gen, arc).replace('.', '_') + '.h'
            file_id += 1
            if not os.path.exists(os.path.dirname(out)):
                os.makedirs(os.path.dirname(out))
            assert os.path.exists(os.path.dirname(out))
            var_arc = arc.upper().replace('/', '_').replace('.', '_').replace(' ', '_')
            if arc.endswith('.txt'):
                header = HexDataHeader()
                with open(path, 'rb') as handle:
                    header.data = handle.read()
                header.var_header_guard = '_H_{}_H_'.format(var_arc.upper())
                header.var_data_name = '{}_data'.format(var_arc.lower())
                header.var_data_size = '{}_size'.format(var_arc.lower())
                header.metadata = [('{}_id'.format(var_arc.lower()), file_id)]
                with open(out, 'wb') as handle:
                    handle.write(header.pack())
            if arc.endswith('.png'):
                with open(path, 'rb') as handle:
                    data = handle.read()
                png = Png.parse(data)

                png.force_color16()
                palette = png.create_palette()
                palette_size = len(palette)
                palette_bits = png.bits_per_pixel_for_palette(palette)
                data = b''
                for r, g, b, a in palette:
                    data += create_color16be(r, g, b)
                    #print((r, g, b, a))
                    #print(hex(create_color16(r, g, b)))
                for r, g, b, a in palette:
                    data += struct.pack('B', a)
                data += png.pack_data_for_palette(palette)

                header = HexDataHeader()
                header.data = data
                header.var_header_guard = '_H_{}_H_'.format(var_arc.upper())
                header.var_data_name = '{}_data'.format(var_arc.lower())
                header.var_data_size = '{}_size'.format(var_arc.lower())
                header.metadata = [
                    ('{}_id'.format(var_arc.lower()), file_id),
                    ('{}_width'.format(var_arc.lower()), png.width),
                    ('{}_height'.format(var_arc.lower()), png.height),
                    ('{}_palette_size'.format(var_arc.lower()), palette_size),
                    ('{}_palette_bits'.format(var_arc.lower()), palette_bits),
                ]
                with open(out, 'wb') as handle:
                    handle.write(header.pack())
                #preview = out.replace('.h', '.bmp')
                #with open(preview, 'wb') as handle:
                #    handle.write(png.pack_bmp())
            print(arc)

def pack_config(assets, gen):
    config_h = os.path.join(gen, 'config.h')
    with open(config_h, 'wt') as handle:
        def out(text):
            handle.write(text + '\n')
        out('#ifndef _H_GEN_CONFIG_H_')
        out('#define _H_GEN_CONFIG_H_')
        out('\n')
        level_txt = os.path.join(assets, 'level.txt')
        tile_size = 8
        level_width = 0
        level_height = 0
        with open(level_txt, 'rt') as level:
            for line in level.readlines():
                line = line.strip()
                if line == '':
                    continue
                level_height += 1
                if len(line) > level_width:
                    level_width = len(line)
        out('#define GEN_CONFIG_LEVEL_WIDTH ({})'.format(level_width))
        out('#define GEN_CONFIG_LEVEL_HEIGHT ({})'.format(level_height))
        screen_width = level_width * tile_size
        screen_height = (3 + level_height + 2) * tile_size
        out('#define GEN_CONFIG_SCREEN_WIDTH ({})'.format(screen_width))
        out('#define GEN_CONFIG_SCREEN_HEIGHT ({})'.format(screen_height))
        out('\n')
        out('#endif // _H_GEN_CONFIG_H_')

def main():
    root = os.path.realpath(os.path.dirname(__file__))
    assets = os.path.join(root, 'assets')
    src = os.path.join(root, 'src')
    gen = os.path.join(src, 'gen')
    if os.path.exists(gen):
        shutil.rmtree(gen)
    if not os.path.exists(gen):
        os.makedirs(gen)
    assert os.path.exists(assets)
    assert os.path.exists(gen)
    pack_assets(assets, gen)
    pack_config(assets, gen)

if __name__ == '__main__':
    main()
