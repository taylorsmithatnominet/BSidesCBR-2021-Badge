#!/usr/bin/env python3
import argparse
import binascii
import collections.abc
import contextlib
import errno
import itertools
import json
import logging
import os
import pickle
import platform
import queue
import shutil
import subprocess
import sys
import threading
import time
from multiprocessing import Process, cpu_count

import serial.tools.list_ports as serial_list_ports

from serial import Serial as PySerial
from serial import serialutil

root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
external = os.path.join(root, 'external')
lib_path = os.path.join(root, 'arduino-libs')
out = os.path.join(root, 'out')


class ProcStreamReader(object):
    __STREAMS = {"stdout", "stderr"}

    def __init__(self, p, handlers=None):
        self.p = p
        self.__qs = {n: queue.Queue() for n in self.__STREAMS}
        if handlers is None:
            handlers = dict()
        _handlers = {
            n: tuple(e for e in (self.__qs[n].put, handlers.get(n)) if e)
            for n in self.__STREAMS
        }
        for n in self.__STREAMS:
            threading.Thread(
                target=self.__qloop, args=(getattr(self.p, n), _handlers[n])
            ).start()

    @staticmethod
    def __qloop(fobj, handlers):
        for l in map(str.strip, fobj or tuple()):
            for h in handlers:
                h(l)

    def __get_line(self, fobj_name):
        try:
            return self.__qs[fobj_name].get(timeout=1)
        except queue.Empty:
            return None

    def stdout_read_line(self):
        return self.__get_line("stdout")

    def stderr_read_line(self):
        return self.__get_line("stderr")


class Logger(object):

    def __init__(self):
        root = logging.getLogger()
        root.setLevel(logging.DEBUG)
        handler = logging.StreamHandler(sys.stderr)
        handler.setLevel(logging.DEBUG)
        formatter = logging.Formatter('[%(asctime)s] [%(name)s/%(levelname)s] %(message)s',
                                      '%Y-%m-%d %H:%M:%S')
        handler.setFormatter(formatter)
        root.addHandler(handler)
        self.logger = root
        self.scope_name = None
        self.scopes = []

    def __call__(self, scope_name):
        self.scope_name = scope_name
        return self

    def __enter__(self):
        scope_name = self.scope_name
        self.scopes.append(scope_name)
        scopes_dedup = list()
        for scope in self.scopes:
            if scope not in scopes_dedup:
                scopes_dedup.append(scope)
        self.set_name('/'.join(scopes_dedup))
        return self

    def __exit__(self, *args, **kwargs):
        self.scopes = self.scopes[:-1]
        scopes_dedup = list()
        for scope in self.scopes:
            if scope not in scopes_dedup:
                scopes_dedup.append(scope)
        self.set_name('/'.join(scopes_dedup))
        return

    def set_name(self, name):
        self.logger.name = name

    def debug(self, *args, **kwargs):
        self.logger.debug(*args, **kwargs)

    def info(self, *args, **kwargs):
        self.logger.info(*args, **kwargs)

    def warn(self, *args, **kwargs):
        self.logger.warning(*args, **kwargs)

    def warning(self, *args, **kwargs):
        self.logger.warning(*args, **kwargs)

    def error(self, *args, **kwargs):
        self.logger.error(*args, **kwargs)

    def command(self, *args, **kwargs):
        self.logger.info("Running %r", args[0])
        kwargs.setdefault('stdout', subprocess.PIPE)
        kwargs.setdefault('stderr', subprocess.PIPE)
        kwargs.setdefault('universal_newlines', True)
        kwargs.setdefault('bufsize', 0)
        env = os.environ.copy()
        env.update(kwargs.pop('env', None) or {})
        process = subprocess.Popen(*args, **kwargs, env=env)
        ProcStreamReader(process, handlers={
            "stdout": self.logger.info, "stderr": self.logger.error,
        })
        # If we don't specify a timeout, this tends to confuse the logger and
        # end up blocking forever - probably something to do with our reader
        # threads
        return process.wait(timeout=600)

log = Logger()

class Wsl(object):

    def __init__(self):
        pass

    def is_wsl(self):
        with log('win'):
            if os.name == 'nt':
                return False
            if os.path.exists('/mnt/c/Windows'):
                return True
            if os.environ.get('WSL_DISTRO_NAME'):
                raise Exception("Please restart WSL with 'wsl.exe --shutdown' in command prompt")
            return False

    def comports(self):
        with log('win'):
            cmd = 'import serial.tools.list_ports; import pickle; import binascii; print(binascii.b2a_base64(pickle.dumps(list(serial.tools.list_ports.comports()))).decode())'
            out = subprocess.check_output(['py.exe', '-3', '-c', cmd]).decode().strip()
            return pickle.loads(binascii.a2b_base64(out))

    @property
    def environ(self):
        with log('win'):
            _environ = dict()
            out = subprocess.check_output(['cmd.exe', '/c', 'set'], stderr=subprocess.DEVNULL).decode().strip()
            for line in out.split('\n'):
                line = line.strip()
                if line == '':
                    continue
                items = line.split('=')
                key = items[0]
                value = '='.join(items[1:])
                _environ[key] = value
            return _environ

    def command(self, args):
        with log('win'):
            if args[0] == 'python3':
                new_args = ['py.exe', '-3']
                new_args.extend(args[1:])
                return log.command(new_args)
            args_pickle_hex = binascii.b2a_base64(pickle.dumps(args)).decode().strip()
            cmd = "import sys; import subprocess; import pickle; import binascii; "
            cmd += "args = pickle.loads(binascii.a2b_base64('{}')); ".format(args_pickle_hex)
            cmd += "p = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE); "
            cmd += "a, b = p.communicate(); print(a.decode()); print(b.decode()); sys.exit(p.returncode); "
            args = ['py.exe', '-3', '-c', cmd]
            return log.command(args)

    def copy_to_windows(self, path):
        with log('win'):
            if not os.path.exists('/mnt/c/tmp'):
                os.mkdir('/mnt/c/tmp')
            name = os.path.basename(path)
            dst = os.path.join('/mnt/c/tmp', name)
            winpath = 'c:\\tmp\\' + name
            if os.path.exists(dst):
                os.remove(dst)
            shutil.copyfile(path, dst)
            assert os.path.exists(dst)
            return winpath

    def path_on_windows(self, path):
        with log('win'):
            path = os.path.realpath(path)
            share = '\\\\wsl$\\{}'.format(os.environ['WSL_DISTRO_NAME'])
            path = path.replace('~/', '{}/'.format(os.environ['HOME']))
            path = path.replace('/', '\\')
            path = share + path
            return path

    def path_in_wsl(self, path):
        path = path.replace('\\', '/')
        if path.lower().startswith('//wsl$'):
            path = '/' + '/'.join(path.split('/')[4:])
        return path

