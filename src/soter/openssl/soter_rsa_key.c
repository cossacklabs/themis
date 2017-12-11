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

#include <soter/soter_error.h>
#include <soter/soter_rsa_key.h>

#include <openssl/evp.h>
#include <openssl/rsa.h>

#include <string.h>

static size_t rsa_pub_key_size(int mod_size)
{
	switch (mod_size)
	{
	case 128: /* 1024 */
		return sizeof(soter_rsa_pub_key_1024_t);
	case 256: /* 2048 */
		return sizeof(soter_rsa_pub_key_2048_t);
	case 512: /* 4096 */
		return sizeof(soter_rsa_pub_key_4096_t);
	case 1024: /* 8192 */
		return sizeof(soter_rsa_pub_key_8192_t);
	default:
		return 0;
	}
}

static size_t rsa_priv_key_size(int mod_size)
{
	switch (mod_size)
	{
	case 128: /* 1024 */
		return sizeof(soter_rsa_priv_key_1024_t);
	case 256: /* 2048 */
		return sizeof(soter_rsa_priv_key_2048_t);
	case 512: /* 4096 */
		return sizeof(soter_rsa_priv_key_4096_t);
	case 1024: /* 8192 */
		return sizeof(soter_rsa_priv_key_8192_t);
	default:
		return 0;
	}
}

static char* rsa_pub_key_tag(int mod_size)
{
	switch (mod_size)
	{
	case 128: /* 1024 */
		return RSA_PUB_KEY_TAG(1024);
	case 256: /* 2048 */
		return RSA_PUB_KEY_TAG(2048);
	case 512: /* 4096 */
		return RSA_PUB_KEY_TAG(4096);
	case 1024: /* 8192 */
		return RSA_PUB_KEY_TAG(8192);
	default:
		return NULL;
	}
}

static char* rsa_priv_key_tag(int mod_size)
{
	switch (mod_size)
	{
	case 128: /* 1024 */
		return RSA_PRIV_KEY_TAG(1024);
	case 256: /* 2048 */
		return RSA_PRIV_KEY_TAG(2048);
	case 512: /* 4096 */
		return RSA_PRIV_KEY_TAG(4096);
	case 1024: /* 8192 */
		return RSA_PRIV_KEY_TAG(8192);
	default:
		return NULL;
	}
}

static bool is_mod_size_supported(int mod_size)
{
	switch (mod_size)
	{
	case 128:
	case 256:
	case 512:
	case 1024:
		return true;
	default:
		return false;
	}
}

static soter_status_t bignum_to_bytes(const BIGNUM *bn, uint8_t *to, size_t to_length)
{
	size_t bn_size = (size_t)BN_num_bytes(bn);
	size_t bytes_copied;

	if (bn_size > to_length)
	{
		return SOTER_FAIL;
	}

	bytes_copied = BN_bn2bin(bn, to + (to_length - bn_size));

	if (bytes_copied != bn_size)
	{
		return SOTER_FAIL;
	}

	memset(to, 0, to_length - bn_size);

	return SOTER_SUCCESS;
}

