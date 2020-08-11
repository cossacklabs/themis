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

#include "themis/themis_test.h"

#include <assert.h>
#include <string.h>

/* Fuzz parameters */
#define MAX_MESSAGE_SIZE 2048
#define MESSAGES_TO_SEND 3

/* return codes for scheduler */
#define TEST_CONTINUE 1
#define TEST_STOP_SUCCESS 0
#define TEST_STOP_ERROR (-1)

static uint8_t client_priv[] = {
    0x52, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0x51, 0xf4, 0xaa, 0x72, 0x00, 0x9f, 0x0f,
    0x09, 0xce, 0xbe, 0x09, 0x33, 0xc2, 0x5e, 0x9a, 0x05, 0x99, 0x53, 0x9d, 0xb2, 0x32, 0xa2,
    0x34, 0x64, 0x7a, 0xde, 0xde, 0x83, 0x8f, 0x65, 0xa9, 0x2a, 0x14, 0x6d, 0xaa, 0x90, 0x01,
};
static uint8_t client_pub[] = {
    0x55, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0x13, 0x8b, 0xdf, 0x0c, 0x02, 0x1f, 0x09,
    0x88, 0x39, 0xd9, 0x73, 0x3a, 0x84, 0x8f, 0xa8, 0x50, 0xd9, 0x2b, 0xed, 0x3d, 0x38, 0xcf,
    0x1d, 0xd0, 0xce, 0xf4, 0xae, 0xdb, 0xcf, 0xaf, 0xcb, 0x6b, 0xa5, 0x4a, 0x08, 0x11, 0x21,
};

static uint8_t server_priv[] = {
    0x52, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0x49, 0x87, 0x04, 0x6b, 0x00, 0xf2, 0x06,
    0x07, 0x7d, 0xc7, 0x1c, 0x59, 0xa1, 0x8f, 0x39, 0xfc, 0x94, 0x81, 0x3f, 0x9e, 0xc5, 0xba,
    0x70, 0x6f, 0x93, 0x08, 0x8d, 0xe3, 0x85, 0x82, 0x5b, 0xf8, 0x3f, 0xc6, 0x9f, 0x0b, 0xdf,
};
static uint8_t server_pub[] = {
    0x55, 0x45, 0x43, 0x32, 0x00, 0x00, 0x00, 0x2d, 0x75, 0x58, 0x33, 0xd4, 0x02, 0x12, 0xdf,
    0x1f, 0xe9, 0xea, 0x48, 0x11, 0xe1, 0xf9, 0x71, 0x8e, 0x24, 0x11, 0xcb, 0xfd, 0xc0, 0xa3,
    0x6e, 0xd6, 0xac, 0x88, 0xb6, 0x44, 0xc2, 0x9a, 0x24, 0x84, 0xee, 0x50, 0x4c, 0x3e, 0xa0,
};

