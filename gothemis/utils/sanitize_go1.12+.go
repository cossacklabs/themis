// +build go1.12

/*
 * Copyright (c) 2020 Cossack Labs Limited
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

package utils

// SanitizeBuffer applies a workaround for CGo 1.11 and earlier.
//
// This function is an internal implementation detail and may be removed without prior notice.
func SanitizeBuffer(data []byte) []byte {
	// Since Go 1.12 we can just return the data.
	return data
}