soter_status_t soter_engine_specific_to_rsa_pub_key(const soter_engine_specific_rsa_key_t *engine_key, soter_container_hdr_t *key, size_t* key_length)
{
	EVP_PKEY *pkey = (EVP_PKEY *)engine_key;
	RSA *rsa;
	soter_status_t res;
	int rsa_mod_size;
	size_t output_length;
	uint32_t *pub_exp;

	if (!key_length)
	{
		return SOTER_INVALID_PARAMETER;
	}

	if (EVP_PKEY_RSA != EVP_PKEY_id(pkey))
	{
		return SOTER_INVALID_PARAMETER;
	}

	rsa = EVP_PKEY_get1_RSA((EVP_PKEY *)pkey);
	if (NULL == rsa)
	{
		return SOTER_FAIL;
	}

	rsa_mod_size = RSA_size(rsa);
	if (!is_mod_size_supported(rsa_mod_size))
	{
		res = SOTER_INVALID_PARAMETER;
		goto err;
	}

	output_length = rsa_pub_key_size(rsa_mod_size);
	if ((!key) || (output_length > *key_length))
	{
		*key_length = output_length;
		res = SOTER_BUFFER_TOO_SMALL;
		goto err;
	}

	pub_exp = (uint32_t *)((unsigned char *)(key + 1) + rsa_mod_size);
	const BIGNUM* rsa_e;
	BIGNUM* rsa_n;
#if OPENSSL_VERSION_NUMBER < 0x10100000L
	rsa_e = rsa->e;
	rsa_n = rsa->n;
#else
	RSA_get0_key(rsa, (const BIGNUM**)&rsa_n, &rsa_e, NULL);
#endif
	if (BN_is_word(rsa_e, RSA_F4))
	{
		*pub_exp = htonl(RSA_F4);
	}
	else if (BN_is_word(rsa_e, RSA_3))
	{
		*pub_exp = htonl(RSA_3);
	}
	else
	{
		res = SOTER_INVALID_PARAMETER;
		goto err;
	}

	res = bignum_to_bytes(rsa_n, (unsigned char *)(key + 1), rsa_mod_size);
	if (SOTER_SUCCESS != res)
	{
		goto err;
	}

	memcpy(key->tag, rsa_pub_key_tag(rsa_mod_size), SOTER_CONTAINER_TAG_LENGTH);
	key->size = htonl(output_length);
	soter_update_container_checksum(key);
	*key_length = output_length;
	res = SOTER_SUCCESS;

err:
	/* Free extra reference on RSA object provided by EVP_PKEY_get1_RSA */
	RSA_free(rsa);

	return res;
}