static uint8_t client_priv_rsa[] = {
    0x52, 0x52, 0x41, 0x32, 0x00, 0x00, 0x04, 0x90, 0x94, 0x65, 0xbe, 0x5c, 0x75, 0x5e, 0x6a,
    0xea, 0xeb, 0x7d, 0x0d, 0x59, 0xa4, 0xe3, 0xc4, 0xb0, 0x2c, 0x11, 0x7b, 0xc6, 0xb0, 0x0e,
    0xb0, 0x97, 0x65, 0x25, 0x0a, 0xb2, 0xb1, 0xa0, 0xdd, 0x93, 0x59, 0x66, 0x6c, 0x7f, 0xaa,
    0xfc, 0x00, 0x29, 0x0e, 0xcc, 0x45, 0xd2, 0xeb, 0x1d, 0x4d, 0x88, 0x57, 0xff, 0x6d, 0x3a,
    0x85, 0x4e, 0x51, 0x21, 0x49, 0xa2, 0xfb, 0x73, 0xc8, 0xb3, 0xfe, 0x66, 0xe3, 0xa5, 0x84,
    0x2b, 0x23, 0xa7, 0xbd, 0x62, 0x19, 0x51, 0xc8, 0x1d, 0x1e, 0x94, 0x50, 0x8c, 0x0d, 0x06,
    0x8f, 0x0f, 0x7e, 0xb1, 0x96, 0xeb, 0xea, 0xfe, 0xa1, 0x67, 0x32, 0x23, 0xb7, 0xe2, 0x14,
    0xf7, 0x65, 0xc3, 0xc1, 0x2b, 0x7c, 0xcc, 0x78, 0x50, 0x2c, 0x61, 0x6b, 0x59, 0xe8, 0x36,
    0xe8, 0x22, 0x5b, 0x48, 0x7b, 0x40, 0x0e, 0xfc, 0x40, 0x03, 0x2c, 0x14, 0x6e, 0xf1, 0x09,
    0xf2, 0x83, 0x40, 0xcf, 0x8f, 0xfc, 0x1b, 0x19, 0x35, 0x3a, 0x51, 0xca, 0x3e, 0x18, 0xec,
    0xb1, 0xef, 0x83, 0x5d, 0x26, 0x1b, 0x19, 0x7a, 0x8e, 0x9a, 0x28, 0xef, 0x43, 0xe7, 0xa1,
    0xc5, 0x24, 0x3d, 0x01, 0xf5, 0x25, 0xe1, 0x6c, 0x28, 0x68, 0xdd, 0x4e, 0x8b, 0x42, 0x9f,
    0x09, 0x39, 0xf2, 0x59, 0x0e, 0xe5, 0x86, 0x2f, 0xe6, 0xe4, 0xda, 0x5f, 0xff, 0x24, 0x8f,
    0x01, 0x08, 0xbd, 0x91, 0x5d, 0xb1, 0x19, 0xe2, 0xcc, 0xae, 0xcd, 0xf2, 0xbb, 0x0e, 0xc1,
    0xd3, 0xfd, 0x3b, 0x09, 0x65, 0xe5, 0xc0, 0x65, 0x0d, 0xe6, 0xd7, 0x92, 0xda, 0xce, 0x36,
    0xa4, 0x43, 0xa1, 0xfb, 0x0d, 0x18, 0x2e, 0x6a, 0x19, 0x06, 0x2a, 0x86, 0xb0, 0xd4, 0x79,
    0x9a, 0xf4, 0x65, 0xd7, 0x00, 0xd8, 0x36, 0x52, 0xd3, 0x04, 0x78, 0xfb, 0xa1, 0x5f, 0xaa,
    0x5b, 0xac, 0x07, 0xed, 0x60, 0xee, 0x51, 0xc5, 0x9a, 0xee, 0x33, 0xd1, 0xf5, 0xec, 0x2b,
    0xfa, 0x7a, 0xb5, 0x08, 0xd2, 0x84, 0x88, 0xb3, 0x7c, 0x5b, 0xd1, 0x93, 0x49, 0xb8, 0xe2,
    0xc4, 0xb1, 0x92, 0x03, 0x91, 0x7e, 0x46, 0x9f, 0x77, 0x98, 0xd5, 0x27, 0x4e, 0x83, 0x9b,
    0x63, 0xca, 0x14, 0xe2, 0x52, 0x1d, 0x80, 0xa4, 0x46, 0x9e, 0x30, 0x6e, 0x88, 0xe0, 0xa2,
    0xcb, 0x6b, 0x23, 0xdc, 0x9f, 0x63, 0x50, 0x69, 0xf0, 0x16, 0x9a, 0x7d, 0xb3, 0xd1, 0x6b,
    0x4c, 0x54, 0x2b, 0x47, 0x02, 0x82, 0x77, 0xd1, 0x6c, 0x14, 0x96, 0x4d, 0xb2, 0x10, 0xd9,
    0x49, 0x67, 0x6b, 0x45, 0x7f, 0x54, 0xec, 0xe9, 0x71, 0x8e, 0x71, 0xbf, 0xec, 0xfe, 0x1f,
    0xb0, 0xaf, 0xce, 0x47, 0x2c, 0x18, 0xfa, 0xdc, 0x43, 0x7b, 0x8c, 0x40, 0x5e, 0x2a, 0xe4,
    0x87, 0x26, 0x65, 0xa3, 0x8a, 0x67, 0x45, 0x67, 0x7f, 0xf3, 0x8c, 0x3c, 0x55, 0xb3, 0xc4,
    0x29, 0x6f, 0xaf, 0x83, 0xde, 0x77, 0xc5, 0x78, 0x70, 0x58, 0x23, 0x61, 0x83, 0xf7, 0xbc,
    0xd8, 0x73, 0x9e, 0x58, 0x9e, 0x8c, 0xae, 0x9c, 0xce, 0xd8, 0x25, 0x10, 0x5b, 0xb7, 0x9a,
    0x7d, 0x3a, 0x3c, 0x2b, 0xe8, 0x02, 0x21, 0xd5, 0x34, 0x8b, 0xfd, 0x71, 0x2d, 0xc1, 0x7d,
    0x71, 0xf8, 0xe7, 0xbf, 0xc0, 0x92, 0x3e, 0xb5, 0x82, 0xf7, 0xb9, 0x84, 0x09, 0xea, 0x1f,
    0xcd, 0x15, 0x2d, 0xfc, 0x66, 0x4c, 0x6b, 0xcc, 0x16, 0x47, 0x0c, 0x7e, 0xee, 0x40, 0xce,
    0x0d, 0xd2, 0x5f, 0x39, 0x8b, 0x13, 0x0c, 0x67, 0x8f, 0x19, 0x37, 0x71, 0x3d, 0x2c, 0xe8,
    0x44, 0x8a, 0x58, 0xd1, 0xe4, 0xa4, 0x27, 0x7f, 0x63, 0x68, 0x6e, 0x3c, 0x46, 0x0d, 0x3a,
    0xc6, 0x6c, 0x33, 0x86, 0x0b, 0xef, 0x16, 0xb2, 0x5b, 0x56, 0xfc, 0x6a, 0xca, 0x80, 0xf5,
    0xb6, 0xf4, 0x50, 0x81, 0xde, 0x35, 0x91, 0x7d, 0x1a, 0x94, 0x6b, 0x7c, 0xc1, 0x93, 0x35,
    0x2c, 0xe1, 0xc4, 0xe7, 0x26, 0x09, 0x60, 0x55, 0xf7, 0x33, 0xb4, 0xe8, 0xc7, 0x24, 0xfe,
    0x3f, 0xf0, 0xd1, 0x9a, 0x72, 0xbb, 0x3b, 0x97, 0x63, 0x28, 0x66, 0x7c, 0xf7, 0xd0, 0xb1,
    0xc9, 0x36, 0xfa, 0xb5, 0x21, 0x10, 0x24, 0x1c, 0x66, 0xe0, 0x8f, 0x95, 0x90, 0x0d, 0xb9,
    0xa6, 0x6d, 0xee, 0xbd, 0xb0, 0x5b, 0x22, 0xdd, 0x64, 0x3c, 0xee, 0x99, 0xf2, 0x5b, 0x9e,
    0x77, 0x6e, 0x44, 0x35, 0x4a, 0x53, 0xc2, 0x54, 0x80, 0x0e, 0xb9, 0xc3, 0xed, 0xd2, 0x7e,
    0x00, 0xae, 0x8a, 0x5e, 0x98, 0xd3, 0x46, 0xa7, 0xe1, 0x17, 0xf4, 0x5e, 0xa0, 0x3c, 0x18,
    0x2b, 0xc2, 0x78, 0x93, 0x15, 0x6b, 0x54, 0x64, 0x87, 0x5b, 0x9a, 0x61, 0xc5, 0xcc, 0x6e,
    0x13, 0xee, 0x6a, 0x32, 0xd0, 0x53, 0xc5, 0x54, 0xd7, 0x2c, 0xf4, 0x05, 0x4b, 0x40, 0x70,
    0xad, 0x66, 0x37, 0x03, 0x69, 0xc9, 0x99, 0xb2, 0xa8, 0xdb, 0x0c, 0x03, 0x46, 0xd0, 0xfd,
    0x2d, 0x50, 0xb0, 0x9a, 0x8d, 0x17, 0x35, 0xb5, 0x2d, 0x8e, 0x3f, 0x24, 0x99, 0x7e, 0x41,
    0xc9, 0x76, 0x8f, 0x5f, 0xad, 0x61, 0xc5, 0xbc, 0x12, 0x87, 0xee, 0x38, 0x29, 0x53, 0xfa,
    0x81, 0xa2, 0xc2, 0x4d, 0xfa, 0xc1, 0x22, 0x0f, 0xb6, 0x6d, 0x7d, 0xef, 0xcc, 0xb4, 0xa3,
    0xac, 0x8e, 0x02, 0x21, 0x8f, 0x00, 0x87, 0x17, 0xe8, 0x98, 0x27, 0x43, 0xf8, 0xfb, 0x5a,
    0x8a, 0x93, 0x1a, 0xc8, 0xa8, 0xe9, 0x59, 0x1c, 0x99, 0xfd, 0x70, 0x15, 0x27, 0x50, 0xef,
    0xf6, 0x5d, 0x3c, 0xc6, 0x4b, 0x70, 0x8f, 0xcc, 0x64, 0x86, 0xee, 0x0e, 0xfe, 0x5d, 0x6a,
    0xce, 0xc8, 0x83, 0x50, 0xc2, 0x70, 0xbd, 0xaa, 0x4d, 0x48, 0x32, 0x95, 0x41, 0x91, 0x61,
    0xc7, 0xed, 0xf5, 0x71, 0x79, 0x6e, 0x30, 0x0d, 0x27, 0xa1, 0x0a, 0xb3, 0x8d, 0x36, 0x1d,
    0x2f, 0xf2, 0xec, 0xff, 0x45, 0x4d, 0x06, 0x6f, 0x52, 0x20, 0x9c, 0xdb, 0x31, 0x09, 0xe1,
    0x48, 0x5b, 0x7d, 0x91, 0x6b, 0x5a, 0x24, 0x10, 0x27, 0x69, 0x18, 0x41, 0x57, 0x98, 0x91,
    0x1a, 0x92, 0x9f, 0xd7, 0x9c, 0xc2, 0x58, 0x7d, 0xc7, 0x35, 0x2b, 0xca, 0xed, 0x60, 0xf7,
    0xc1, 0x3d, 0x02, 0x47, 0x58, 0x0c, 0x63, 0xe1, 0xa8, 0x91, 0xd2, 0xf1, 0x85, 0xd4, 0xdc,
    0xcf, 0x44, 0x91, 0x32, 0xde, 0xd0, 0xd7, 0x87, 0x0d, 0x41, 0x77, 0x54, 0xc5, 0x00, 0xab,
    0x02, 0x1e, 0x11, 0xf7, 0xf7, 0xe9, 0x8f, 0x5e, 0x6b, 0x8a, 0xc5, 0xc2, 0x9d, 0xcb, 0x07,
    0xfe, 0xc7, 0x09, 0x93, 0x48, 0xb0, 0xcd, 0x6c, 0x7d, 0xf1, 0x14, 0x13, 0xa9, 0x99, 0xdc,
    0x9a, 0x9f, 0xc2, 0x72, 0x99, 0xde, 0xce, 0xb4, 0x42, 0x79, 0x0a, 0x33, 0xb0, 0x9e, 0x2f,
    0xb6, 0xf0, 0x9a, 0x50, 0x74, 0xb1, 0x0f, 0x5b, 0xb6, 0x2c, 0xf4, 0x02, 0x64, 0x2c, 0x69,
    0x2c, 0x7f, 0x86, 0xcc, 0x11, 0xd4, 0x77, 0x19, 0xfe, 0x4a, 0x09, 0xc3, 0xf3, 0xbd, 0x29,
    0xe3, 0x7d, 0x6d, 0x9f, 0x8e, 0x6d, 0xa6, 0x69, 0x3a, 0xc8, 0x23, 0x55, 0x5b, 0x3f, 0xcb,
    0x8c, 0xf3, 0x8f, 0x4e, 0x48, 0x05, 0x52, 0x3a, 0xf1, 0x8c, 0x0c, 0xcb, 0xd8, 0x3d, 0xbe,
    0x60, 0xd4, 0x17, 0x8a, 0x98, 0x9b, 0x03, 0xb8, 0xd5, 0x0e, 0xd2, 0xb1, 0xe0, 0x67, 0x9c,
    0x93, 0xc2, 0xbc, 0xfe, 0x6c, 0x4a, 0x4b, 0x0b, 0x07, 0xe8, 0xe1, 0x15, 0xc5, 0xd1, 0x48,
    0xe4, 0xc7, 0xae, 0x0b, 0x06, 0x68, 0x57, 0x9d, 0xd6, 0xe4, 0x2e, 0x8c, 0x85, 0x87, 0x02,
    0xf2, 0xb7, 0x36, 0x7e, 0xce, 0x73, 0x70, 0x1c, 0x33, 0x82, 0x4e, 0x4c, 0xdf, 0xfd, 0x59,
    0x76, 0xa6, 0xa9, 0x95, 0xd4, 0x79, 0xd7, 0xf1, 0xf3, 0xb9, 0xa6, 0x59, 0x5d, 0x4c, 0xd4,
    0x6f, 0xb3, 0x27, 0x07, 0x81, 0xbf, 0xcf, 0xc8, 0x35, 0x95, 0x68, 0x66, 0xab, 0x2d, 0x61,
    0xbd, 0x09, 0xbb, 0x16, 0xcf, 0x38, 0x5e, 0x96, 0xeb, 0x93, 0x7a, 0x97, 0x78, 0x50, 0x2d,
    0x7d, 0xe3, 0x41, 0x8b, 0xca, 0x6f, 0x07, 0x47, 0x00, 0xcd, 0x13, 0x35, 0xaa, 0x21, 0xcc,
    0x46, 0x12, 0x0e, 0x25, 0x45, 0xee, 0x2f, 0x9c, 0x42, 0x09, 0x80, 0x72, 0x99, 0xee, 0x31,
    0x6d, 0x77, 0xbd, 0x89, 0x19, 0x15, 0x94, 0xd9, 0xea, 0xd1, 0xaf, 0x69, 0x3a, 0x3f, 0xdf,
    0x14, 0xca, 0x27, 0xab, 0xb7, 0x70, 0x58, 0x4b, 0xe8, 0x14, 0x92, 0x4c, 0xcc, 0x56, 0xf8,
    0x4c, 0x50, 0x89, 0xea, 0x01, 0xf6, 0x5c, 0x7d, 0x6d, 0xbe, 0x09, 0xda, 0x02, 0x90, 0x7f,
    0x29, 0xaa, 0xdc, 0xf3, 0x13, 0x03, 0xc6, 0x8e, 0xd5, 0xed, 0xab, 0x00, 0xf1, 0x68, 0x43,
    0x7f, 0xc1, 0x45, 0xd3, 0x1b, 0xb2, 0x14, 0x75, 0x55, 0x00, 0x01, 0x00, 0x01,
};
static uint8_t client_pub_rsa[] = {
    0x55, 0x52, 0x41, 0x32, 0x00, 0x00, 0x01, 0x10, 0xe9, 0x99, 0x84, 0x59, 0xb6, 0x2c, 0xf4,
    0x02, 0x64, 0x2c, 0x69, 0x2c, 0x7f, 0x86, 0xcc, 0x11, 0xd4, 0x77, 0x19, 0xfe, 0x4a, 0x09,
    0xc3, 0xf3, 0xbd, 0x29, 0xe3, 0x7d, 0x6d, 0x9f, 0x8e, 0x6d, 0xa6, 0x69, 0x3a, 0xc8, 0x23,
    0x55, 0x5b, 0x3f, 0xcb, 0x8c, 0xf3, 0x8f, 0x4e, 0x48, 0x05, 0x52, 0x3a, 0xf1, 0x8c, 0x0c,
    0xcb, 0xd8, 0x3d, 0xbe, 0x60, 0xd4, 0x17, 0x8a, 0x98, 0x9b, 0x03, 0xb8, 0xd5, 0x0e, 0xd2,
    0xb1, 0xe0, 0x67, 0x9c, 0x93, 0xc2, 0xbc, 0xfe, 0x6c, 0x4a, 0x4b, 0x0b, 0x07, 0xe8, 0xe1,
    0x15, 0xc5, 0xd1, 0x48, 0xe4, 0xc7, 0xae, 0x0b, 0x06, 0x68, 0x57, 0x9d, 0xd6, 0xe4, 0x2e,
    0x8c, 0x85, 0x87, 0x02, 0xf2, 0xb7, 0x36, 0x7e, 0xce, 0x73, 0x70, 0x1c, 0x33, 0x82, 0x4e,
    0x4c, 0xdf, 0xfd, 0x59, 0x76, 0xa6, 0xa9, 0x95, 0xd4, 0x79, 0xd7, 0xf1, 0xf3, 0xb9, 0xa6,
    0x59, 0x5d, 0x4c, 0xd4, 0x6f, 0xb3, 0x27, 0x07, 0x81, 0xbf, 0xcf, 0xc8, 0x35, 0x95, 0x68,
    0x66, 0xab, 0x2d, 0x61, 0xbd, 0x09, 0xbb, 0x16, 0xcf, 0x38, 0x5e, 0x96, 0xeb, 0x93, 0x7a,
    0x97, 0x78, 0x50, 0x2d, 0x7d, 0xe3, 0x41, 0x8b, 0xca, 0x6f, 0x07, 0x47, 0x00, 0xcd, 0x13,
    0x35, 0xaa, 0x21, 0xcc, 0x46, 0x12, 0x0e, 0x25, 0x45, 0xee, 0x2f, 0x9c, 0x42, 0x09, 0x80,
    0x72, 0x99, 0xee, 0x31, 0x6d, 0x77, 0xbd, 0x89, 0x19, 0x15, 0x94, 0xd9, 0xea, 0xd1, 0xaf,
    0x69, 0x3a, 0x3f, 0xdf, 0x14, 0xca, 0x27, 0xab, 0xb7, 0x70, 0x58, 0x4b, 0xe8, 0x14, 0x92,
    0x4c, 0xcc, 0x56, 0xf8, 0x4c, 0x50, 0x89, 0xea, 0x01, 0xf6, 0x5c, 0x7d, 0x6d, 0xbe, 0x09,
    0xda, 0x02, 0x90, 0x7f, 0x29, 0xaa, 0xdc, 0xf3, 0x13, 0x03, 0xc6, 0x8e, 0xd5, 0xed, 0xab,
    0x00, 0xf1, 0x68, 0x43, 0x7f, 0xc1, 0x45, 0xd3, 0x1b, 0xb2, 0x14, 0x75, 0x55, 0x00, 0x01,
    0x00, 0x01,
};

