import argparse
import os
import pathlib
import re
import sys
try:
    from PIL import Image
except:
    raise Exception("Please install PIL e.g. 'sudo apt-get install python3-pil'")
import struct
import io
import shutil

SCRIPT_DIR = pathlib.Path(__file__).parent

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

        # arduino
        text += '#include <Arduino.h>'
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

def bmp_to_wbr_4bit_color(data):
    # parse image into RGB
    img = Image.open(io.BytesIO(data))
    img_new = Image.new('RGB', img.size, (255, 255, 255))
    img_new.paste(img, None)
    img = img_new
    r, g, b = img.split()

    # pack to WHITE, BLACK, RED (eink)
    wbr_4bit_color_list = list()
    img_grey = img.convert('L')
    width, height = img.size
    for y in range(0, height):
        for x in range(0, width):
            xy = (x, y)
            grey = img_grey.getpixel(xy)
            r, g, b = img.getpixel(xy)
            if grey < 50:
                wbr_4bit_color = 0  # BLACK
            elif grey > 200:
                wbr_4bit_color = 3  # WHITE
            else:
                wbr_4bit_color = 1  # RED
            wbr_4bit_color_list.append(wbr_4bit_color)
    while len(wbr_4bit_color_list) % 4 != 0:
        wbr_4bit_color_list.append(0)

    # 4bit colors to bytes
    values = list()
    for i in range(0, len(wbr_4bit_color_list), 4):
        value = 0
        for j in range(0, 4):
            wbr_4bit_color = wbr_4bit_color_list[i + j]
            value |= wbr_4bit_color << (j * 2)
        values.append(value)
    data = b''.join([struct.pack('<B', value) for value in values])
    assert len(data) * 4 == len(wbr_4bit_color_list)

    return (width, height , data)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--src-dir", "-s", type=pathlib.Path, default=SCRIPT_DIR / "assets",
    )
    parser.add_argument(
        "--dest-dir", "-d", type=pathlib.Path,
        default=SCRIPT_DIR / "src" / "asset",
    )
    parser.add_argument(
        "--clean", "-c", action="store_true",
        help="Clean existing packed assets",
    )
    args = parser.parse_args()
    # Clean up the working tree if told to do so
    if args.clean and args.dest_dir.exists():
        shutil.rmtree(args.dest_dir)
    # Prepare the destination directory
    os.makedirs(args.dest_dir, exist_ok=True)
    _slug_patt = re.compile("[/\.]")
    slugify = lambda s: _slug_patt.sub("_", s)
    for root, folders, files in os.walk(args.src_dir):
        for file_ in files:
            file_ = pathlib.Path(root, file_)
            print(file_.name)
            s_relpath = file_.relative_to(args.src_dir)
            d_absdir = args.dest_dir / s_relpath.parent
            os.makedirs(d_absdir, exist_ok=True)
            d_abspath = d_absdir / (slugify(file_.name) + ".h")
            var_arc = 'ASSET_' + slugify(file_.name)
            if file_.suffix in {".bmp", }:
                with file_.open("rb") as handle:
                    data = handle.read()
                width, height, data = bmp_to_wbr_4bit_color(data)
                header = HexDataHeader()
                header.data = data
                header.var_header_guard = '_H_{}_H_'.format(var_arc.upper())
                header.var_data_name = '{}_data'.format(var_arc.lower())
                header.var_data_size = '{}_size'.format(var_arc.lower())
                header.metadata = [
                    ('{}_bpp'.format(var_arc.lower()), 2),
                    ('{}_width'.format(var_arc.lower()), width),
                    ('{}_height'.format(var_arc.lower()), height),
                ]
                with d_abspath.open("wb") as handle:
                    handle.write(header.pack())

if __name__ == '__main__':
    main()
