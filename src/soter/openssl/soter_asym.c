/**
 * @file
 *
 * (c) CossackLabs
 */

#include "common/error.h"
#include "soter/soter.h"
#include "soter_openssl.h"
#include <openssl/evp.h>
#include <openssl/rsa.h>

static int soter_algo_to_nid(soter_asym_algo_t algo)
{
	switch (algo)
	{
	case SOTER_ASYM_RSA:
		return EVP_PKEY_RSA;
	case SOTER_ASYM_EC:
		return EVP_PKEY_EC;
	default:
		return 0;
	}
}

soter_status_t soter_asym_init(soter_asym_ctx_t* asym_ctx, soter_asym_algo_t algo, soter_asym_padding_t pad)
{
	EVP_PKEY *pkey;
	int nid = soter_algo_to_nid(algo);

	if (!nid)
	{
		return HERMES_INVALID_PARAMETER;
	}

	asym_ctx->pad = pad;

	pkey = EVP_PKEY_new();
	if (!pkey)
	{
		return HERMES_NO_MEMORY;
	}

	if (!EVP_PKEY_set_type(pkey, nid))
	{
		EVP_PKEY_free(pkey);
		return HERMES_FAIL;
	}

	asym_ctx->pkey_ctx = EVP_PKEY_CTX_new(pkey, NULL);
	if (!(asym_ctx->pkey_ctx))
	{
		EVP_PKEY_free(pkey);
		return HERMES_FAIL;
	}

	return HERMES_SUCCESS;
}

soter_status_t soter_asym_cleanup(soter_asym_ctx_t* asym_ctx)
{
	if (asym_ctx->pkey_ctx)
	{
		EVP_PKEY_CTX_free(asym_ctx->pkey_ctx);
	}

	return HERMES_SUCCESS;
}

soter_status_t soter_asym_gen_key(soter_asym_ctx_t* asym_ctx)
{
	EVP_PKEY *pkey = EVP_PKEY_CTX_get0_pkey(asym_ctx->pkey_ctx);

	if (!pkey)
	{
		return HERMES_INVALID_PARAMETER;
	}

	if (!EVP_PKEY_keygen_init(asym_ctx->pkey_ctx))
	{
		return HERMES_INVALID_PARAMETER;
	}

	if (EVP_PKEY_RSA == EVP_PKEY_id(pkey))
	{
		/* Although it seems that OpenSSL/LibreSSL use 0x10001 as default public exponent, we will set it explicitly just in case */
		BIGNUM *pub_exp = BN_new();
		if (!pub_exp)
		{
			return HERMES_NO_MEMORY;
		}

		if (!BN_set_word(pub_exp, RSA_F4))
		{
			BN_free(pub_exp);
			return HERMES_FAIL;
		}

		if (1 > EVP_PKEY_CTX_ctrl(asym_ctx->pkey_ctx, -1, -1, EVP_PKEY_CTRL_RSA_KEYGEN_PUBEXP, 0, pub_exp))
		{
			BN_free(pub_exp);
			return HERMES_FAIL;
		}

		/* Override default key size for RSA key. Currently OpenSSL has default key size of 1024. LibreSSL has 2048. We will put 2048 explicitly */
		if (1 > EVP_PKEY_CTX_ctrl(asym_ctx->pkey_ctx, -1, -1, EVP_PKEY_CTRL_RSA_KEYGEN_BITS, 2048, NULL))
		{
			return HERMES_FAIL;
		}
	}

	if (EVP_PKEY_keygen(asym_ctx->pkey_ctx, &pkey))
	{
		return HERMES_SUCCESS;
	}
	else
	{
		return HERMES_FAIL;
	}
}