typedef struct client_info_type client_info_t;

struct client_info_type {
    const char* id;

    const uint8_t* priv;
    size_t priv_length;

    const uint8_t* pub;
    size_t pub_length;

    secure_session_t* session;
    secure_session_user_callbacks_t transport;
};

static client_info_t client = {
    "client",
    client_priv,
    sizeof(client_priv),
    client_pub,
    sizeof(client_pub),
    NULL,
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    },
};
static client_info_t server = {
    "server",
    server_priv,
    sizeof(server_priv),
    server_pub,
    sizeof(server_pub),
    NULL,
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    },
};

static client_info_t client_rsa = {
    "client",
    client_priv_rsa,
    sizeof(client_priv_rsa),
    client_pub_rsa,
    sizeof(client_pub_rsa),
    NULL,
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    },
};

/* Peers will communicate using shared memory */
static uint8_t shared_mem[4096];
static size_t current_length = 0;

static int on_get_public_key(
    const void* id, size_t id_length, void* key_buffer, size_t key_buffer_length, void* user_data)
{
    client_info_t* info = user_data;
    client_info_t* peer;

    if (info == &client) {
        /* The client should request server's public key */
        peer = &server;
    } else if (info == &server) {
        /* The server should request client's public key */
        peer = &client;
    } else {
        return -1;
    }

    if (memcmp(peer->id, id, id_length) != 0) {
        return -1;
    }

    if (peer->pub_length > key_buffer_length) {
        return -1;
    }

    memcpy(key_buffer, peer->pub, peer->pub_length);
    return 0;
}

