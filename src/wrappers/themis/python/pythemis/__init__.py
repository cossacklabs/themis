# coding: utf-8
#
# Copyright (c) 2015 Cossack Labs Limited
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import os
import sys
import warnings
from ctypes import cdll
from ctypes.util import find_library


def _canonical_themis_paths():
    paths = []

    if sys.platform.startswith('linux'):
        # Default library installation path for "make install"
        paths.append('/usr/local/lib/libthemis.so.0')
        # Don't bother figuring out the "right" absolute path, since
        # that depends on the distro and non-distro-provided Pythons
        # don't know about the right paths at all. Simply use soname
        # and let ld figure it out.
        paths.append('libthemis.so.0')

    if sys.platform.startswith('darwin'):
        # Default library installation path for "make install"
        paths.append('/usr/local/lib/libthemis.0.dylib')
        # These are install names of libraries installed via Homebrew
        # Add both M1 and Intel paths so that x86 Ruby works on M1
        paths.append('/opt/homebrew/opt/libthemis/lib/libthemis.0.dylib')
        paths.append('/usr/local/opt/libthemis/lib/libthemis.0.dylib')
        # Last try, look for ABI-qualified name
        paths.append('libthemis.0.dylib')

    return paths


def _load_themis():
    for path in _canonical_themis_paths():
        try:
            return cdll.LoadLibrary(path)
        except OSError:
            continue

    warnings.warn("""failed to load the canonical Themis Core library

Proceeding to find 'themis' library in standard paths.
This might cause ABI mismatch and crash the process.
""",
                  category=RuntimeWarning)

    themis_path = find_library('themis')
    # find_library() returns None on failure and LoadLibrary() would
    # happily 'load' that. We kinda need the library, make it an error.
    if not themis_path:
        raise RuntimeError('failed to locate Themis Core library')
    return cdll.LoadLibrary(themis_path)


__all__ = ["smessage", "scell", "ssession"]

themis = _load_themis()
