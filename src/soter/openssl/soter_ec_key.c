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

#include <soter/error.h>
#include <soter/soter_ec_key.h>

#include <openssl/evp.h>
#include <openssl/ec.h>

#include <string.h>

static bool is_curve_supported(int curve)
{
	switch (curve)
	{
	case NID_X9_62_prime256v1:
	case NID_secp384r1:
	case NID_secp521r1:
		return true;
	default:
		return false;
	}
}

/* Input size directly since public key type structures may be aligned to word boundary */
static size_t ec_pub_key_size(int curve)
{
	switch (curve)
	{
	case NID_X9_62_prime256v1: /* P-256 */
		return sizeof(soter_container_hdr_t) + EC_PUB_SIZE(256);
	case NID_secp384r1: /* P-384 */
		return sizeof(soter_container_hdr_t) + EC_PUB_SIZE(384);
	case NID_secp521r1: /* P-521 */
		return sizeof(soter_container_hdr_t) + EC_PUB_SIZE(521);
	default:
		return 0;
	}
}

static size_t ec_priv_key_size(int curve)
{
	switch (curve)
	{
	case NID_X9_62_prime256v1: /* P-256 */
		return sizeof(soter_ec_priv_key_256_t);
	case NID_secp384r1: /* P-384 */
		return sizeof(soter_ec_priv_key_384_t);
	case NID_secp521r1: /* P-521 */
		return sizeof(soter_ec_priv_key_521_t);
	default:
		return 0;
	}
}

static char* ec_pub_key_tag(int curve)
{
	switch (curve)
	{
	case NID_X9_62_prime256v1: /* P-256 */
		return EC_PUB_KEY_TAG(256);
	case NID_secp384r1: /* P-384 */
		return EC_PUB_KEY_TAG(384);
	case NID_secp521r1: /* P-521 */
		return EC_PUB_KEY_TAG(521);
	default:
		return NULL;
	}
}

static char* ec_priv_key_tag(int curve)
{
	switch (curve)
	{
	case NID_X9_62_prime256v1: /* P-256 */
		return EC_PRIV_KEY_TAG(256);
	case NID_secp384r1: /* P-384 */
		return EC_PRIV_KEY_TAG(384);
	case NID_secp521r1: /* P-521 */
		return EC_PRIV_KEY_TAG(521);
	default:
		return NULL;
	}
}

static size_t bn_encode(const BIGNUM* bn, uint8_t* buffer, size_t length)
{
	int bn_size = BN_num_bytes(bn);
	if (length < (size_t)bn_size)
		return 0;
	memset(buffer, 0, length - bn_size);
	return (length - bn_size) + BN_bn2bin(bn, buffer + (length - bn_size));
}

soter_status_t soter_engine_specific_to_ec_pub_key(const soter_engine_specific_ec_key_t *engine_key, soter_container_hdr_t *key, size_t* key_length)
{
	EVP_PKEY *pkey = (EVP_PKEY *)engine_key;
	soter_status_t res;
	size_t output_length;
	EC_KEY *ec;
	const EC_GROUP *group;
	const EC_POINT *Q;
	int curve;

	if ((!key_length) || (EVP_PKEY_EC != EVP_PKEY_id(pkey)))
	{
		return SOTER_INVALID_PARAMETER;
	}

	ec = EVP_PKEY_get1_EC_KEY((EVP_PKEY *)pkey);
	if (NULL == ec)
	{
		return SOTER_FAIL;
	}

	group = EC_KEY_get0_group(ec);
	if (NULL == group)
	{
		res = SOTER_INVALID_PARAMETER;
		goto err;
	}

	curve = EC_GROUP_get_curve_name(group);
	if (!is_curve_supported(curve))
	{
		res = SOTER_INVALID_PARAMETER;
		goto err;
	}

	output_length = ec_pub_key_size(curve);
	if ((!key) || (output_length > *key_length))
	{
		*key_length = output_length;
		res = SOTER_BUFFER_TOO_SMALL;
		goto err;
	}

	*key_length = output_length;

	Q = EC_KEY_get0_public_key(ec);
	if (NULL == Q)
	{
		res = SOTER_INVALID_PARAMETER;
		goto err;
	}

	if ((output_length - sizeof(soter_container_hdr_t)) != EC_POINT_point2oct(group, Q, POINT_CONVERSION_COMPRESSED, (unsigned char *)(key + 1), output_length - sizeof(soter_container_hdr_t), NULL))
	{
		res = SOTER_FAIL;
		goto err;
	}

	memcpy(key->tag, ec_pub_key_tag(curve), SOTER_CONTAINER_TAG_LENGTH);
	key->size = htonl(output_length);
	soter_update_container_checksum(key);
	*key_length = output_length;
	res = SOTER_SUCCESS;

err:
	/* Free extra reference on EC_KEY object provided by EVP_PKEY_get1_EC_KEY */
	EC_KEY_free(ec);

	return res;
}

