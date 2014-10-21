/**
 * @file
 *
 * (c) CossackLabs
 */

#include <common/error.h>
#include "soter_openssl.h"
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

	if (0 == nid)
	{
		return HERMES_INVALID_PARAMETER;
	}

	pkey = EVP_PKEY_new();
	if (!pkey)
	{
		return HERMES_NO_MEMORY;
	}

	if (!EVP_PKEY_set_type(pkey, EVP_PKEY_EC))
	{
		EVP_PKEY_free(pkey);
		return HERMES_FAIL;
	}

	asym_ka_ctx->pkey_ctx = EVP_PKEY_CTX_new(pkey, NULL);
	if (!(asym_ka_ctx->pkey_ctx))
	{
		EVP_PKEY_free(pkey);
		return HERMES_FAIL;
	}

	if (1 != EVP_PKEY_CTX_set_ec_paramgen_curve_nid(asym_ka_ctx->pkey_ctx, nid))
	{
		EVP_PKEY_free(pkey);
		return HERMES_FAIL;
	}

	return HERMES_SUCCESS;
}

soter_status_t soter_asym_ka_cleanup(soter_asym_ka_t* asym_ka_ctx)
{
	if (asym_ka_ctx->pkey_ctx)
	{
		EVP_PKEY_CTX_free(asym_ka_ctx->pkey_ctx);
	}

	return HERMES_SUCCESS;
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
	if (HERMES_SUCCESS == status)
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
		return HERMES_INVALID_PARAMETER;
	}

	status = soter_asym_ka_cleanup(asym_ka_ctx);
	if (HERMES_SUCCESS == status)
	{
		free(asym_ka_ctx);
		return HERMES_SUCCESS;
	}
	else
	{
		return status;
	}
}

soter_status_t soter_asym_ka_gen_key(soter_asym_ka_t* asym_ka_ctx)
{
	EVP_PKEY *pkey = EVP_PKEY_CTX_get0_pkey(asym_ka_ctx->pkey_ctx);

	if (!pkey)
	{
		return HERMES_INVALID_PARAMETER;
	}

	if (EVP_PKEY_EC != EVP_PKEY_id(pkey))
	{
		return HERMES_INVALID_PARAMETER;
	}

	if (!EVP_PKEY_keygen_init(asym_ka_ctx->pkey_ctx))
	{
		return HERMES_INVALID_PARAMETER;
	}

	if (EVP_PKEY_keygen(asym_ka_ctx->pkey_ctx, &pkey))
	{
		return HERMES_SUCCESS;
	}
	else
	{
		return HERMES_FAIL;
	}
}
