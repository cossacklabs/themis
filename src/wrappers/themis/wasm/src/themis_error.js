// Copyright (c) 2019 Cossack Labs Limited
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file
 * Themis error codes and exception types.
 */

/**
 * Themis error code constants.
 *
 * Keep in sync with <themis/themis_error.h>.
 */
const errorCodes = {
    SUCCESS:            0,
    FAIL:               11,
    INVALID_PARAMETER:  12,
    NO_MEMORY:          13,
    BUFFER_TOO_SMALL:   14,
    DATA_CORRUPT:       15,
    INVALID_SIGNATURE:  16,
    NOT_SUPPORTED:      17,

    SSESSION_SEND_OUTPUT_TO_PEER:   1,
    SSESSION_KA_NOT_FINISHED:       19,
    SSESSION_TRANSPORT_ERROR:       20,
    SSESSION_GET_PUB_FOR_ID_CALLBACK_ERROR: 21,

    // Yes, the error codes overlap.
    SCOMPARE_SEND_OUTPUT_TO_PEER:   1,
    SCOMPARE_MATCH:                 21,
    SCOMPARE_NO_MATCH:              22,
    SCOMPARE_NOT_READY:             0,
}

class ThemisError extends Error {
    constructor(subsystem, errorCode) {
        var message = 'Themis: ' + subsystem + ': '
        if (arguments.length > 2) {
            message += arguments[2]
            for (var i = 3; i < arguments.length; i++) {
                message += ', '
                message += arguments[i]
            }
        } else {
            message += ThemisError.description(errorCode, subsystem)
        }
        super(message)
        this.subsystem = subsystem
        this.errorCode = errorCode
    }

    static description(errorCode, subsystem) {
        // Because the error codes overlap, check specific subsystems first,
        // then go to generic error codes.
        if (subsystem === 'SecureComparator') {
            switch (errorCode) {
                case errorCodes.SCOMPARE_SEND_OUTPUT_TO_PEER:
                    return 'send data to peer'
                case errorCodes.SCOMPARE_MATCH:
                    return 'data matches'
                case errorCodes.SCOMPARE_NO_MATCH:
                    return 'data does not match'
                case errorCodes.SCOMPARE_NOT_READY:
                    return 'comparator not ready'
            }
        }
        if (subsystem === 'SecureSession') {
            switch (errorCode) {
                case errorCodes.SSESSION_SEND_OUTPUT_TO_PEER:
                    return 'send data to peer'
                case errorCodes.SSESSION_KA_NOT_FINISHED:
                    return 'key agreement not finished'
                case errorCodes.SSESSION_TRANSPORT_ERROR:
                    return 'transport layer error'
                case errorCodes.SSESSION_GET_PUB_FOR_ID_CALLBACK_ERROR:
                    return 'failed to get public key for ID'
            }
        }
        switch (errorCode) {
            case errorCodes.SUCCESS:
                return 'success'
            case errorCodes.FAIL:
                return 'failure'
            case errorCodes.INVALID_PARAMETER:
                return 'invalid parameter'
            case errorCodes.NO_MEMORY:
                return 'out of memory'
            case errorCodes.BUFFER_TOO_SMALL:
                return 'buffer too small'
            case errorCodes.DATA_CORRUPT:
                return 'corrupted data'
            case errorCodes.INVALID_SIGNATURE:
                return 'invalid signature'
            case errorCodes.NOT_SUPPORTED:
                return 'operation not supported'
        }
        return 'unknown error: ' + error
    }
}

module.exports.ThemisError = ThemisError
module.exports.ThemisErrorCode = errorCodes
