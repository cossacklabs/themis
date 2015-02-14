/*
 * common.h
 *
 *  Created on: Feb 7, 2015
 *      Author: ignat
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_NAME "\0themissessiontestsocket"
#define BUF_SIZE 1024

void* run_server(void *arg);
void* run_client(void *arg);

#endif /* COMMON_H_ */
