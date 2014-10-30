/**
 * @file
 *
 * (c) CossackLabs
 */

#include <common/error.h>
#include <soter/soter.h>
#include <soter/soter_ec_key.h>
#include "soter_openssl.h"
#include "soter_ecdsa_common.h"
#include <openssl/evp.h>
#include <openssl/rsa.h>


soter_status_t soter_sign_init_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx, const void* private_key, const size_t private_key_length, const void* public_key, const size_t public_key_length)
{
  return HERMES_FAIL;
}

soter_status_t soter_sign_export_key_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx, void* key, size_t* key_length, bool isprivate)
{
  return HERMES_FAIL;
}

soter_status_t soter_sign_update_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx, const void* data, const size_t data_length)
{
  return HERMES_FAIL;
}

soter_status_t soter_sign_final_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx, void* signature, size_t *signature_length)
{
  return HERMES_FAIL;
}
