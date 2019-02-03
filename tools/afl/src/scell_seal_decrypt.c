/*
 * Copyright (c) 2019 Cossack Labs Limited
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <themis/themis.h>

#include "readline.h"

int main(void)
{
	themis_status_t err = THEMIS_SUCCESS;

	/*
	 * Read test data.
	 */

	uint8_t *master_key_bytes = NULL;
	size_t master_key_size = 0;

	if (read_line_binary(stdin, &master_key_bytes, &master_key_size))
	{
		return 1;
	}

	uint8_t *user_context_bytes = NULL;
	size_t user_context_size = 0;

	if (read_line_binary(stdin, &user_context_bytes, &user_context_size))
	{
		return 1;
	}

	uint8_t *message_bytes = NULL;
	size_t message_size = 0;

	if (read_line_binary(stdin, &message_bytes, &message_size))
	{
		return 1;
	}

	/*
	 * Try decrypting it.
	 */

	uint8_t *decrypted_bytes = NULL;
	size_t decrypted_size = 0;

	err = themis_secure_cell_decrypt_seal(
		master_key_bytes, master_key_size,
		user_context_bytes, user_context_size,
		message_bytes, message_size,
		NULL, &decrypted_size);

	if (err != THEMIS_BUFFER_TOO_SMALL)
	{
		return 2;
	}

	decrypted_bytes = malloc(decrypted_size);
	if (!decrypted_bytes)
	{
		return 2;
	}

	err = themis_secure_cell_decrypt_seal(
		master_key_bytes, master_key_size,
		user_context_bytes, user_context_size,
		message_bytes, message_size,
		decrypted_bytes, &decrypted_size);

	if (err != THEMIS_SUCCESS)
	{
		return 2;
	}

	return 0;
}
