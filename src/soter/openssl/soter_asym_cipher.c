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

/* Padding is ignored. We use OAEP by default. Parameter is to support more paddings in the future */
soter_status_t soter_asym_cipher_init(soter_asym_cipher_t* asym_cipher, soter_asym_cipher_padding_t pad)
{
	EVP_PKEY *pkey;

	pkey = EVP_PKEY_new();
	if (!pkey)
	{
		return HERMES_NO_MEMORY;
	}

	/* Only RSA supports asymmetric encryption */
	if (!EVP_PKEY_set_type(pkey, EVP_PKEY_RSA))
	{
		EVP_PKEY_free(pkey);
		return HERMES_FAIL;
	}

	asym_cipher->pkey_ctx = EVP_PKEY_CTX_new(pkey, NULL);
	if (!(asym_cipher->pkey_ctx))
	{
		EVP_PKEY_free(pkey);
		return HERMES_FAIL;
	}

	return HERMES_SUCCESS;
}

soter_status_t soter_asym_cipher_cleanup(soter_asym_cipher_t* asym_cipher)
{
	if (asym_cipher->pkey_ctx)
	{
		EVP_PKEY_CTX_free(asym_cipher->pkey_ctx);
	}

	return HERMES_SUCCESS;
}

soter_status_t soter_asym_cipher_gen_key(soter_asym_cipher_t* asym_cipher)
{
	BIGNUM *pub_exp;
	EVP_PKEY *pkey = EVP_PKEY_CTX_get0_pkey(asym_cipher->pkey_ctx);

	if (!pkey)
	{
		return HERMES_INVALID_PARAMETER;
	}

	if (EVP_PKEY_RSA != EVP_PKEY_id(pkey))
	{
		return HERMES_INVALID_PARAMETER;
	}

	if (!EVP_PKEY_keygen_init(asym_cipher->pkey_ctx))
	{
		return HERMES_INVALID_PARAMETER;
	}

	/* Although it seems that OpenSSL/LibreSSL use 0x10001 as default public exponent, we will set it explicitly just in case */
	pub_exp = BN_new();
	if (!pub_exp)
	{
		return HERMES_NO_MEMORY;
	}

	if (!BN_set_word(pub_exp, RSA_F4))
	{
		BN_free(pub_exp);
		return HERMES_FAIL;
	}

	if (1 > EVP_PKEY_CTX_ctrl(asym_cipher->pkey_ctx, -1, -1, EVP_PKEY_CTRL_RSA_KEYGEN_PUBEXP, 0, pub_exp))
	{
		BN_free(pub_exp);
		return HERMES_FAIL;
	}

	/* Override default key size for RSA key. Currently OpenSSL has default key size of 1024. LibreSSL has 2048. We will put 2048 explicitly */
	if (1 > EVP_PKEY_CTX_ctrl(asym_cipher->pkey_ctx, -1, -1, EVP_PKEY_CTRL_RSA_KEYGEN_BITS, 2048, NULL))
	{
		return HERMES_FAIL;
	}

	if (EVP_PKEY_keygen(asym_cipher->pkey_ctx, &pkey))
	{
		return HERMES_SUCCESS;
	}
	else
	{
		return HERMES_FAIL;
	}
}

soter_status_t soter_asym_cipher_encrypt(soter_asym_cipher_t* asym_cipher, const void* plain_data, size_t plain_data_length, void* cipher_data, size_t* cipher_data_length)
{
	EVP_PKEY *pkey = EVP_PKEY_CTX_get0_pkey(asym_cipher->pkey_ctx);

	if (!pkey)
	{
		return HERMES_INVALID_PARAMETER;
	}

	if (EVP_PKEY_RSA != EVP_PKEY_id(pkey))
	{
		/* We can only do assymetric encryption with RSA algorithm */
		return HERMES_INVALID_PARAMETER;
	}

	/* Currently we support only OAEP padding for RSA encryption */
	/* TODO: should we support "no padding" or PKCS1.5 padding? */
	if (!EVP_PKEY_encrypt_init(asym_cipher->pkey_ctx))
	{
		return HERMES_FAIL;
	}

	/* TODO: This function automatically sets SHA1 as MGF digest for OAEP padding. Should we change it to SHA256? */
	if (1 > EVP_PKEY_CTX_ctrl(asym_cipher->pkey_ctx, -1, -1, EVP_PKEY_CTRL_RSA_PADDING, RSA_PKCS1_OAEP_PADDING, NULL))
	{
		return HERMES_FAIL;
	}

	if (EVP_PKEY_encrypt(asym_cipher->pkey_ctx, (unsigned char *)cipher_data, cipher_data_length, (const unsigned char *)plain_data, plain_data_length))
	{
		return HERMES_SUCCESS;
	}
	else
	{
		return HERMES_FAIL;
	}
}

soter_status_t soter_asym_cipher_decrypt(soter_asym_cipher_t* asym_cipher, const void* cipher_data, size_t cipher_data_length, void* plain_data, size_t* plain_data_length)
{
	EVP_PKEY *pkey = EVP_PKEY_CTX_get0_pkey(asym_cipher->pkey_ctx);

	if (!pkey)
	{
		return HERMES_INVALID_PARAMETER;
	}

	if (EVP_PKEY_RSA != EVP_PKEY_id(pkey))
	{
		/* We can only do assymetric encryption with RSA algorithm */
		return HERMES_INVALID_PARAMETER;
	}

	/* Currently we support only OAEP padding for RSA encryption */
	/* TODO: should we support "no padding" or PKCS1.5 padding? */
	if (!EVP_PKEY_decrypt_init(asym_cipher->pkey_ctx))
	{
		return HERMES_FAIL;
	}

	/* TODO: This function automatically sets SHA1 as MGF digest for OAEP padding. Should we change it to SHA256? */
	if (1 > EVP_PKEY_CTX_ctrl(asym_cipher->pkey_ctx, -1, -1, EVP_PKEY_CTRL_RSA_PADDING, RSA_PKCS1_OAEP_PADDING, NULL))
	{
		return HERMES_FAIL;
	}

	if (EVP_PKEY_decrypt(asym_cipher->pkey_ctx, (unsigned char *)plain_data, plain_data_length, (const unsigned char *)cipher_data, cipher_data_length))
	{
		return HERMES_SUCCESS;
	}
	else
	{
		return HERMES_FAIL;
	}
}

soter_asym_cipher_t* soter_asym_cipher_create(soter_asym_cipher_padding_t pad)
{
	soter_status_t status;
	soter_asym_cipher_t *ctx = malloc(sizeof(soter_asym_cipher_t));
	if (!ctx)
	{
		return NULL;
	}

	status = soter_asym_cipher_init(ctx, pad);
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

soter_status_t soter_asym_cipher_destroy(soter_asym_cipher_t* asym_cipher)
{
	soter_status_t status;

	if (!asym_cipher)
	{
		return HERMES_INVALID_PARAMETER;
	}

	status = soter_asym_cipher_cleanup(asym_cipher);
	if (HERMES_SUCCESS == status)
	{
		free(asym_cipher);
		return HERMES_SUCCESS;
	}
	else
	{
		return status;
	}
}