wsl = Wsl()

class SerialPortNotFoundError(Exception):
    pass


class Serial(object):

    def __init__(self, serial=None):
        self.ports = None
        self.serial = serial
        self.line = None

    def clear_cache(self):
        self.ports = None

    def comports(self):
        with log('serial'):
            if isinstance(self.ports, list):
                return self.ports
            if not wsl.is_wsl():
                self.ports = list(serial_list_ports.comports())
            else:
                self.ports = wsl.comports()
            for port in self.ports:
                if port.device.startswith('c'):
                    port.device = port.device.upper()
            return self.ports

    def find(self, description):
        with log('serial'):
            for port in self.comports():
                if port.description.lower().find(description.lower()) != -1:
                    log.info('{} {}'.format(port.device, port.description))
                    return port.device
        raise SerialPortNotFoundError

    def __call__(self, *args, **kwargs):
        serial = PySerial(*args, **kwargs, timeout=1)
        return self.__class__(serial)

    def __enter__(self, *args, **kwargs):
        self.serial.__enter__(*args, **kwargs)
        return self

    def __exit__(self, *args, **kwargs):
        return self.serial.__exit__(*args, **kwargs)

    def read(self, size=1, block=True):
        if block:
            return self.serial.read(size)
        data = b''
        while self.serial.in_waiting and len(data) < size:
            data += self.serial.read(1)
        if len(data) == 0:
            return None
        return data

    def readline(self, block=True):
        if block:
            data = b''
            while not data.endswith(b'\r\n'):
                data += self.read()
            return data
        data = b''
        if self.line:
            data = self.line
            self.line = None
        while True:
            x = self.read(block=False)
            if x == None:
                break
            data += x
            if data.endswith(b'\r\n'):
                break
        if data.endswith(b'\r\n'):
            return data
        self.line = data
        return None

serial = Serial()

class FakeSerial(object):

    def __init__(self):
        self.line = None

    def __call__(self, *args, **kwargs):
        return self.__class__()

    def __enter__(self, *args, **kwargs):
        return self

    def __exit__(self, *args, **kwargs):
        return

    def read(self, size=1, block=True):
        if block:
            raise NotImplementedError("Would block forever, this is a fake device")
        return None

    def readline(self, block=True):
        if block:
            data = b''
            while not data.endswith(b'\r\n'):
                data += self.read()
            return data
        data = b''
        if self.line:
            data = self.line
            self.line = None
        while True:
            x = self.read(block=False)
            if x == None:
                break
            data += x
            if data.endswith(b'\r\n'):
                break
        if data.endswith(b'\r\n'):
            return data
        self.line = data
        return None


class Git(object):

    def __init__(self, path='git'):
        self.path = path

    def check(self):
        with log('git'):
            # Confirm that we have `git` at all
            assert log.command([self.path, '--version']) == 0
            # Confirm that we can contact a remote
            if log.command(
                [self.path, 'ls-remote'], stdout=subprocess.DEVNULL,
                start_new_session=True,
            ) != 0:
                log.error(
                    "Unable to connect to the git remote - "
                    "skipping submodule update checks."
                )
                return False
        return True

    def init_submodules(self, *paths):
        with log('git'):
            log.info("Updating submodules...please wait..")
            args = [
                self.path, 'submodule', 'update', '--init', '--recursive',
                "--", *paths,
            ]
            if 0 != log.command(args):
                raise Exception(' '.join(args))

git = Git()


class GitLFS(object):
    def __init__(self, path="git-lfs"):
        self.path = path

    def check(self):
        with log("git-lfs"):
            # Confirm that we have `git-lfs` at all
            if not log.command([self.path, "--version"]):
                log.error("git-lfs program not found - unable to proceed")
                raise FileNotFoundError(self.path)
            # We prefer for the user to have installed the hooks as well but
            # will only warn them if they haven't
            if not log.command([Git.path, "lfs", "--version"]):
                log.warning(
                    "git-lfs hooks not installed - "
                    "your working tree will be marked as changed when "
                    "LFS pointers are replaced with their corresponding files."
                )

    def checkout(self):
        with log("git-lfs"):
            for action in {"fetch", "checkout"}:
                log.info("%s LFS files", action.title())
                cmd = (self.path, action)
                rv = log.command(cmd)
                if rv != 0:
                    log.error("Failed to %s LFS files", action)
                    raise subprocess.CalledProcessError(rv, cmd)

lfs = GitLFS()


class CMake(object):

    def __init__(self, path='cmake', default='Unix Makefiles'):
        self.path = path
        self.default = default

    def check(self):
        with log('cmake'):
            log.command([self.path, '--version'])

    def generate(
        self, path, out,
        generator=None, toolchain=None, debug=False,
        defines=None, environ=None,
    ):
        with log('cmake'):
            if not generator:
                generator = cls.default
            if toolchain:
                toolchain = os.path.abspath(toolchain)
                if not os.path.exists(toolchain):
                    raise FileNotFoundError(toolchain)
            path = os.path.abspath(path)
            if not os.path.exists(path):
                raise FileNotFoundError(path)
            if not os.path.exists(out):
                os.makedirs(out)
            assert os.path.exists(out)
            if not defines:
                defines = list()
            if toolchain:
                defines.append(('CMAKE_TOOLCHAIN_FILE', toolchain))
            defines.append(
                ("CMAKE_BUILD_TYPE", "Debug" if debug else "Release")
            )
            args = ['cmake']
            for key, value in defines:
                define = '-D{}'.format(key)
                if value:
                    define = '{}={}'.format(define, value)
                args.append(define)
            args.extend(['-G', generator])
            args.append(path)
            if 0 != log.command(args, env=environ, cwd=out):
                raise Exception(' '.join(args))