soter_status_t soter_engine_specific_to_rsa_priv_key(const soter_engine_specific_rsa_key_t *engine_key, soter_container_hdr_t *key, size_t* key_length)
{
	EVP_PKEY *pkey = (EVP_PKEY *)engine_key;
	RSA *rsa;
	soter_status_t res;
	int rsa_mod_size;
	size_t output_length;
	uint32_t *pub_exp;
	unsigned char *curr_bn = (unsigned char *)(key + 1);

	if (!key_length)
	{
		return SOTER_INVALID_PARAMETER;
	}

	if (EVP_PKEY_RSA != EVP_PKEY_id(pkey))
	{
		return SOTER_INVALID_PARAMETER;
	}

	rsa = EVP_PKEY_get1_RSA((EVP_PKEY *)pkey);
	if (NULL == rsa)
	{
		return SOTER_FAIL;
	}

	rsa_mod_size = RSA_size(rsa);
	if (!is_mod_size_supported(rsa_mod_size))
	{
		res = SOTER_INVALID_PARAMETER;
		goto err;
	}

	output_length = rsa_priv_key_size(rsa_mod_size);
	if ((!key) || (output_length > *key_length))
	{
		*key_length = output_length;
		res = SOTER_BUFFER_TOO_SMALL;
		goto err;
	}

	pub_exp = (uint32_t *)(curr_bn + ((rsa_mod_size * 4) + (rsa_mod_size / 2)));
	const BIGNUM* rsa_e;
	const BIGNUM* rsa_d;
	const BIGNUM* rsa_n;
#if OPENSSL_VERSION_NUMBER < 0x10100000L
	rsa_e = rsa->e;
	rsa_d = rsa->d;
	rsa_n = rsa->n;
#else
	RSA_get0_key(rsa, &rsa_n, &rsa_e, &rsa_d);
#endif
	if (BN_is_word(rsa_e, RSA_F4))
	{
		*pub_exp = htonl(RSA_F4);
	}
	else if (BN_is_word(rsa_e, RSA_3))
	{
		*pub_exp = htonl(RSA_3);
	}
	else
	{
		res = SOTER_INVALID_PARAMETER;
		goto err;
	}

	/* Private exponent */
	res = bignum_to_bytes(rsa_d, curr_bn, rsa_mod_size);
	if (SOTER_SUCCESS != res)
	{
		goto err;
	}
	curr_bn += rsa_mod_size;

	const BIGNUM* rsa_p;
	const BIGNUM* rsa_q;
#if OPENSSL_VERSION_NUMBER < 0x10100000L
	rsa_p = rsa->p;
	rsa_q = rsa->q;
#else
	RSA_get0_factors(rsa, &rsa_p, &rsa_q);
#endif

	/* p */
	res = bignum_to_bytes(rsa_p, curr_bn, rsa_mod_size / 2);
	if (SOTER_SUCCESS != res)
	{
		goto err;
	}
	curr_bn += rsa_mod_size / 2;

	/* q */
	res = bignum_to_bytes(rsa_q, curr_bn, rsa_mod_size / 2);
	if (SOTER_SUCCESS != res)
	{
		goto err;
	}
	curr_bn += rsa_mod_size / 2;

	const BIGNUM* rsa_dmp1;
	const BIGNUM* rsa_dmq1;
	const BIGNUM* rsa_iqmp;
#if OPENSSL_VERSION_NUMBER < 0x10100000L
	rsa_dmp1 = rsa->dmp1;
	rsa_dmq1 = rsa->dmq1;
	rsa_iqmp = rsa->iqmp;
#else
	RSA_get0_crt_params(rsa, &rsa_dmp1, &rsa_dmq1, &rsa_iqmp);
#endif

	/* dp */
	res = bignum_to_bytes(rsa_dmp1, curr_bn, rsa_mod_size / 2);
	if (SOTER_SUCCESS != res)
	{
		goto err;
	}
	curr_bn += rsa_mod_size / 2;

	/* dq */
	res = bignum_to_bytes(rsa_dmq1, curr_bn, rsa_mod_size / 2);
	if (SOTER_SUCCESS != res)
	{
		goto err;
	}
	curr_bn += rsa_mod_size / 2;

	/* qp */
	res = bignum_to_bytes(rsa_iqmp, curr_bn, rsa_mod_size / 2);
	if (SOTER_SUCCESS != res)
	{
		goto err;
	}
	curr_bn += rsa_mod_size / 2;

	/* modulus */
	res = bignum_to_bytes(rsa_n, curr_bn, rsa_mod_size);
	if (SOTER_SUCCESS != res)
	{
		goto err;
	}

	memcpy(key->tag, rsa_priv_key_tag(rsa_mod_size), SOTER_CONTAINER_TAG_LENGTH);
	key->size = htonl(output_length);
	soter_update_container_checksum(key);
	*key_length = output_length;
	res = SOTER_SUCCESS;

err:
	/* Free extra reference on RSA object provided by EVP_PKEY_get1_RSA */
	RSA_free(rsa);

//	if (SOTER_SUCCESS != res)
//	{
//		/* Zero output memory to avoid leaking private key information */
//		memset(key, 0, *key_length);
//	}

	return res;
}

