/*
 * Copyright (c) 2016 Cossack Labs Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Package errors describes errors returned by GoThemis functions.
package errors

/*
#include <themis/themis.h>
*/
import "C"

// ThemisErrorCode describes an error reported by Themis Core.
type ThemisErrorCode int

// Error code constants.
//
// These are numeric error codes used by Themis Core (see `<themis/themis_error.h>`).
const (
	Success          ThemisErrorCode = C.THEMIS_SUCCESS
	Fail                             = C.THEMIS_FAIL
	InvalidParameter                 = C.THEMIS_INVALID_PARAMETER
	NoMemory                         = C.THEMIS_NO_MEMORY
	BufferTooSmall                   = C.THEMIS_BUFFER_TOO_SMALL
	DataCorrupt                      = C.THEMIS_DATA_CORRUPT
	InvalidSignature                 = C.THEMIS_INVALID_SIGNATURE
	NotSupported                     = C.THEMIS_NOT_SUPPORTED
)

// ThemisError is a common type of GoThemis errors.
type ThemisError struct {
	description string
	errorCode   ThemisErrorCode
}

// Error returns human-readable textual description of the error.
func (e *ThemisError) Error() string {
	return e.description
}

// Code returns machine-readable numeric error code.
func (e *ThemisError) Code() ThemisErrorCode {
	return e.errorCode
}

// New makes an error with provided description.
func New(description string) *ThemisError {
	// This constructor is used by default in legacy CGo code which does not return
	// accurate Themis Core error codes. Use "Fail" which is likely to be correct
	// and is a good generic placeholder for an error.
	return NewWithCode(Fail, description)
}

// NewWithCode makes an error with provided numeric code and description.
func NewWithCode(code ThemisErrorCode, description string) *ThemisError {
	return &ThemisError{description, code}
}

// ThemisCallbackError is user-generated error returned from Secure Session callback.
type ThemisCallbackError struct {
	msg string
}

// Error returns textual description of the error.
func (e *ThemisCallbackError) Error() string {
	return e.msg
}

// NewCallbackError makes an error with provided description.
func NewCallbackError(msg string) *ThemisCallbackError {
	return &ThemisCallbackError{msg}
}