static ssize_t on_send_data(const uint8_t* data, size_t data_length, void* user_data)
{
    UNUSED(user_data);
    memcpy(shared_mem, data, data_length);
    current_length = data_length;
    return (ssize_t)data_length;
}

static ssize_t on_receive_data(uint8_t* data, size_t data_length, void* user_data)
{
    UNUSED(user_data);

    if (data_length < current_length) {
        return -1;
    }

    memcpy(data, shared_mem, current_length);

    return current_length;
}

static void on_state_changed(int event, void* user_data)
{
    UNUSED(event);
    UNUSED(user_data);
    /* TODO: implement */
}

static secure_session_user_callbacks_t transport = {on_send_data,
                                                    on_receive_data,
                                                    on_state_changed,
                                                    on_get_public_key,
                                                    NULL};

static int client_function(void)
{
    static bool connected = false;
    themis_status_t res;
    uint8_t recv_buf[2048];
    ssize_t bytes_received;
    ssize_t bytes_sent;

    /* Client is not connected yet. Initiate key agreement */
    if (!connected) {
        res = secure_session_connect((client.session));
        if (THEMIS_SUCCESS == res) {
            connected = true;
            return TEST_CONTINUE;
        }

        testsuite_fail_if(res, "secure_session_connect failed");
        return TEST_STOP_ERROR;
    }
    if (secure_session_is_established(client.session)) {
        size_t remote_id_length = 0;
        if (THEMIS_BUFFER_TOO_SMALL
            != secure_session_get_remote_id(client.session, NULL, &remote_id_length)) {
            testsuite_fail_if(true, "remote id getting failed (length_determination)");
            return TEST_STOP_ERROR;
        }
        uint8_t* remote_id = malloc(remote_id_length);
        assert(remote_id);
        if (THEMIS_SUCCESS
            != secure_session_get_remote_id(client.session, remote_id, &remote_id_length)) {
            testsuite_fail_if(true, "remote id getting failed");
            free(remote_id);
            return TEST_STOP_ERROR;
        }
        testsuite_fail_unless(remote_id_length == strlen(server.id)
                                  && 0 == memcmp(remote_id, server.id, strlen(server.id)),
                              "secure_session remote id getting");
        free(remote_id);

        static int messages_to_send = MESSAGES_TO_SEND;

        /* Connection is already established. */
        static uint8_t data_to_send[MAX_MESSAGE_SIZE];
        static size_t length_to_send;

        /* If there is anything to receive, receive it */
        if (current_length) {
            bytes_received = secure_session_receive(client.session, recv_buf, sizeof(recv_buf));
            if (bytes_received > 0) {
                /* The server should echo our previously sent data */
                testsuite_fail_unless((length_to_send == (size_t)bytes_received)
                                          && (!memcmp(recv_buf, data_to_send, bytes_received)),
                                      "secure_session message send/receive");
                messages_to_send--;

                if (!messages_to_send) {
                    return TEST_STOP_SUCCESS;
                }
            } else {
                /* We shoud receive something. */
                testsuite_fail_if(bytes_received, "secure_session_receive failed");
                return TEST_STOP_ERROR;
            }
        }

        length_to_send = rand_int(MAX_MESSAGE_SIZE);

        if (THEMIS_SUCCESS != soter_rand(data_to_send, length_to_send)) {
            testsuite_fail_if(true, "soter_rand failed");
            return TEST_STOP_ERROR;
        }

        bytes_sent = secure_session_send((client.session), data_to_send, length_to_send);
        if (bytes_sent > 0) {
            /* Check whether data was indeed encrypted (it should not be the same as in
             * data_to_send) */
            testsuite_fail_if((length_to_send == current_length)
                                  || (!memcmp(data_to_send, shared_mem, length_to_send)),
                              "secure_session client message wrap");
            return TEST_CONTINUE;
        }

        testsuite_fail_if(true, "secure_session_send failed");
        return TEST_STOP_ERROR;
    }
    /* Connection is not established. We should receive some key agreement data. */

    bytes_received = secure_session_receive((client.session), recv_buf, sizeof(recv_buf));
    /* When key agreement data is received and processed client gets 0 in return value (no data
     * for client is received) */
    if (bytes_received) {
        testsuite_fail_if(bytes_received, "secure_session_receive failed");
        return TEST_STOP_ERROR;
    }

    if (secure_session_is_established((client.session))) {
        /* Negotiation completed. Clear the shared memory. */
        current_length = 0;
    }

    return TEST_CONTINUE;
}