/* TODO: Maybe, basic container validation should be put in separate functions outside of engine-specific code */
soter_status_t soter_rsa_pub_key_to_engine_specific(const soter_container_hdr_t *key, size_t key_length, soter_engine_specific_rsa_key_t **engine_key)
{
	int rsa_mod_size;
	RSA *rsa;
	EVP_PKEY *pkey = (EVP_PKEY *)(*engine_key);
	const uint32_t *pub_exp;

	if (key_length != ntohl(key->size))
	{
		return SOTER_INVALID_PARAMETER;
	}

	/* Validate tag */
	if (memcmp(key->tag, RSA_PUB_KEY_PREF, strlen(RSA_PUB_KEY_PREF)))
	{
		return SOTER_INVALID_PARAMETER;
	}

	if (SOTER_SUCCESS != soter_verify_container_checksum(key))
	{
		return SOTER_DATA_CORRUPT;
	}

	switch (key->tag[3])
	{
	case '1':
		rsa_mod_size = 128;
		break;
	case '2':
		rsa_mod_size = 256;
		break;
	case '4':
		rsa_mod_size = 512;
		break;
	case '8':
		rsa_mod_size = 1024;
		break;
	default:
		return SOTER_INVALID_PARAMETER;
	}

	if (key_length < rsa_pub_key_size(rsa_mod_size))
	{
		return SOTER_INVALID_PARAMETER;
	}

	pub_exp = (const uint32_t *)((unsigned char *)(key + 1) + rsa_mod_size);
	switch (ntohl(*pub_exp))
	{
	case RSA_3:
	case RSA_F4:
		break;
	default:
		return SOTER_INVALID_PARAMETER;
	}

	rsa = RSA_new();
	if (!rsa)
	{
		return SOTER_NO_MEMORY;
	}

	BIGNUM* rsa_n = BN_new();
	if(!rsa_n){
		return SOTER_NO_MEMORY;
	}
	BIGNUM* rsa_e = BN_new();
	if (!rsa_e)
	{
		RSA_free(rsa);
		BN_free(rsa_n);
		return SOTER_NO_MEMORY;
	}

	if (!BN_set_word(rsa_e, ntohl(*pub_exp)))
	{
		BN_free(rsa_n);
		BN_free(rsa_e);
		RSA_free(rsa);
		return SOTER_FAIL;
	}

	if (!BN_bin2bn((const unsigned char *)(key + 1), rsa_mod_size, rsa_n))
	{
		BN_free(rsa_n);
		BN_free(rsa_e);
		RSA_free(rsa);
		return SOTER_FAIL;
	}
#if OPENSSL_VERSION_NUMBER < 0x10100000L
	rsa->e = rsa_e;
	rsa->n = rsa_n;
#else
	RSA_set0_key(rsa, rsa_n, rsa_e, NULL);
#endif


	if (!EVP_PKEY_assign_RSA(pkey, rsa))
	{
		BN_free(rsa_n);
		BN_free(rsa_e);
		RSA_free(rsa);
		return SOTER_FAIL;
	}

	/* EVP_PKEY_assign_RSA does not increment the reference count, so no need to free RSA object */
	return SOTER_SUCCESS;
}

