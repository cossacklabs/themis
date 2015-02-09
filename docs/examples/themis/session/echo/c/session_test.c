/*
 ============================================================================
 Name        : session_test.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
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
