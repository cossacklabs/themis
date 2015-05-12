/*
* Copyright (c) 2015 Cossack Labs Limited
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

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include <pthread.h>

int main(void)
{
	pthread_t server_thread;
	pthread_t client_thread;

	pthread_create(&server_thread, NULL, run_server, NULL);
	sleep(1); /* for server to become ready */
	pthread_create(&client_thread, NULL, run_client, NULL);

	pthread_join(server_thread, NULL);
	pthread_join(client_thread, NULL);

	return EXIT_SUCCESS;
}
