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
#include "soter_openssl.h"
#include <soter/soter_ec_key.h>
#include <openssl/ec.h>

static int soter_alg_to_curve_nid(soter_asym_ka_alg_t alg)
{
	switch (alg)
	{
	case SOTER_ASYM_KA_EC_P256:
		return NID_X9_62_prime256v1;
	default:
		return 0;
	}
}

soter_status_t soter_asym_ka_init(soter_asym_ka_t* asym_ka_ctx, soter_asym_ka_alg_t alg)
{
	EVP_PKEY *pkey;
	int nid = soter_alg_to_curve_nid(alg);

	if ((!asym_ka_ctx) || (0 == nid))
	{
		return SOTER_INVALID_PARAMETER;
	}

	pkey = EVP_PKEY_new();
	if (!pkey)
	{
		return SOTER_NO_MEMORY;
	}

	if (!EVP_PKEY_set_type(pkey, EVP_PKEY_EC))
	{
		EVP_PKEY_free(pkey);
		return SOTER_FAIL;
	}

	asym_ka_ctx->pkey_ctx = EVP_PKEY_CTX_new(pkey, NULL);
	if (!(asym_ka_ctx->pkey_ctx))
	{
		EVP_PKEY_free(pkey);
		return SOTER_FAIL;
	}
	if (1 != EVP_PKEY_paramgen_init(asym_ka_ctx->pkey_ctx))
	{
		EVP_PKEY_free(pkey);
		return SOTER_FAIL;
	}

	if (1 != EVP_PKEY_CTX_set_ec_paramgen_curve_nid(asym_ka_ctx->pkey_ctx, nid))
	{
		EVP_PKEY_free(pkey);
		return SOTER_FAIL;
	}

	if (1 != EVP_PKEY_paramgen(asym_ka_ctx->pkey_ctx, &pkey))
	{
		EVP_PKEY_free(pkey);
		return SOTER_FAIL;
	}

	/*if (1 != EVP_PKEY_CTX_ctrl(asym_ka_ctx->pkey_ctx, EVP_PKEY_EC, -1, EVP_PKEY_CTRL_EC_PARAMGEN_CURVE_NID, nid, NULL))
	{
		EVP_PKEY_free(pkey);
		return SOTER_FAIL;
	}*/

	return SOTER_SUCCESS;
}

soter_status_t soter_asym_ka_cleanup(soter_asym_ka_t* asym_ka_ctx)
{
	if (!asym_ka_ctx)
	{
		return SOTER_INVALID_PARAMETER;
	}
	if (asym_ka_ctx->pkey_ctx)
	{

		EVP_PKEY_CTX_free(asym_ka_ctx->pkey_ctx);
	}
	return SOTER_SUCCESS;
}

soter_asym_ka_t* soter_asym_ka_create(soter_asym_ka_alg_t alg)
{
	soter_status_t status;
	soter_asym_ka_t *ctx = malloc(sizeof(soter_asym_ka_t));
	if (!ctx)
	{
		return NULL;
	}

	status = soter_asym_ka_init(ctx, alg);
	if (SOTER_SUCCESS == status)
	{
		return ctx;
	}
	else
	{
		free(ctx);
		return NULL;
	}
}

soter_status_t soter_asym_ka_destroy(soter_asym_ka_t* asym_ka_ctx)
{
	soter_status_t status;

	if (!asym_ka_ctx)
	{
		return SOTER_INVALID_PARAMETER;
	}

	status = soter_asym_ka_cleanup(asym_ka_ctx);
	if (SOTER_SUCCESS == status)
	{
		free(asym_ka_ctx);
		return SOTER_SUCCESS;
	}
	else
	{
		return status;
	}
}

soter_status_t soter_asym_ka_gen_key(soter_asym_ka_t* asym_ka_ctx)
{
	EVP_PKEY *pkey;
	EC_KEY *ec;

	if (!asym_ka_ctx)
	{
		return SOTER_INVALID_PARAMETER;
	}

	pkey = EVP_PKEY_CTX_get0_pkey(asym_ka_ctx->pkey_ctx);

	if (!pkey)
	{
		return SOTER_INVALID_PARAMETER;
	}

	if (EVP_PKEY_EC != EVP_PKEY_id(pkey))
	{
		return SOTER_INVALID_PARAMETER;
	}

	ec = EVP_PKEY_get0(pkey);
	if (NULL == ec)
	{
		return SOTER_INVALID_PARAMETER;
	}

	if (1 == EC_KEY_generate_key(ec))
	{
		return SOTER_SUCCESS;
	}
	else
	{
		return SOTER_FAIL;
	}
}