soter_status_t soter_engine_specific_to_ec_priv_key(const soter_engine_specific_ec_key_t *engine_key, soter_container_hdr_t *key, size_t* key_length)
{
	EVP_PKEY *pkey = (EVP_PKEY *)engine_key;
	soter_status_t res;
	size_t output_length;
	EC_KEY *ec;
	const EC_GROUP *group;
	const BIGNUM *d;
	int curve;

	if ((!key_length) || (EVP_PKEY_EC != EVP_PKEY_id(pkey)))
	{
		return SOTER_INVALID_PARAMETER;
	}

	ec = EVP_PKEY_get1_EC_KEY((EVP_PKEY *)pkey);
	if (NULL == ec)
	{
		return SOTER_FAIL;
	}

	group = EC_KEY_get0_group(ec);
	if (NULL == group)
	{
		res = SOTER_INVALID_PARAMETER;
		goto err;
	}

	curve = EC_GROUP_get_curve_name(group);
	if (!is_curve_supported(curve))
	{
		res = SOTER_INVALID_PARAMETER;
		goto err;
	}

	output_length = ec_pub_key_size(curve);
	if ((!key) || (output_length > *key_length))
	{
		*key_length = output_length;
		res = SOTER_BUFFER_TOO_SMALL;
		goto err;
	}

	*key_length = output_length;

	d = EC_KEY_get0_private_key(ec);
	if (NULL == d)
	{
		res = SOTER_INVALID_PARAMETER;
		goto err;
	}

	if ((output_length - sizeof(soter_container_hdr_t)) != bn_encode(d, (unsigned char *)(key + 1), output_length - sizeof(soter_container_hdr_t)))
	{
		res = SOTER_FAIL;
		goto err;
	}

	memcpy(key->tag, ec_priv_key_tag(curve), SOTER_CONTAINER_TAG_LENGTH);
	key->size = htonl(output_length);
	soter_update_container_checksum(key);
	*key_length = output_length;
	res = SOTER_SUCCESS;

err:
	/* Free extra reference on EC_KEY object provided by EVP_PKEY_get1_EC_KEY */
	EC_KEY_free(ec);

	return res;
}

