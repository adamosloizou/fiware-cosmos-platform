# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
"""Module to obtain console dimensions in a portable way"""

import os
import platform
import shlex
import struct
import subprocess
from sys import stdin, stdout, stderr


STDIN_HANDLE = -10
STDOUT_HANDLE = -11
STDERR_HANDLE = -12


class TerminalError(Exception):
    pass


def get_terminal_size():
    """ Obtain a tuple comprised by the width and height of the terminal.
    Works on Linux, OSX, Windows and Cygwin. Throws a TerminalError if
    fails"""
    current_os = platform.system()
    if current_os == 'Windows':
        return get_terminal_size_windows()
    elif current_os in ['Linux', 'Darwin'] or current_os.startswith('CYGWIN'):
        return get_terminal_size_posix()
    else:
        raise TerminalError(
            "cannot determine your OS to check the terminal size")
    return tuple_xy


def get_terminal_size_windows():
    try:
        from ctypes import windll, create_string_buffer
        h = windll.kernel32.GetStdHandle(STDERR_HANDLE)
        csbi = create_string_buffer(22)
        res = windll.kernel32.GetConsoleScreenBufferInfo(h, csbi)
        if res:
            (_, _, _, _, _, left, top, right, bottom, _, _) = struct.unpack(
                "hhhhHhhhhhh", csbi.raw)
            sizex = right - left + 1
            sizey = bottom - top + 1
            return sizex, sizey
    except:
        pass
    raise TerminalError('cannot determine terminal size from Windows')


def get_terminal_size_posix():
    def ioctl_GWINSZ(fd):
        try:
            import fcntl
            import termios
            cr = struct.unpack('hh',
                               fcntl.ioctl(fd, termios.TIOCGWINSZ, '1234'))
            return cr
        except:
            pass
    cr = ioctl_GWINSZ(stdin.fileno()) or \
         ioctl_GWINSZ(stdout.fileno()) or \
         ioctl_GWINSZ(stderr.fileno())
    if not cr:
        try:
            with os.open(os.ctermid(), os.O_RDONLY) as fd:
            	cr = ioctl_GWINSZ(fd)
        except:
            pass
    if not cr:
        try:
            cr = (os.environ['LINES'], os.environ['COLUMNS'])
        except:
            pass
    if not cr:
        raise TerminalError('cannot determine terminal size from POSIX')
    return int(cr[1]), int(cr[0])
