/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef SOTER_OPENSSL_H
#define SOTER_OPENSSL_H

#include "soter/soter.h"
#include <openssl/evp.h>

struct soter_hash_ctx_type
{
  EVP_MD_CTX evp_md_ctx;
};

struct soter_sym_ctx_type
{
  EVP_CIPHER_CTX evp_sym_ctx;
  soter_sym_alg_t algId;
};

struct soter_asym_ctx_type
{
	EVP_PKEY_CTX *pkey_ctx;
	soter_asym_padding_t pad;
};

struct soter_asym_cipher_type
{
	EVP_PKEY_CTX *pkey_ctx;
};

#endif /* SOTER_OPENSSL_H */
