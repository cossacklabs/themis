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
import warnings
from enum import IntEnum


class THEMIS_CODES(IntEnum):
    NETWORK_ERROR = 2222
    BUFFER_TOO_SMALL = 14
    INVALID_PARAMETER = 12
    FAIL = 11
    SUCCESS = 0
    SEND_AS_IS = 1


class ThemisError(Exception):
    def __init__(self, error_code, message):
        self.error_code = error_code
        self.message = message

    def __str__(self):
        return repr("Themis error: {} -- {}".format(str(self.error_code),
                                                    self.message))


class themis_exception(ThemisError):
    def __init__(self, *args, **kwargs):
        warnings.warn("themis_exception is deprecated in favor of ThemisError.")
        super(themis_exception, self).__init__(*args, **kwargs)
