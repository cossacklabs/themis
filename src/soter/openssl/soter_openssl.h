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
  EVP_CHIPHER_CTX evp_sym_ctx;
};
#endif /* SOTER_OPENSSL_H */