cmake = CMake()

class Make(object):

    def __init__(self, path='make'):
        self.path = path

    def check(self):
        with log('make'):
            log.command([self.path, '--version'])

    def build(self, path, environ=None, cores=None):
        with log('make'):
            if not os.path.exists(path):
                raise FileNotFoundError(path)
            args = ['make']
            if cores:
                args.append('-j{}'.format(cores))
            assert 0 == log.command(args, cwd=path, env=environ)

make = Make()

class Wget(object):

    def __init__(self, path='wget'):
        self.path = path

    def check(self):
        with log('wget'):
            log.command([self.path, '--version'])

    def download(self, url, out):
        with log('wget'):
            if os.path.exists(out):
                os.remove(out)
            args = ['wget', '-O', out, url]
            p = subprocess.Popen(args)
            p.communicate()
            if 0 != p.returncode:
                raise Exception(' '.join(args))

wget = Wget()

class Tar(object):

    def __init__(self, path='tar'):
        self.path = path

    def check(self):
        with log('tar'):
            log.command([self.path, '--version'])

    def extract(self, path, out=None):
        with log('tar'):
            if out:
                raise NotImplementedError
            path = os.path.abspath(path)
            if not os.path.exists(path):
                raise FileNotFoundError(path)
            cwd = os.path.dirname(path)
            args = ['tar', 'xf', path]
            p = subprocess.Popen(args, cwd=cwd)
            p.communicate()
            if 0 != p.returncode:
                raise Exception(' '.join(args))

tar = Tar()