soter_status_t soter_ec_pub_key_to_engine_specific(const soter_container_hdr_t *key, size_t key_length, soter_engine_specific_ec_key_t **engine_key)
{
	int curve;
	EC_KEY *ec=NULL;
	const EC_GROUP *group;
	EC_POINT *Q=NULL;
	EVP_PKEY *pkey = (EVP_PKEY *)(*engine_key);
	soter_status_t res;

	if ((!key) || (key_length < sizeof(soter_container_hdr_t)))
	{
		return SOTER_INVALID_PARAMETER;
	}

	if (key_length != ntohl(key->size))
	{
		return SOTER_INVALID_PARAMETER;
	}

	/* Validate tag */
	if (memcmp(key->tag, EC_PUB_KEY_PREF, strlen(EC_PUB_KEY_PREF)))
	{
		return SOTER_INVALID_PARAMETER;
	}

	if (SOTER_SUCCESS != soter_verify_container_checksum(key))
	{
		return SOTER_DATA_CORRUPT;
	}

	switch (key->tag[3])
	{
	case '2':
		curve = NID_X9_62_prime256v1;
		break;
	case '3':
		curve = NID_secp384r1;
		break;
	case '5':
		curve = NID_secp521r1;
		break;
	default:
		return SOTER_INVALID_PARAMETER;
	}

	if (key_length < ec_pub_key_size(curve))
	{
		return SOTER_INVALID_PARAMETER;
	}

	ec = EC_KEY_new_by_curve_name(curve);
	if (NULL == ec)
	{
		return SOTER_FAIL;
	}

	group = EC_KEY_get0_group(ec);
	if (NULL == group)
	{
		res = SOTER_FAIL;
		goto err;
	}

	Q = EC_POINT_new(group);
	if (NULL == group)
	{
		res = SOTER_NO_MEMORY;
		goto err;
	}

	if (1 != EC_POINT_oct2point(group, Q, (const unsigned char *)(key + 1), (int)(key_length - sizeof(soter_container_hdr_t)), NULL))
	{
		res = SOTER_INVALID_PARAMETER;
		goto err;
	}

	if (1 != EC_KEY_set_public_key(ec, Q))
	{
		res = SOTER_FAIL;
		goto err;
	}

	EC_POINT_free(Q);
	Q = NULL;

	if (EVP_PKEY_assign_EC_KEY(pkey, ec))
	{
		res = SOTER_SUCCESS;
		ec = NULL;
	}
	else
	{
		res = SOTER_FAIL;
	}

err:

	if (Q)
	{
		EC_POINT_free(Q);
	}

	if (ec)
	{
		EC_KEY_free(ec);
	}

	return res;
}

soter_status_t soter_ec_priv_key_to_engine_specific(const soter_container_hdr_t *key, size_t key_length, soter_engine_specific_ec_key_t **engine_key)
{
	int curve;
	EC_KEY *ec=NULL;
	const EC_GROUP *group;
	BIGNUM *d=NULL;
	EVP_PKEY *pkey = (EVP_PKEY *)(*engine_key);
	soter_status_t res;

	if (key_length != ntohl(key->size))
	{
		return SOTER_INVALID_PARAMETER;
	}

	/* Validate tag */
	if (memcmp(key->tag, EC_PRIV_KEY_PREF, strlen(EC_PRIV_KEY_PREF)))
	{
		return SOTER_INVALID_PARAMETER;
	}

	if (SOTER_SUCCESS != soter_verify_container_checksum(key))
	{
		return SOTER_DATA_CORRUPT;
	}

	switch (key->tag[3])
	{
	case '2':
		curve = NID_X9_62_prime256v1;
		break;
	case '3':
		curve = NID_secp384r1;
		break;
	case '5':
		curve = NID_secp521r1;
		break;
	default:
		return SOTER_INVALID_PARAMETER;
	}

	if (key_length < ec_priv_key_size(curve))
	{
		return SOTER_INVALID_PARAMETER;
	}

	ec = EC_KEY_new_by_curve_name(curve);
	if (NULL == ec)
	{
		return SOTER_FAIL;
	}

	group = EC_KEY_get0_group(ec);
	if (NULL == group)
	{
		res = SOTER_FAIL;
		goto err;
	}

	d = BN_bin2bn((const unsigned char *)(key + 1),(int)(key_length - sizeof(soter_container_hdr_t)), NULL);
	if (NULL == d)
	{
		res = SOTER_NO_MEMORY;
		goto err;
	}

	if (1 != EC_KEY_set_private_key(ec, d))
	{
		res = SOTER_FAIL;
		goto err;
	}

	BN_clear_free(d);
	d = NULL;

	if (EVP_PKEY_assign_EC_KEY(pkey, ec))
	{
		res = SOTER_SUCCESS;
		ec = NULL;
	}
	else
	{
		res = SOTER_FAIL;
	}

err:

	if (d)
	{
		BN_clear_free(d);
	}

	if (ec)
	{
		EC_KEY_free(ec);
	}

	return res;
}
