/**
 * @file
 *
 * (c) CossackLabs
 */

#include <common/error.h>
#include <soter/soter.h>
#include <soter/soter_ec_key.h>
#include "soter_openssl.h"
#include <openssl/evp.h>
#include <openssl/rsa.h>

soter_status_t soter_gen_key_rsa(EVP_PKEY_CTX *pkey_ctx)
{
    return HERMES_FAIL;
}

soter_status_t soter_import_key_rsa(EVP_PKEY *pkey, const void* key, const size_t key_length)
{
  return HERMES_FAIL;
}

