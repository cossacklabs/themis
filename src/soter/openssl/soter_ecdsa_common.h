/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef SOTER_RSA_COMMON_H
#define SOTER_RSA_COMMON_H

#include <common/error.h>
#include <soter/soter.h>
#include <soter/soter_ec_key.h>
#include "soter_openssl.h"

soter_status_t soter_ec_gen_key(EVP_PKEY_CTX *pkey_ctx);
soter_status_t soter_ec_import_key(EVP_PKEY *pkey, const void* key, const size_t key_length);
soter_status_t soter_ec_export_key(soter_sign_ctx_t* ctx, void* key, size_t* key_length, bool isprivate);

#endif