static int client_function_no_transport(void)
{
    static bool connected = false;
    themis_status_t res;
    uint8_t recv_buf[2048];
    ssize_t bytes_received = 0;
    uint8_t processing_buf[2048];
    size_t processing_buf_size = 0;

    /* Client is not connected yet. Initiate key agreement */
    if (!connected) {
        res = secure_session_generate_connect_request((client.session),
                                                      processing_buf,
                                                      (size_t*)(&processing_buf_size));
        if (THEMIS_BUFFER_TOO_SMALL != res) {
            testsuite_fail_if(res, "secure_session_generate_connect_request failed");
            return TEST_STOP_ERROR;
        }

        res = secure_session_generate_connect_request((client.session),
                                                      processing_buf,
                                                      (size_t*)(&processing_buf_size));
        if (THEMIS_SUCCESS == res) {
            /* This test-send function never fails, so we do not check for error here */
            on_send_data(processing_buf, processing_buf_size, NULL);
            connected = true;
            return TEST_CONTINUE;
        }

        testsuite_fail_if(res, "secure_session_generate_connect_request failed");
        return TEST_STOP_ERROR;
    }

    if (secure_session_is_established((client.session))) {
        size_t remote_id_length = 0;
        if (THEMIS_BUFFER_TOO_SMALL
            != secure_session_get_remote_id(client.session, NULL, &remote_id_length)) {
            testsuite_fail_if(true, "remote id getting failed (length_determination)");
            return TEST_STOP_ERROR;
        }
        uint8_t* remote_id = malloc(remote_id_length);
        assert(remote_id);
        if (THEMIS_SUCCESS
            != secure_session_get_remote_id(client.session, remote_id, &remote_id_length)) {
            testsuite_fail_if(true, "remote id getting failed");
            free(remote_id);
            return TEST_STOP_ERROR;
        }
        testsuite_fail_unless(remote_id_length == strlen(server.id)
                                  && 0 == memcmp(remote_id, server.id, strlen(server.id)),
                              "secure_session remote id getting");
        free(remote_id);

        static int messages_to_send = MESSAGES_TO_SEND;

        /* Connection is already established. */
        static uint8_t data_to_send[MAX_MESSAGE_SIZE];
        static size_t length_to_send;
        processing_buf_size = sizeof(processing_buf);

        /* If there is anything to receive, receive it */
        if (current_length) {
            bytes_received = on_receive_data(recv_buf, sizeof(recv_buf), NULL);
            if (bytes_received > 0) {
                res = secure_session_unwrap((client.session),
                                            recv_buf,
                                            (size_t)bytes_received,
                                            processing_buf,
                                            (size_t*)(&processing_buf_size));
                if (THEMIS_SUCCESS != res) {
                    testsuite_fail_if(res, "secure_session_unwrap failed");
                    return TEST_STOP_ERROR;
                }

                /* The server should echo our previously sent data */
                testsuite_fail_if(current_length == (size_t)processing_buf_size,
                                  "secure_session message send/receive");
                messages_to_send--;

                if (!messages_to_send) {
                    return TEST_STOP_SUCCESS;
                }
            } else {
                /* We shoud receive something. */
                testsuite_fail_if(bytes_received, "secure_session_receive failed");
                return TEST_STOP_ERROR;
            }
        }

        length_to_send = rand_int(MAX_MESSAGE_SIZE - 64);

        if (THEMIS_SUCCESS != soter_rand(data_to_send, length_to_send)) {
            testsuite_fail_if(true, "soter_rand failed");
            return TEST_STOP_ERROR;
        }

        processing_buf_size = sizeof(processing_buf);
        res = secure_session_wrap((client.session),
                                  data_to_send,
                                  length_to_send,
                                  processing_buf,
                                  (size_t*)(&processing_buf_size));
        if (THEMIS_SUCCESS != res) {
            testsuite_fail_if(res, "secure_session_wrap failed");
            return TEST_STOP_ERROR;
        }

        /* This test-send function never fails, so we do not check for error here */
        on_send_data(processing_buf, processing_buf_size, NULL);
        testsuite_fail_if(length_to_send == current_length, "secure_session client message wrap");
        return TEST_CONTINUE;
    }

    /* Connection is not established. We should receive some key agreement data. */

    bytes_received = on_receive_data(recv_buf, sizeof(recv_buf), NULL);
    if (bytes_received <= 0) {
        testsuite_fail_if(bytes_received, "secure_session_receive failed");
        return TEST_STOP_ERROR;
    }

    res = secure_session_unwrap((client.session),
                                recv_buf,
                                bytes_received,
                                processing_buf,
                                (size_t*)(&processing_buf_size));
    if (THEMIS_SUCCESS == res) {
        if (secure_session_is_established((client.session))) {
            /* Negotiation completed. Clear the shared memory. */
            current_length = 0;
            return TEST_CONTINUE;
        }

        testsuite_fail_if(true, "secure_session_unwrap failed");
        return TEST_STOP_ERROR;
    }
    if (THEMIS_BUFFER_TOO_SMALL != res) {
        testsuite_fail_if(true, "secure_session_unwrap failed");
        return TEST_STOP_ERROR;
    }

    res = secure_session_unwrap((client.session),
                                recv_buf,
                                bytes_received,
                                processing_buf,
                                (size_t*)(&processing_buf_size));
    if ((THEMIS_SSESSION_SEND_OUTPUT_TO_PEER == res) && (processing_buf_size > 0)) {
        /* This test-send function never fails, so we do not check for error here */
        on_send_data(processing_buf, processing_buf_size, NULL);
        return TEST_CONTINUE;
    }

    testsuite_fail_if(true, "secure_session_unwrap failed");
    return TEST_STOP_ERROR;
}