soter_status_t soter_asym_encrypt(soter_asym_ctx_t* asym_ctx, const void* plain_data, size_t plain_data_length, void* cipher_data, size_t* cipher_data_length)
{
	EVP_PKEY *pkey = EVP_PKEY_CTX_get0_pkey(asym_ctx->pkey_ctx);

	if (!pkey)
	{
		return HERMES_INVALID_PARAMETER;
	}

	if (EVP_PKEY_RSA != EVP_PKEY_id(pkey))
	{
		/* We can only do assymetric encryption with RSA algorithm */
		return HERMES_INVALID_PARAMETER;
	}

	if (SOTER_ASYM_OAEP != asym_ctx->pad)
	{
		/* Currently we support only OAEP padding for RSA encryption */
		/* TODO: should we support "no padding" or PKCS1.5 padding? */
		return HERMES_INVALID_PARAMETER;
	}

	if (!EVP_PKEY_encrypt_init(asym_ctx->pkey_ctx))
	{
		return HERMES_FAIL;
	}

	/* TODO: This function automatically sets SHA1 as MGF digest for OAEP padding. Should we change it to SHA256? */
	if (1 > EVP_PKEY_CTX_ctrl(asym_ctx->pkey_ctx, -1, -1, EVP_PKEY_CTRL_RSA_PADDING, RSA_PKCS1_OAEP_PADDING, NULL))
	{
		return HERMES_FAIL;
	}

	if (EVP_PKEY_encrypt(asym_ctx->pkey_ctx, (unsigned char *)cipher_data, cipher_data_length, (const unsigned char *)plain_data, plain_data_length))
	{
		return HERMES_SUCCESS;
	}
	else
	{
		return HERMES_FAIL;
	}
}

soter_status_t soter_asym_decrypt(soter_asym_ctx_t* asym_ctx, const void* cipher_data, size_t cipher_data_length, void* plain_data, size_t* plain_data_length)
{
	EVP_PKEY *pkey = EVP_PKEY_CTX_get0_pkey(asym_ctx->pkey_ctx);

	if (!pkey)
	{
		return HERMES_INVALID_PARAMETER;
	}

	if (EVP_PKEY_RSA != EVP_PKEY_id(pkey))
	{
		/* We can only do assymetric encryption with RSA algorithm */
		return HERMES_INVALID_PARAMETER;
	}

	if (SOTER_ASYM_OAEP != asym_ctx->pad)
	{
		/* Currently we support only OAEP padding for RSA encryption */
		/* TODO: should we support "no padding" or PKCS1.5 padding? */
		return HERMES_INVALID_PARAMETER;
	}

	if (!EVP_PKEY_decrypt_init(asym_ctx->pkey_ctx))
	{
		return HERMES_FAIL;
	}

	/* TODO: This function automatically sets SHA1 as MGF digest for OAEP padding. Should we change it to SHA256? */
	if (1 > EVP_PKEY_CTX_ctrl(asym_ctx->pkey_ctx, -1, -1, EVP_PKEY_CTRL_RSA_PADDING, RSA_PKCS1_OAEP_PADDING, NULL))
	{
		return HERMES_FAIL;
	}

	if (EVP_PKEY_decrypt(asym_ctx->pkey_ctx, (unsigned char *)plain_data, plain_data_length, (const unsigned char *)cipher_data, cipher_data_length))
	{
		return HERMES_SUCCESS;
	}
	else
	{
		return HERMES_FAIL;
	}
}

soter_asym_ctx_t* soter_asym_create(soter_asym_algo_t algo, soter_asym_padding_t pad)
{
	soter_status_t status;
	soter_asym_ctx_t *ctx = malloc(sizeof(soter_asym_ctx_t));
	if (!ctx)
	{
		return NULL;
	}

	status = soter_asym_init(ctx, algo, pad);
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

soter_status_t soter_asym_destroy(soter_asym_ctx_t* asym_ctx)
{
	soter_status_t status;

	if (!asym_ctx)
	{
		return HERMES_INVALID_PARAMETER;
	}

	status = soter_asym_cleanup(asym_ctx);
	if (HERMES_SUCCESS == status)
	{
		free(asym_ctx);
		return HERMES_SUCCESS;
	}
	else
	{
		return status;
	}
}