soter_status_t soter_asym_ka_import_key(soter_asym_ka_t* asym_ka_ctx, const void* key, size_t key_length)
{
	const soter_container_hdr_t *hdr = key;
	EVP_PKEY *pkey;

	if ((!asym_ka_ctx) || (!key))
	{
		return SOTER_INVALID_PARAMETER;
	}

	if (key_length < sizeof(soter_container_hdr_t))
	{
		return SOTER_INVALID_PARAMETER;
	}

	pkey = EVP_PKEY_CTX_get0_pkey(asym_ka_ctx->pkey_ctx);

	if (!pkey)
	{
		return SOTER_INVALID_PARAMETER;
	}

	if (EVP_PKEY_EC != EVP_PKEY_id(pkey))
	{
		return SOTER_INVALID_PARAMETER;
	}

	switch (hdr->tag[0])
	{
	case 'R':
		return soter_ec_priv_key_to_engine_specific(hdr, key_length, ((soter_engine_specific_ec_key_t **)&pkey));
	case 'U':
		return soter_ec_pub_key_to_engine_specific(hdr, key_length, ((soter_engine_specific_ec_key_t **)&pkey));
	default:
		return SOTER_INVALID_PARAMETER;
	}
}

soter_status_t soter_asym_ka_export_key(soter_asym_ka_t* asym_ka_ctx, void* key, size_t* key_length, bool isprivate)
{
	EVP_PKEY *pkey;

	if (!asym_ka_ctx)
	{
		return SOTER_INVALID_PARAMETER;
	}

	pkey = EVP_PKEY_CTX_get0_pkey(asym_ka_ctx->pkey_ctx);

	if (!pkey)
	{
		return SOTER_INVALID_PARAMETER;
	}

	if (EVP_PKEY_EC != EVP_PKEY_id(pkey))
	{
		return SOTER_INVALID_PARAMETER;
	}

	if (isprivate)
	{
		return soter_engine_specific_to_ec_priv_key((const soter_engine_specific_ec_key_t *)pkey, (soter_container_hdr_t *)key, key_length);
	}
	else
	{
		return soter_engine_specific_to_ec_pub_key((const soter_engine_specific_ec_key_t *)pkey, (soter_container_hdr_t *)key, key_length);
	}
}

soter_status_t soter_asym_ka_derive(soter_asym_ka_t* asym_ka_ctx, const void* peer_key, size_t peer_key_length, void *shared_secret, size_t* shared_secret_length)
{
	EVP_PKEY *peer_pkey = EVP_PKEY_new();
	soter_status_t res;
	size_t out_length;

	if (NULL == peer_pkey)
	{
		return SOTER_NO_MEMORY;
	}

	if ((!asym_ka_ctx) || (!shared_secret_length))
	{
		EVP_PKEY_free(peer_pkey);
		return SOTER_INVALID_PARAMETER;
	}

	res = soter_ec_pub_key_to_engine_specific((const soter_container_hdr_t *)peer_key, peer_key_length, ((soter_engine_specific_ec_key_t **)&peer_pkey));
	if (SOTER_SUCCESS != res)
	{
		EVP_PKEY_free(peer_pkey);
		return res;
	}

	if (1 != EVP_PKEY_derive_init(asym_ka_ctx->pkey_ctx))
	{
		EVP_PKEY_free(peer_pkey);
		return SOTER_FAIL;
	}

	if (1 != EVP_PKEY_derive_set_peer(asym_ka_ctx->pkey_ctx, peer_pkey))
	{
		EVP_PKEY_free(peer_pkey);
		return SOTER_FAIL;
	}

	if (1 != EVP_PKEY_derive(asym_ka_ctx->pkey_ctx, NULL, &out_length))
	{
		EVP_PKEY_free(peer_pkey);
		return SOTER_FAIL;
	}

	if (out_length > *shared_secret_length)
	{
		EVP_PKEY_free(peer_pkey);
		*shared_secret_length = out_length;
		return SOTER_BUFFER_TOO_SMALL;
	}

	if (1 != EVP_PKEY_derive(asym_ka_ctx->pkey_ctx, (unsigned char *)shared_secret, shared_secret_length))
	{
		EVP_PKEY_free(peer_pkey);
		return SOTER_FAIL;
	}

	EVP_PKEY_free(peer_pkey);
	return SOTER_SUCCESS;
}