static void server_function(void)
{
    uint8_t recv_buf[2048];
    ssize_t bytes_received;

    if (current_length > 0) {
        bytes_received = secure_session_receive((server.session), recv_buf, sizeof(recv_buf));
    } else {
        /* Nothing to receive. Do nothing */
        return;
    }

    if (bytes_received < 0) {
        testsuite_fail_if(bytes_received, "secure_session_receive failed");
        return;
    }

    size_t remote_id_length = 0;
    if (THEMIS_BUFFER_TOO_SMALL
        != secure_session_get_remote_id(server.session, NULL, &remote_id_length)) {
        testsuite_fail_if(true, "remote id getting failed (length_determination)");
        return;
    }
    uint8_t* remote_id = malloc(remote_id_length);
    assert(remote_id);
    if (THEMIS_SUCCESS != secure_session_get_remote_id(server.session, remote_id, &remote_id_length)) {
        testsuite_fail_if(true, "remote id getting failed");
        free(remote_id);
        return;
    }
    testsuite_fail_unless(remote_id_length == strlen(client.id)
                              && 0 == memcmp(remote_id, client.id, strlen(client.id)),
                          "secure_session remote id getting");
    free(remote_id);

    if (0 == bytes_received) {
        /* This was a key agreement packet. Nothing to do */
        return;
    }

    if (bytes_received > 0) {
        /* We received some data. Echo it back to the client. */
        ssize_t bytes_sent = secure_session_send((server.session), recv_buf, (size_t)bytes_received);

        if (bytes_sent == bytes_received) {
            /* Check whether data was indeed encrypted (it should not be the same as in
             * data_to_send) */
            testsuite_fail_if(((size_t)bytes_sent == current_length)
                                  || (!memcmp(recv_buf, shared_mem, bytes_sent)),
                              "secure_session server message wrap");
        } else {
            testsuite_fail_if(true, "secure_session_send failed");
        }
    }
}