soter_status_t soter_rsa_priv_key_to_engine_specific(const soter_container_hdr_t *key, size_t key_length, soter_engine_specific_rsa_key_t **engine_key)
{
	int rsa_mod_size;
	RSA *rsa;
	EVP_PKEY *pkey = (EVP_PKEY *)(*engine_key);
	const uint32_t *pub_exp;
	const unsigned char *curr_bn = (const unsigned char *)(key + 1);

	if (key_length != ntohl(key->size))
	{
		return SOTER_INVALID_PARAMETER;
	}

	/* Validate tag */
	if (memcmp(key->tag, RSA_PRIV_KEY_PREF, strlen(RSA_PRIV_KEY_PREF)))
	{
		return SOTER_INVALID_PARAMETER;
	}

	if (SOTER_SUCCESS != soter_verify_container_checksum(key))
	{
		return SOTER_DATA_CORRUPT;
	}

	switch (key->tag[3])
	{
	case '1':
		rsa_mod_size = 128;
		break;
	case '2':
		rsa_mod_size = 256;
		break;
	case '4':
		rsa_mod_size = 512;
		break;
	case '8':
		rsa_mod_size = 1024;
		break;
	default:
		return SOTER_INVALID_PARAMETER;
	}

	if (key_length < rsa_priv_key_size(rsa_mod_size))
	{
		return SOTER_INVALID_PARAMETER;
	}

	pub_exp = (const uint32_t *)(curr_bn + ((rsa_mod_size * 4) + (rsa_mod_size / 2)));;
	switch (ntohl(*pub_exp))
	{
	case RSA_3:
	case RSA_F4:
		break;
	default:
		return SOTER_INVALID_PARAMETER;
	}

	rsa = RSA_new();
	if (!rsa)
	{
		return SOTER_NO_MEMORY;
	}
	BIGNUM* rsa_e = BN_new();
	if (!rsa_e)
	{
		RSA_free(rsa);
		return SOTER_NO_MEMORY;
	}

	if (!BN_set_word(rsa_e, ntohl(*pub_exp)))
	{
		RSA_free(rsa);
		return SOTER_FAIL;
	}

	/* Private exponent */
	BIGNUM* rsa_d = BN_new();
	if (!rsa_d)
	{
		RSA_free(rsa);
		return SOTER_NO_MEMORY;
	}

	if (!BN_bin2bn(curr_bn, rsa_mod_size, rsa_d))
	{
		RSA_free(rsa);
		return SOTER_FAIL;
	}
	curr_bn += rsa_mod_size;

	BIGNUM* rsa_p = BN_new();
	if (!rsa_p)
	{
		RSA_free(rsa);
		return SOTER_NO_MEMORY;
	}

	if (!BN_bin2bn(curr_bn, rsa_mod_size / 2, rsa_p))
	{
		RSA_free(rsa);
		return SOTER_FAIL;
	}
	curr_bn += rsa_mod_size / 2;

	/* q */
	BIGNUM* rsa_q = BN_new();
	if (!rsa_q)
	{
		RSA_free(rsa);
		return SOTER_NO_MEMORY;
	}

	if (!BN_bin2bn(curr_bn, rsa_mod_size / 2, rsa_q))
	{
		RSA_free(rsa);
		return SOTER_FAIL;
	}
	curr_bn += rsa_mod_size / 2;

	/* dp */
	BIGNUM* rsa_dmp1 = BN_new();
	if (!rsa_dmp1)
	{
		RSA_free(rsa);
		return SOTER_NO_MEMORY;
	}

	if (!BN_bin2bn(curr_bn, rsa_mod_size / 2, rsa_dmp1))
	{
		RSA_free(rsa);
		return SOTER_FAIL;
	}
	curr_bn += rsa_mod_size / 2;

	/* dq */
	BIGNUM* rsa_dmq1 = BN_new();
	if (!rsa_dmq1)
	{
		RSA_free(rsa);
		return SOTER_NO_MEMORY;
	}

	if (!BN_bin2bn(curr_bn, rsa_mod_size / 2, rsa_dmq1))
	{
		RSA_free(rsa);
		return SOTER_FAIL;
	}
	curr_bn += rsa_mod_size / 2;

	/* qp */
	BIGNUM* rsa_iqmp = BN_new();
	if (!rsa_iqmp)
	{
		RSA_free(rsa);
		return SOTER_NO_MEMORY;
	}

	if (!BN_bin2bn(curr_bn, rsa_mod_size / 2, rsa_iqmp))
	{
		RSA_free(rsa);
		return SOTER_FAIL;
	}
	curr_bn += rsa_mod_size / 2;

	/* modulus */
	BIGNUM* rsa_n = BN_new();
	if (!rsa_n)
	{
		RSA_free(rsa);
		return SOTER_NO_MEMORY;
	}

	if (!BN_bin2bn(curr_bn, rsa_mod_size, rsa_n))
	{
		RSA_free(rsa);
		return SOTER_FAIL;
	}
	/* If at least one CRT parameter is zero, free them */
	if (BN_is_zero(rsa_p) || BN_is_zero(rsa_q) || BN_is_zero(rsa_dmp1) || BN_is_zero(rsa_dmq1) || BN_is_zero(rsa_iqmp))
	{
		BN_free(rsa_p);
		rsa_p = NULL;

		BN_free(rsa_q);
		rsa_q = NULL;

		BN_free(rsa_dmp1);
		rsa_dmp1 = NULL;

		BN_free(rsa_dmq1);
		rsa_dmq1 = NULL;

		BN_free(rsa_iqmp);
		rsa_iqmp = NULL;
	}
#if OPENSSL_VERSION_NUMBER < 0x10100000L
	rsa->n = rsa_n;
	rsa->e = rsa_e;
	rsa->d = rsa_d;
	rsa->dmq1 = rsa_dmq1;
	rsa->dmp1 = rsa_dmp1;
	rsa->iqmp = rsa_iqmp;
	rsa->q = rsa_q;
	rsa->p = rsa_p;
#else
	RSA_set0_key(rsa, rsa_n, rsa_e, rsa_d);
	RSA_set0_factors(rsa, rsa_p, rsa_q);
	RSA_set0_crt_params(rsa, rsa_dmp1, rsa_dmq1, rsa_iqmp);
#endif

	if (!EVP_PKEY_assign_RSA(pkey, rsa))
	{
		RSA_free(rsa);
		return SOTER_FAIL;
	}

	/* EVP_PKEY_assign_RSA does not increment the reference count, so no need to free RSA object */
	return SOTER_SUCCESS;
}
