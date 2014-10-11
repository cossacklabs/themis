/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef SOTER_SOTER_RSA_KEY_H
#define SOTER_SOTER_RSA_KEY_H

#include "soter_container.h"

#define RSA_BYTE_SIZE(_KEY_SIZE_) (_KEY_SIZE_ / 8)

#define DECLARE_RSA_PUBLIC_KEY(_KEY_SIZE_) \
	struct soter_rsa_pub_key_##_KEY_SIZE_##_type \
	{ \
		soter_container_hdr_t hdr; \
		uint8_t mod[RSA_BYTE_SIZE(_KEY_SIZE_)]; \
		uint32_t pub_exp; \
	}; \
	\
	typedef struct soter_rsa_pub_key_##_KEY_SIZE_##_type soter_rsa_pub_key_##_KEY_SIZE_##_t

/* Our RSA private key containers include CRT params, since most crypto libraries support them. If at some point CRT params are not available, respective fields a filled with zeroes. */
/* struct members are ordered this way to avoid struct member alingment on different platforms */
#define DECLARE_RSA_PRIVATE_KEY(_KEY_SIZE_) \
	struct soter_rsa_priv_key_##_KEY_SIZE_##_type \
	{ \
		soter_container_hdr_t hdr; \
		uint8_t priv_exp[RSA_BYTE_SIZE(_KEY_SIZE_)]; \
		uint8_t p[RSA_BYTE_SIZE(_KEY_SIZE_) / 2]; \
		uint8_t q[RSA_BYTE_SIZE(_KEY_SIZE_) / 2]; \
		uint8_t dp[RSA_BYTE_SIZE(_KEY_SIZE_) / 2]; \
		uint8_t dq[RSA_BYTE_SIZE(_KEY_SIZE_) / 2]; \
		uint8_t qp[RSA_BYTE_SIZE(_KEY_SIZE_) / 2]; \
		uint8_t mod[RSA_BYTE_SIZE(_KEY_SIZE_)]; \
		uint32_t pub_exp; \
	}; \
	\
	typedef struct soter_rsa_priv_key_##_KEY_SIZE_##_type soter_rsa_priv_key_##_KEY_SIZE_##_t

#define DECLARE_RSA_KEY(_KEY_SIZE_) \
	DECLARE_RSA_PUBLIC_KEY(_KEY_SIZE_); \
	DECLARE_RSA_PRIVATE_KEY(_KEY_SIZE_)

/* We support 1024, 2048, 4096, 8192 RSA keys */
DECLARE_RSA_KEY(1024);
DECLARE_RSA_KEY(2048);
DECLARE_RSA_KEY(4096);
DECLARE_RSA_KEY(8192);

#endif /* SOTER_SOTER_RSA_KEY_H */