static void server_function_no_transport(void)
{
    uint8_t recv_buf[2048];
    ssize_t bytes_received;
    uint8_t processing_buf[2048];
    ssize_t processing_buf_size = sizeof(processing_buf);
    themis_status_t res;
    if (current_length > 0) {
        bytes_received = on_receive_data(recv_buf, sizeof(recv_buf), NULL);
        res = secure_session_unwrap((server.session),
                                    recv_buf,
                                    bytes_received,
                                    processing_buf,
                                    (size_t*)(&processing_buf_size));
    } else {
        /* Nothing to receive. Do nothing */
        return;
    }

    size_t remote_id_length = 0;
    if (THEMIS_BUFFER_TOO_SMALL
        != secure_session_get_remote_id(server.session, NULL, &remote_id_length)) {
        testsuite_fail_if(true, "remote id getting failed (length_determination)");
        return;
    }
    uint8_t* remote_id = malloc(remote_id_length);
    assert(remote_id);
    if (THEMIS_SUCCESS != secure_session_get_remote_id(server.session, remote_id, &remote_id_length)) {
        testsuite_fail_if(true, "remote id getting failed");
        free(remote_id);
        return;
    }
    testsuite_fail_unless(remote_id_length == strlen(client.id)
                              && 0 == memcmp(remote_id, client.id, strlen(client.id)),
                          "secure_session remote id getting");
    free(remote_id);

    if ((THEMIS_SSESSION_SEND_OUTPUT_TO_PEER == res) && (processing_buf_size > 0)) {
        /* This is key agreement data. Return response to the client. */
        on_send_data(processing_buf, processing_buf_size, NULL);
        return;
    }
    if ((THEMIS_SUCCESS == res) && (processing_buf_size > 0)) {
        ssize_t bytes_sent = 0;

        /* This is actual data. Echo it to the client. */
        if (!secure_session_is_established((server.session))) {
            /* Should not happen */
            testsuite_fail_if(true, "secure_session_unwrap failed");
            return;
        }
        memcpy(recv_buf, processing_buf, (size_t)processing_buf_size);
        res = secure_session_wrap((server.session),
                                  recv_buf,
                                  processing_buf_size,
                                  processing_buf,
                                  (size_t*)(&bytes_sent));
        if (THEMIS_BUFFER_TOO_SMALL != res) {
            testsuite_fail_if(true, "secure_session_wrap failed");
            return;
        }

        res = secure_session_wrap((server.session),
                                  recv_buf,
                                  processing_buf_size,
                                  processing_buf,
                                  (size_t*)(&bytes_sent));
        if (THEMIS_SUCCESS != res) {
            testsuite_fail_if(true, "secure_session_wrap failed");
            return;
        }

        testsuite_fail_if(processing_buf_size == bytes_sent, "secure_session server message wrap");
        on_send_data(processing_buf, bytes_sent, NULL);
        return;
    }
    testsuite_fail_if(true, "secure_session_unwrap failed");
}

