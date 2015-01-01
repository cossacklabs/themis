/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef SOTER_EC_KEY_H
#define SOTER_EC_KEY_H

#include <soter/soter_container.h>
#include <soter/soter.h>

#include <arpa/inet.h>

#define EC_PRIV_KEY_PREF "REC"
#define EC_PUB_KEY_PREF "UEC"

#define EC_256 "2"
#define EC_384 "3"
#define EC_521 "5"

#define EC_KEY_SUF(_KEY_SIZE_) EC_##_KEY_SIZE_

#define EC_PRIV_KEY_TAG(_KEY_SIZE_) (EC_PRIV_KEY_PREF EC_KEY_SUF(_KEY_SIZE_))
#define EC_PUB_KEY_TAG(_KEY_SIZE_) (EC_PUB_KEY_PREF EC_KEY_SUF(_KEY_SIZE_))

#define EC_BYTE_SIZE(_KEY_SIZE_) ((_KEY_SIZE_ + 7) / 8)

#define EC_PRIV_SIZE(_KEY_SIZE_) EC_BYTE_SIZE(_KEY_SIZE_)
#define EC_PUB_SIZE(_KEY_SIZE_) (EC_BYTE_SIZE(_KEY_SIZE_) + 1)

#define DECLARE_EC_PUBLIC_KEY(_KEY_SIZE_) \
	struct soter_ec_pub_key_##_KEY_SIZE_##_type \
	{ \
		soter_container_hdr_t hdr; \
		uint8_t d[EC_PUB_SIZE(_KEY_SIZE_)]; \
	}; \
	\
	typedef struct soter_ec_pub_key_##_KEY_SIZE_##_type soter_ec_pub_key_##_KEY_SIZE_##_t

/* struct members are ordered this way to avoid struct member alingment on different platforms */
#define DECLARE_EC_PRIVATE_KEY(_KEY_SIZE_) \
	struct soter_ec_priv_key_##_KEY_SIZE_##_type \
	{ \
		soter_container_hdr_t hdr; \
		uint8_t Q[EC_PRIV_SIZE(_KEY_SIZE_)]; \
	}; \
	\
	typedef struct soter_ec_priv_key_##_KEY_SIZE_##_type soter_ec_priv_key_##_KEY_SIZE_##_t

#define DECLARE_EC_KEY(_KEY_SIZE_) \
	DECLARE_EC_PUBLIC_KEY(_KEY_SIZE_); \
	DECLARE_EC_PRIVATE_KEY(_KEY_SIZE_)

DECLARE_EC_KEY(256);
DECLARE_EC_KEY(384);
DECLARE_EC_KEY(521);

/* This is considered internal API */
typedef void soter_engine_specific_ec_key_t;

soter_status_t soter_ec_pub_key_to_engine_specific(const soter_container_hdr_t *key, size_t key_length, soter_engine_specific_ec_key_t **engine_key);
soter_status_t soter_ec_priv_key_to_engine_specific(const soter_container_hdr_t *key, size_t key_length, soter_engine_specific_ec_key_t **engine_key);
soter_status_t soter_engine_specific_to_ec_priv_key(const soter_engine_specific_ec_key_t *engine_key, soter_container_hdr_t *key, size_t* key_length);
soter_status_t soter_engine_specific_to_ec_pub_key(const soter_engine_specific_ec_key_t *engine_key, soter_container_hdr_t *key, size_t* key_length);

#endif /* SOTER_EC_KEY_H */