class XtensaTools(object):
    URL = 'https://dl.espressif.com/dl/xtensa-esp32-elf-gcc8_2_0-esp32-2019r1-rc2-linux-amd64.tar.xz'

    def __init__(self):
        self.url = self.URL
        self.xtena_sdk = os.path.join(external, 'xtensa-esp32-elf')
        self.bin = os.path.join(self.xtena_sdk, 'bin')
        self.gcc = os.path.join(self.bin, 'xtensa-esp32-elf-gcc')
        self.gdb = os.path.join(self.bin, 'xtensa-esp32-elf-gdb')
        self.elf_size = os.path.join(self.bin, 'xtensa-esp32-elf-size')

    def check(self):
        with log('xtensa'):
            log.command([self.gcc, '--version'])

    def download(self):
        with log('xtensa'):
            log.info('Downloading xtensa tools for esp32...please wait...')
            xtensa_tar_gz = os.path.join(external, 'xtensa.tar.gz')
            if not os.path.exists(self.gcc):
                wget.download(self.url, xtensa_tar_gz)
                tar.extract(xtensa_tar_gz)
            if os.path.exists(xtensa_tar_gz):
                os.remove(xtensa_tar_gz)
            if not os.path.exists(self.gcc):
                raise Exception("Failed to download xtensa toolchain to {}".format(self.xtensa_sdk))
            self.patch()

    def patch(self):
        with log('xtensa'):
            find_pattern = '#ifndef _STDLIB_H_\n#define _STDLIB_H_'
            replace_pattern = '\n'
            replace_pattern += '\n#define __alloc_align(x)    __attribute__((__alloc_align__(x)))'
            replace_pattern += '\n#define __alloc_size(x)    __attribute__((__alloc_size__(x)))'
            replace_pattern += '\n#define __result_use_check  __attribute__((__warn_unused_result__))'
            stdlib_h = os.path.join(self.xtena_sdk, 'xtensa-esp32-elf', 'sys-include', 'stdlib.h')
            with open(stdlib_h, 'rt') as handle:
                text = handle.read()
            if text.count(find_pattern) == 1 and text.count(replace_pattern) == 0:
                text = text.replace(find_pattern, find_pattern + replace_pattern)
                log.info('esp32 stdlib.h has been patched')
            assert text.count(find_pattern) == 1 and text.count(find_pattern + replace_pattern) == 1
            log.info('esp32 stdlib.h patch found')
            with open(stdlib_h, 'wt') as handle:
                handle.write(text)
            find_pattern = 'long	random (void);'
            replace_pattern = '//long random (void) ;'
            stdlib_h = os.path.join(self.xtena_sdk, 'xtensa-esp32-elf', 'sys-include', 'stdlib.h')
            with open(stdlib_h, 'rt') as handle:
                text = handle.read()
            if text.count(find_pattern) == 1 and text.count(replace_pattern) == 0:
                text = text.replace(find_pattern, replace_pattern)
                log.info('esp32 stdlib.h has been patched')
            assert text.count(find_pattern) == 0 and text.count(replace_pattern) == 1
            log.info('esp32 stdlib.h patch found')
            with open(stdlib_h, 'wt') as handle:
                handle.write(text)

    def sections(self, elf):
        sections = list()
        assert os.path.exists(self.elf_size)
        assert os.path.exists(elf)
        p = subprocess.Popen([self.elf_size, '-A', elf], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
        stdout, _ = p.communicate()
        text = stdout.decode()
        text = text.replace('\t', ' ')
        while text.find('  ') != -1:
            text = text.replace('  ', ' ')
        for line in text.split('\n'):
            line = line.replace('\r', '')
            line = line.strip()
            if line == '':
                continue
            if line.startswith('/') or line.startswith('\\'):
                continue
            if line.lower().startswith('total'):
                continue
            if line.lower().startswith('section'):
                continue
            section, size, address = line.split(' ')
            size = int(size)
            address = int(address)
            sections.append((section, size, address))
        return sections

xtensa = XtensaTools()

class ArduinoSdk(object):
    URL = 'https://downloads.arduino.cc/arduino-1.8.10-linux64.tar.xz'

    def __init__(self):
        self.url = self.URL
        self.sdk = os.path.join(external, 'arduino-1.8.10')
        self.bin = os.path.join(self.sdk, 'hardware', 'tools', 'avr', 'bin')
        self.gcc = os.path.join(self.bin, 'avr-gcc')
        self.toolchain_cmake = os.path.join(external, 'arduino-cmake', 'toolchain-atmega328p.cmake')
        self.toolchain_samd21_cmake = os.path.join(external, 'arduino-cmake', 'toolchain-samd21.cmake')

    def check(self):
        with log('arduino'):
            log.command([self.gcc, '--version'])

    def download(self):
        with log('arduino'):
            log.info('Downloading Arduino SDK ...please wait...')
            arduino_tar_gz = os.path.join(external, 'arduino-1.8.10-linux64.tar.xz')
            if not os.path.exists(self.gcc):
                wget.download(self.url, arduino_tar_gz)
                tar.extract(arduino_tar_gz)
            if os.path.exists(arduino_tar_gz):
                os.remove(arduino_tar_gz)
            if not os.path.exists(self.gcc):
                raise Exception("Failed to download Arduino SDK to {}".format(self.sdk))

arduino = ArduinoSdk()


class AvrDude(object):

    def __init__(self, path='avrdude'):
        self.path = path
        if os.name == 'nt' or wsl.is_wsl():
            self.path += '.exe'
        self.sdk = arduino.sdk
        if wsl.is_wsl():
            self.sdk = wsl.environ.get('ARDUINO_SDK', None)
            if not self.sdk:
                self.sdk = r'C:\Program Files (x86)\Arduino'
        self.bin = os.path.join(self.sdk, 'hardware', 'tools', 'avr', 'bin')
        self.conf = os.path.join(self.sdk, 'hardware', 'tools', 'avr', 'etc', 'avrdude.conf')
        self.path = os.path.join(self.bin, self.path)
        if wsl.is_wsl():
            self.path = self.path.replace('/', '\\')
            self.conf = self.conf.replace('/', '\\')

    def flash(self, device, programmer, comport, baudrate, path):
        with log('avrdude'):
            if wsl.is_wsl():
                path = wsl.copy_to_windows(path)
            args = list()
            args.extend([self.path, '-C', self.conf, '-s', '-v'])
            args.extend(['-p', device])
            args.extend(['-c', programmer])
            args.extend(['-P', comport])
            args.extend(['-b', str(baudrate)])
            args.extend(['-U', 'flash:w:{}:i'.format(path)])
            if wsl.is_wsl():
                assert 0 == wsl.command(args)
            else:
                assert 0 == log.command(args)

avrdude = AvrDude()

class ArduinoArmTools(object):
    URL = 'http://downloads.arduino.cc/tools/gcc-arm-none-eabi-7-2017-q4-major-linux64.tar.bz2'

    def __init__(self):
        self.url = self.URL
        self.path = os.path.join(external, 'arm-none-eabi-gcc')
        self.bin = os.path.join(self.path, 'bin')
        self.gcc = os.path.join(self.bin, 'arm-none-eabi-gcc')

    def check(self):
        with log('arduino-arm'):
            log.command([self.gcc, '--version'])

    def download(self):
        with log('arduino-arm'):
            log.info('Downloading arduino arm-none-eabi-gcc tools...please wait...')
            tools_tar_gz = os.path.join(external, 'gcc-arm-none-eabi-7-2017-q4-major-linux64.tar.bz2')
            tmp_tools_path = os.path.join(external, 'gcc-arm-none-eabi-7-2017-q4-major')
            if not os.path.exists(self.gcc):
                wget.download(self.url, tools_tar_gz)
                tar.extract(tools_tar_gz)
                shutil.move(tmp_tools_path, self.path)
            if os.path.exists(tools_tar_gz):
                os.remove(tools_tar_gz)
            if not os.path.exists(self.gcc):
                raise Exception("Failed to download arduino arm-none-eabi-gcc toolchain to {}".format(self.path))

arduino_arm_tools = ArduinoArmTools()

class ArduinoSAMD(object):

    def __init__(self):
        self.path = os.path.join(external, 'arduino-samd')

arduino_samd = ArduinoSAMD()

class CMSISAtmel(object):
    URL = 'https://downloads.arduino.cc/CMSIS-Atmel-1.2.0.tar.bz2'

    def __init__(self):
        self.url = self.URL
        self.path = os.path.join(external, 'CMSIS-Atmel')
        self.include = os.path.join(self.path, 'CMSIS', 'Device', 'ATMEL')

    def check(self):
        with log('cmsis-atmel'):
            if not os.path.exists(self.include):
                raise FileNotFoundError(self.include)
            log.info('Found')

    def download(self):
        with log('cmsis-atmel'):
            log.info('Downloading CMSIS-Atmel...please wait...')
            tools_tar_gz = os.path.join(external, 'CMSIS-Atmel-1.2.0.tar.bz2')
            if not os.path.exists(self.include):
                wget.download(self.url, tools_tar_gz)
                tar.extract(tools_tar_gz)
            if os.path.exists(tools_tar_gz):
                os.remove(tools_tar_gz)
            if not os.path.exists(self.include):
                raise Exception("Failed to download CMSIS-Atmel to {}".format(self.path))

cmsis_atmel = CMSISAtmel()

class CMSIS(object):
    URL = 'http://downloads.arduino.cc/CMSIS-4.5.0.tar.bz2'

    def __init__(self):
        self.url = self.URL
        self.path = os.path.join(external, 'CMSIS')
        self.include = os.path.join(self.path, 'CMSIS', 'Include')

    def check(self):
        with log('cmsis'):
            if not os.path.exists(self.include):
                raise FileNotFoundError(self.include)
            log.info('Found')

    def download(self):
        with log('cmsis'):
            log.info('Downloading CMSIS...please wait...')
            tools_tar_gz = os.path.join(external, 'CMSIS-4.5.0.tar.bz2')
            if not os.path.exists(self.include):
                wget.download(self.url, tools_tar_gz)
                tar.extract(tools_tar_gz)
            if os.path.exists(tools_tar_gz):
                os.remove(tools_tar_gz)
            if not os.path.exists(self.include):
                raise Exception("Failed to download CMSIS to {}".format(self.path))

cmsis = CMSIS()

class BOSSAC(object):
    URL = "https://github.com/shumatech/BOSSA/releases/download/1.7.0/bossac-1.7.0-x86_64-linux-gnu.tar.gz"
    URL_WSL = "https://github.com/shumatech/BOSSA/releases/download/1.7.0/bossac-1.7.0-mingw32.tar.gz"

    def __init__(self, path='bossac'):
        self.url = self.URL
        if wsl.is_wsl():
            path = 'bossac.exe'
            self.url = self.URL_WSL
        self.path = os.path.join(external, 'bossac')
        self.bin = os.path.join(self.path, path)

    def check(self):
        with log('bossac'):
            if not os.path.exists(self.bin):
                raise FileNotFoundError(self.bin)
            log.info('Found')

    def download(self):
        with log('bossac'):
            log.info('Downloading bossac...please wait...')
            tools_tar_gz = os.path.join(external, self.url.split('/')[-1])
            tmp_tools_path = os.path.join(external, 'bossac-1.7.0')
            if not os.path.exists(self.bin):
                wget.download(self.url, tools_tar_gz)
                tar.extract(tools_tar_gz)
                shutil.move(tmp_tools_path, self.path)
            if os.path.exists(tools_tar_gz):
                os.remove(tools_tar_gz)
            if not os.path.exists(self.bin):
                raise Exception("Failed to download bossac to {}".format(self.path))

    def flash(self, comport, path):
        with log('bossac'):
            bossac = self.bin
            if wsl.is_wsl():
                bossac = wsl.path_on_windows(bossac)
                path = wsl.copy_to_windows(path)
            args = list()
            args.extend([bossac])
            args.extend(['-i', '-d'])
            args.extend(['--port={}'.format(comport)])
            args.extend(['-U', 'true'])
            args.extend(['-i', '-e', '-w', '-v'])
            args.extend([path])
            args.extend(['-R'])
            if wsl.is_wsl():
                assert 0 == wsl.command(args)
            else:
                assert 0 == log.command(args)

bossac = BOSSAC()

class EspMdf(object):

    def __init__(self):
        self.repo = os.path.join(external, 'esp-mdf')
        self.esp_idf = os.path.join(self.repo, 'esp-idf')

    def download(self):
        with log('esp_mdf'):
             git.init_submodules(self.repo)


esp_mdf = EspMdf()


class EspIdf(object):

    def __init__(self):
        self.repo = esp_mdf.esp_idf
        self.toolchain_cmake = os.path.join(self.repo, 'tools', 'cmake', 'toolchain-esp32.cmake')

    def download(self):
        # Pulling down MDF will also download its IDF submodule so we don't
        # have to do much here
        esp_mdf.download()

esp_idf = EspIdf()


class EspTool(object):

    def __init__(self):
        self.path = os.path.join(esp_idf.repo, 'components', 'esptool_py', 'esptool', 'esptool.py')

    def command(self, args):
        with log('esptool'):
            path = self.path
            if wsl.is_wsl():
                path = wsl.copy_to_windows(path)
            new_args = ['python3', path]
            new_args.extend(args)
            args = new_args
            if wsl.is_wsl():
                assert 0 == wsl.command(args)
            else:
                assert 0 == log.command(args)

    def write_flash(self, comport, baudrate, *args):
        # Sanity check that all the file paths exist (every second element in
        # `args` since we alternate `<addr> <path> ...`
        if len(args) % 2 != 0:
            raise TypeError(
                "ESP32 flash arguments must be in <addr> <path> pairs"
            )
        for path_candidate in args[1::2]:
            if not os.path.exists(path_candidate):
                raise FileNotFoundError(
                    "ESP32 file to be flashed does not exist", path_candidate
                )
        with log('esptool'):
            if wsl.is_wsl():
                args = *(
                    v for z in
                    zip(args[::2],(wsl.copy_to_windows(path) for path in args[1::2]))
                    for v in z
                ),
            args = (
                "--port", comport, "--baud", str(baudrate), "write_flash",
                # Blow up `*args` so we can hexlify the addresses and `zip`
                # them back together in order
                *(
                    v for z in
                    zip((hex(addr) for addr in args[::2]),args[1::2])
                    for v in z
                ),
            )
            return self.command(args)

esptool = EspTool()


class Esp32(object):
    NAME = "esp32"
    _DEFAULT_PORTS = {
        "Linux": "/dev/ttyUSB0",
    }
    DEFAULT_PORT = _DEFAULT_PORTS.get(platform.system()) if not wsl.is_wsl() else None

    def __init__(self):
        self.out = os.path.join(out, 'esp32')
        self.elffile = os.path.join(self.out, 'src', 'badge', 'badge')
        self.binfile = os.path.join(self.out, 'badge.bin')
        self.flasher_args_json = os.path.join(self.out, 'flasher_args.json')
        self.baudrate = 115200
        self.flash_baudrate = 921600
        self.program_storage = 2097152
        self.program_storage_sections = ['.flash']
        self.dynamic_memory = 327680
        self.dynamic_memory_sections = ['.iram0', '.dram0']

    @property
    def comport(self):
        with log('esp32'):
            log.info('Finding esp32 serial device...')
            port = None
            try:
                port = serial.find('silicon labs cp210x usb to uart bridge')
            except SerialPortNotFoundError:
                pass
            if not port:
                try:
                    port = serial.find('silicon labs dual cp2105 usb to uart bridge: enhanced com port')
                except SerialPortNotFoundError:
                    pass
            if not port:
                raise SerialPortNotFoundError('esp32')
            return port

    def clean(self):
        with log('esp32'):
            if os.path.exists(self.out):
                log.info(self.out)
                shutil.rmtree(self.out)

    def generate(self, **kwargs):
        with log('esp32'):
            if not os.path.exists(self.out):
                os.makedirs(self.out)
            environ = os.environ.copy()
            environ['PATH'] = '{}:{}'.format(xtensa.bin, environ['PATH'])
            environ['IDF_PATH'] = esp_idf.repo
            environ['IDF_TARGET'] = 'esp32'
            environ['MDF_PATH'] = esp_mdf.repo
            environ['ARDUINO_ESP32'] = os.path.join(external, 'arduino-esp32')
            cmake.generate(root, 
                self.out,
                generator='Unix Makefiles',
                toolchain=esp_idf.toolchain_cmake,
                environ=environ,
                **kwargs)

    def build(self, cores):
        if not os.path.exists(self.out):
            with log("autogenerate"):
                # `debug` is `False` otherwise we'd have generated already
                self.generate()
        with log('esp32'):
            environ = os.environ.copy()
            environ['PATH'] = '{}:{}'.format(xtensa.bin, environ['PATH'])
            environ['IDF_PATH'] = esp_idf.repo
            environ['IDF_TARGET'] = 'esp32'
            environ['MDF_PATH'] = esp_mdf.repo
            environ['ARDUINO_ESP32'] = os.path.join(external, 'arduino-esp32')
            make.build(self.out, environ=environ, cores=cores)

    def flash(self, comport=None, baudrate=None):
        with log('esp32'):
            path = self.binfile
            with open(self.flasher_args_json, 'rt') as handle:
                flasher_args = json.load(handle)
            partitions = dict()
            for addrstr, rel_path in flasher_args["flash_files"].items():
                addr = int(addrstr, 16)
                assert addr != 0
                path = os.path.join(self.out, rel_path)
                assert os.path.exists(path)
                partitions[addr] = path
            if not comport:
                comport = self.comport
            if not baudrate:
                baudrate = self.flash_baudrate
            esptool.write_flash(
                comport, baudrate,
                *(v for z in partitions.items() for v in z)
            )

    def sketch_info(self):
        with log('esp32'):
            # calculate
            used_program_storage = 0
            used_dynamic_memory = 0
            for section, size, address in xtensa.sections(self.elffile):
                if size == 0 or address == 0:
                    continue
                for program_storage_section in self.program_storage_sections:
                    if section.startswith(program_storage_section):
                        log.info('FLASH : 0x{:08x} 0x{:08x} {}'.format(address, size, section))
                        used_program_storage += size
                for dynamic_memory_section in self.dynamic_memory_sections:
                    if section.startswith(dynamic_memory_section):
                        log.info('MEMORY: 0x{:08x} 0x{:08x} {}'.format(address, size, section))
                        used_dynamic_memory += size

            # stats
            percent_program_storage = int((used_program_storage / self.program_storage) * 100)
            percent_dynamic_memory = int((used_dynamic_memory / self.dynamic_memory) * 100)
            remaining_dynamic_memory = 0
            if used_dynamic_memory < self.dynamic_memory:
                remaining_dynamic_memory = self.dynamic_memory - used_dynamic_memory

            # present
            log.info('Sketch uses {} bytes ({}%) of program storage space. Maximum is {} bytes.'.format(
                used_program_storage, percent_program_storage, self.program_storage))
            log.info('Global variables use {} bytes ({}%) of dynamic memory, leaving {} bytes for local variables. Maximum is {} bytes.'.format(
                used_dynamic_memory, percent_dynamic_memory, remaining_dynamic_memory, self.dynamic_memory))

    def gdb_info_pc(self, pc):
        if os.name == 'nt':
            args = [wsl.path_in_wsl(xtensa.gdb), '--batch', wsl.path_in_wsl(self.elffile)]
        else:
            args = [xtensa.gdb, '--batch', self.elffile]
        args.extend(['-ex', 'set listsize 1'])
        args.extend(['-ex', 'l *0x{:08x}'.format(pc)])
        args.extend(['-ex', 'q'])
        if os.name == 'nt':
            args = ['bash', '-c', ' '.join(["'{}'".format(arg) for arg in args])]
        p = subprocess.Popen(args, stdout=subprocess.PIPE)
        stdout, stderr = p.communicate()
        stdout = stdout.decode('ascii', errors='replace').strip() + '\n\n'
        line0 = stdout.split('\n')[0].strip()
        line1  = stdout.split('\n')[1].strip()
        method = None
        path = None
        lineno = None
        code = None
        if line0.startswith('0x') and line0.count('is in ') > 0:
            method_file_lineno = 'is in '.join(line0.split('is in ')[1:])
            if method_file_lineno.find('(/') == -1:
                method = method_file_lineno.strip()
            else:
                if method_file_lineno.find(')') != -1:
                    method_file_lineno = method_file_lineno.split(')')[0]
                method, path_lineno = method_file_lineno.split('(/')
                method = method.strip()
                if path_lineno.find(':') != -1:
                    path, lineno = path_lineno.split(':')
                    lineno = int(lineno.strip(), 0)
                else:
                    path = path_lineno
                path = path.strip()
        if line1.strip() != '' and line1.count(' ') > 0:
            code = ' '.join(line1.split(' ')[1:]).strip()
        return method, path, lineno, code

    def decode_and_log_backtrace(self, line):
        line = line.replace('\t', ' ')
        while line.count('  ') != 0:
            line = line.replace('  ', ' ')
        line = line.strip()
        with log('esp32'):
            with log('backtrace'):
                for frame in line.split(' '):
                    if frame.count('0x') != 2:
                        continue
                    assert frame.count(':') == 1
                    pc, sp = frame.split(':')
                    pc = int(pc, 16)
                    sp = int(sp, 16)
                    try:
                        method, path, lineno, code = self.gdb_info_pc(pc)
                    except ValueError:
                        method = path = lineno = code = None
                    method = method if method else '??'
                    path = path if path else '??'
                    lineno = lineno if lineno else '??'
                    code = code if code else '??'
                    log.info('0x{:08x} {} at {}:{} "{}"'.format(pc, method, path, lineno, code))


class Atmega328p(object):
    NAME = "atmega328p"

    def __init__(self):
        self.out = os.path.join(out, 'atmega328p')
        self.baudrate = 115200
        self.device = 'atmega328p'
        self.hexfile = os.path.join(self.out, 'badge.hex')

    @property
    def comport(self):
        with log('atmega328p'):
            log.info('Finding atmega328p serial device...')
            port = None
            try:
                port = serial.find('usb-serial ch340')
            except SerialPortNotFoundError:
                pass
            if not port:
                raise SerialPortNotFoundError('atmega328p')
            return port

    def clean(self):
        with log('atmega328p'):
            if os.path.exists(self.out):
                log.info(self.out)
                shutil.rmtree(self.out)

    def generate(self, **kwargs):
        with log('atmega328p'):
            if not os.path.exists(self.out):
                os.makedirs(self.out)
            environ = os.environ.copy()
            environ['PATH'] = '{}:{}'.format(arduino.bin, environ['PATH'])
            environ['ARDUINO_SDK_PATH'] = arduino.sdk
            cmake.generate(root, 
                self.out,
                generator='Unix Makefiles',
                toolchain=arduino.toolchain_cmake,
                environ=environ,
                **kwargs)

    def build(self, cores):
        if not os.path.exists(self.out):
            with log("autogenerate"):
                # `debug` is `False` otherwise we'd have generated already
                self.generate()
        with log('atmega328p'):
            environ = os.environ.copy()
            environ['PATH'] = '{}:{}'.format(arduino.bin, environ['PATH'])
            environ['ARDUINO_SDK_PATH'] = arduino.sdk
            make.build(self.out, environ=environ, cores=cores)

    def flash(self, programmer, comport=None, baudrate=None):
        with log('atmega328p'):
            if not comport:
                comport = self.comport
            if not baudrate:
                baudrate = self.baudrate
            log.info('Flashing...please wait...')
            avrdude.flash(path=self.hexfile,
                comport=comport, baudrate=baudrate,
                device=self.device, programmer=programmer)


class SAMD21(object):
    NAME = "samd21"
    _DEFAULT_PORTS = {
        "Linux": "/dev/ttyACM0",
    }
    DEFAULT_PORT = _DEFAULT_PORTS.get(platform.system()) if not wsl.is_wsl() else None

    def __init__(self):
        self.out = os.path.join(out, 'samd21')
        self.baudrate = 115200
        self.binfile = os.path.join(self.out, 'io_coprocessor.bin')

    @property
    def comport_bootloader(self):
        serial.clear_cache()
        with log('samd21'):
            log.info('Finding samd21 serial device...')
            port = None
            try:
                port = serial.find('MKRZero bootloader')
            except SerialPortNotFoundError:
                pass
            if not port:
                raise SerialPortNotFoundError('samd21 (bootloader serial port)')
            return port

    @property
    def comport(self):
        if wsl.is_wsl():
            time.sleep(2)
        serial.clear_cache()
        with log('samd21'):
            log.info('Finding samd21 serial device...')
            port = None
            try:
                port = serial.find('MKRZero')
            except SerialPortNotFoundError:
                pass
            if not port:
                raise SerialPortNotFoundError('samd21')
            return port

    def clean(self):
        with log('samd21'):
            if os.path.exists(self.out):
                log.info(self.out)
                shutil.rmtree(self.out)

    def generate(self, **kwargs):
        with log('samd21'):
            if not os.path.exists(self.out):
                os.makedirs(self.out)
            environ = os.environ.copy()
            environ['PATH'] = '{}:{}'.format(arduino_arm_tools.bin, environ['PATH'])
            environ['ARDUINO_SDK_PATH'] = arduino.sdk
            environ['ARDUINO_SAMD_PATH'] = arduino_samd.path
            environ['CMSIS_ATMEL_PATH'] = cmsis_atmel.path
            environ['CMSIS_PATH'] = cmsis.path
            cmake.generate(root, 
                self.out,
                generator='Unix Makefiles',
                toolchain=arduino.toolchain_samd21_cmake,
                environ=environ,
                **kwargs)

    def build(self, cores):
        if not os.path.exists(self.out):
            with log("autogenerate"):
                # `debug` is `False` otherwise we'd have generated already
                self.generate()
        with log('samd21'):
            environ = os.environ.copy()
            environ['PATH'] = '{}:{}'.format(arduino_arm_tools.bin, environ['PATH'])
            environ['ARDUINO_SDK_PATH'] = arduino.sdk
            environ['ARDUINO_SAMD_PATH'] = arduino_samd.path
            environ['CMSIS_ATMEL_PATH'] = cmsis_atmel.path
            environ['CMSIS_PATH'] = cmsis.path
            make.build(self.out, environ=environ, cores=cores)

    def flash(self, comport=None):
        with log('samd21'):
            if not comport:
                comport = self.comport_bootloader
            log.info('Flashing...please wait...')
            bossac.flash(path=self.binfile, comport=comport)


class HostLinux(object):
    NAME= "host"

    def __init__(self):
        self.out = os.path.join(out, 'host')
        self.binfile = os.path.join(self.out, 'src', 'badge', 'badge')

    def clean(self):
        with log('host'):
            if os.path.exists(self.out):
                log.info(self.out)
                shutil.rmtree(self.out)

    def generate(self, **kwargs):
        with log('host'):
            if not os.path.exists(self.out):
                os.makedirs(self.out)
            environ = os.environ.copy()
            cmake.generate(root, 
                self.out,
                generator='Unix Makefiles',
                environ=environ,
                **kwargs)

    def build(self, cores):
        if not os.path.exists(self.out):
            with log("autogenerate"):
                # `debug` is `False` otherwise we'd have generated already
                self.generate()
        with log('host'):
            environ = os.environ.copy()
            make.build(self.out, environ=environ, cores=cores)


def flatten(maybe_iter):
    if isinstance(maybe_iter, collections.abc.Iterable):
        for e in maybe_iter:
            yield from flatten(e)
    else:
        yield maybe_iter

TARGETS = {
    # XXX It's not clear if this still works
    #Atmega328p.NAME: Atmega328p(),
    Esp32.NAME: Esp32(),
    SAMD21.NAME: SAMD21(),
    HostLinux.NAME: HostLinux(),
}
TARGETS["2020-badge"] = tuple(TARGETS[k] for k in (Esp32.NAME, SAMD21.NAME))
TARGETS["2020-all"] = tuple(TARGETS[k] for k in ("2020-badge", HostLinux.NAME))
TARGET_ALL = "all"
TARGETS[TARGET_ALL] = TARGETS["2020-all"]
# Ensure all values are naively iterable
for k, v in TARGETS.items():
    TARGETS[k] = tuple(flatten(v))


def download_tools(args):
    arduino.download()
    xtensa.download()
    arduino_arm_tools.download()
    cmsis_atmel.download()
    cmsis.download()
    bossac.download()

def check_tools(args):
    cmake.check()
    make.check()
    wget.check()
    tar.check()
    arduino.check()
    xtensa.check()
    arduino_arm_tools.check()
    cmsis_atmel.check()
    cmsis.check()
    bossac.check()

def download_libs(args):
    if git.check():
        git.init_submodules(external, lib_path)
    esp_mdf.download()
    esp_idf.download()
    # This is a bit cheeky but we'll also pull down LFS files here
    lfs.checkout()

def clean(args):
    for target_obj in TARGETS[args.target]:
        target_obj.clean()
    if args.target == TARGET_ALL:
        if os.path.exists(out):
            log.info(out)
            shutil.rmtree(out)
    if not os.path.exists(out):
        os.makedirs(out)
    assert os.path.exists(out)

def generate(args):
    for target_obj in TARGETS[args.target]:
        target_obj.generate(debug=args.debug)

def build(args):
    tasks_to_run = list()
    for target_obj in TARGETS[args.target]:
        tasks_to_run.append(
            Process(
                name=f"{target_obj.NAME}-build",
                target=target_obj.build, args=(args.j, ),
            )
        )
    for t in tasks_to_run:
        t.start()
        if args.j == 1:
            t.join()
    if args.j > 1:
        for t in tasks_to_run:
            t.join()
    if any(t.exitcode for t in tasks_to_run):
        log.error("Build tasks failed")
        for t in tasks_to_run:
            if t.exitcode:
                log.error("%r failed", t.name)
        sys.exit(1)
    for target_obj in TARGETS[args.target]:
        try:
            target_obj.sketch_info()
        except AttributeError:
            pass

def flash(args):
    for target_obj in TARGETS[args.target]:
        comport_arg = f"{target_obj.NAME}_port"
        try:
            target_obj.flash(comport=getattr(args, comport_arg))
        except AttributeError:
            pass

def monitor(args):
    # The host build gets monitored standalone
    if args.target == HostLinux.NAME:
        with log(HostLinux.NAME):
            log.command(TARGETS[args.target][0].binfile)
        return
    # We need to run a new process under WSL on Windows
    if wsl.is_wsl():
        wsl_args = ['py.exe', '-3', wsl.path_on_windows(__file__)]
        wsl_args.extend(['--monitor'])
        wsl_args.extend(['--target', args.target])
        for target_obj in TARGETS[args.target]:
            comport_arg = f"{target_obj.NAME}_port"
            if comport_arg in args:
                port = getattr(args, comport_arg)
                if not port:
                    port = target_obj.comport
                wsl_args.extend([
                    f"--{target_obj.NAME}-port", port,
                ])
        subprocess.call(wsl_args)
        return
    # Otherwise we build up the serial port objects we need and monitor them
    serial_objs = {}
    for target_obj in TARGETS[args.target]:
        # Skip targets where we have neither a COM port property or an argument
        # to set the COM port
        comport_arg = f"{target_obj.NAME}_port"
        try:
            comport = getattr(args, comport_arg)
        except AttributeError:
            try:
                comport = target_obj.comport
            except AttributeError:
                continue
        # The SAMD21 often takes a bit to come back after being flashed so
        # we'll delay if we get an `ENOENT` or `EPERM`
        try:
            serial_obj = serial(comport, target_obj.baudrate)
        except serialutil.SerialException as exc:
            if (
                isinstance(exc.args[0], int) and
                exc.args[0] in (errno.ENOENT, errno.EPERM)
            ):
                log.warning(
                    "Pausing to wait for %r serial %r",
                    target_obj.NAME, comport,
                )
                time.sleep(2)
                # Allow any exception to bubble up this time
                serial_obj = serial(comport, target_obj.baudrate)
            else:
                raise exc
        serial_objs[target_obj.NAME] = serial_obj

    with contextlib.ExitStack() as ctx:
        serial_ctxs = dict()
        for name, serial_obj in serial_objs.items():
            serial_ctxs[name] = ctx.enter_context(serial_obj)
        while True:
            for n, s in serial_ctxs.items():
                line = s.readline(block=False)
                if line:
                    line = line.decode('ascii', errors='replace').strip()
                    with log(n):
                        log.info(line)
                    # Special handling for for ESP32 backtraces
                    # XXX: This should avoid touching the target directly
                    if line.startswith('Backtrace:'):
                        (esp32, ) = TARGETS[Esp32.NAME]
                        esp32.decode_and_log_backtrace(line)
                else:
                    # Don't churn the CPU so much...
                    time.sleep(0.01)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--target',
                        help="Target for build",
                        choices=TARGETS.keys(),
                        default='all')
    parser.add_argument('--download',
                        help="Download tools and libs for build environment",
                        action='store_true',
                        default=False)
    parser.add_argument('--check',
                        help="Check for required tools",
                        action='store_true',
                        default=False)
    parser.add_argument('--clean',
                        help="Clean build directory",
                        action='store_true',
                        default=False)
    parser.add_argument('--generate',
                        help="Generate build files using cmake",
                        action='store_true',
                        default=False)
    parser.add_argument('--build',
                        help="Compile firmware",
                        action='store_true',
                        default=False)
    parser.add_argument('--flash',
                        help="Flash connected device",
                        action='store_true',
                        default=False)
    parser.add_argument('--monitor',
                        help="Monitor serial output from hardware",
                        action='store_true',
                        default=False)
    parser.add_argument('--samd21-port',
                        help="COM port for samd21",
                        default=SAMD21.DEFAULT_PORT)
    parser.add_argument('--esp32-port',
                        help="COM port for esp32",
                        default=Esp32.DEFAULT_PORT)
    parser.add_argument('-j',
                        help="Pass -j flag to make",
                        type=int,
                        nargs="?",
                        const=cpu_count(),
                        default=1)
    parser.add_argument("--debug",
                        help="Perform a debug build",
                        action="store_true")
    args = parser.parse_args()

    if args.download:
        with log('download'):
            download_tools(args)
            download_libs(args)

    if args.download:
        with log('check'):
            check_tools(args)

    if args.clean:
        with log('clean'):
            clean(args)

    # If we're attempting a debug build we'll try to generate to ensure that we
    # reconfigure an existing tree if it's not configured appropriately. We
    # won't bother so that re-builds retain the current debug configuration.
    if args.generate or args.debug:
        with log('generate'):
            generate(args)

    if args.build:
        with log('build'):
            build(args)

    if args.flash:
        with log('flash'):
            flash(args)

    if args.monitor:
        with log('monitor'):
            monitor(args)


if __name__ == '__main__':
    main()