static void schedule(void)
{
    int res = client_function();

    while (TEST_CONTINUE == res) {
        server_function();
        res = client_function();
    }

    testsuite_fail_if(res, "secure session: basic flow");
}

static void schedule_no_transport(void)
{
    int res = client_function_no_transport();

    while (TEST_CONTINUE == res) {
        server_function_no_transport();
        res = client_function_no_transport();
    }

    testsuite_fail_if(res, "secure session: basic flow (no transport)");
}

static void test_basic_flow(void)
{
    themis_status_t res;

    memcpy(&(client.transport), &transport, sizeof(secure_session_user_callbacks_t));
    client.transport.user_data = &client;

    memcpy(&(server.transport), &transport, sizeof(secure_session_user_callbacks_t));
    server.transport.user_data = &server;

    client.session = secure_session_create(client.id,
                                           strlen(client.id),
                                           client.priv,
                                           client.priv_length,
                                           &(client.transport));
    if (client.session == NULL) {
        testsuite_fail_if(false, "secure_session_init failed");
        return;
    }

    server.session = secure_session_create(server.id,
                                           strlen(server.id),
                                           server.priv,
                                           server.priv_length,
                                           &(server.transport));
    if (server.session == NULL) {
        testsuite_fail_if(false, "secure_session_init failed");
        secure_session_destroy((client.session));
        return;
    }

    schedule();

    res = secure_session_destroy((server.session));
    if (res) {
        testsuite_fail_if(res, "secure_session_destroy failed");
    }

    res = secure_session_destroy((client.session));
    if (res) {
        testsuite_fail_if(res, "secure_session_destroy failed");
    }
}

static void test_basic_flow_no_transport(void)
{
    themis_status_t res;

    memcpy(&(client.transport), &transport, sizeof(secure_session_user_callbacks_t));
    client.transport.user_data = &client;

    memcpy(&(server.transport), &transport, sizeof(secure_session_user_callbacks_t));
    server.transport.user_data = &server;

    client.session = secure_session_create(client.id,
                                           strlen(client.id),
                                           client.priv,
                                           client.priv_length,
                                           &(client.transport));
    if (client.session == NULL) {
        testsuite_fail_if(false, "secure_session_init failed");
        return;
    }

    server.session = secure_session_create(server.id,
                                           strlen(server.id),
                                           server.priv,
                                           server.priv_length,
                                           &(server.transport));
    if (server.session == NULL) {
        testsuite_fail_if(false, "secure_session_init failed");
        secure_session_destroy((client.session));
        return;
    }

    schedule_no_transport();

    res = secure_session_destroy((server.session));
    if (res) {
        testsuite_fail_if(res, "secure_session_destroy failed");
    }

    res = secure_session_destroy((client.session));
    if (res) {
        testsuite_fail_if(res, "secure_session_destroy failed");
    }
}

static void test_empty_peer_id(void)
{
    memcpy(&(client.transport), &transport, sizeof(secure_session_user_callbacks_t));
    client.transport.user_data = &client;

    client.session = secure_session_create(client.id,
                                           0,
                                           client.priv,
                                           client.priv_length,
                                           &(client.transport));

    testsuite_fail_if(client.session != NULL, "secure_session_create NOT failed with empty Peer ID");
}

static void test_invalid_private_key_type(void)
{
    memcpy(&(client_rsa.transport), &transport, sizeof(secure_session_user_callbacks_t));
    client_rsa.transport.user_data = &client_rsa;

    client_rsa.session = secure_session_create(client_rsa.id,
                                               strlen(client_rsa.id),
                                               client_rsa.priv,
                                               client_rsa.priv_length,
                                               &(client_rsa.transport));

    testsuite_fail_if(client_rsa.session != NULL, "secure_session_create NOT failed with RSA key");
}

void run_secure_session_test(void)
{
    testsuite_enter_suite("secure session: basic flow");
    testsuite_run_test(test_basic_flow);

    testsuite_enter_suite("secure session: basic flow (no transport)");
    testsuite_run_test(test_basic_flow_no_transport);

    testsuite_enter_suite("secure session: invalid parameters");
    testsuite_run_test(test_empty_peer_id);
    testsuite_run_test(test_invalid_private_key_type);
}
