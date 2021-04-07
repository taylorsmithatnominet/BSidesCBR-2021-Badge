#!/usr/bin/env python3
import os
import sys
import subprocess

if __name__ == '__main__':
    args = sys.argv[1:]
    args.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), 'scripts', 'build.py'))
    args.insert(0, sys.executable)
    sys.exit(subprocess.call(args, cwd=os.path.dirname(os.path.abspath(__file__))))
