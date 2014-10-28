/**
 * @f
 *
 * (c) CossackLabs
 */

#ifndef SOTER_SIGN_RSA_H
#define SOTER_SIGN_RSA_H

#include "soter/soter.h"

soter_status_t soter_sign_init_rsa_pss_pkcs8(soter_sign_ctx_t* ctx, const void* private_key, const size_t private_key_length, const void* public_key, const size_t public_key_length);
soter_status_t soter_sign_update_rsa_pss_pkcs8(soter_sign_ctx_t* ctx, const void* data, const size_t data_length);
soter_status_t soter_sign_final_rsa_pss_pkcs8(soter_sign_ctx_t* ctx, void* signature, size_t* signature_length);

soter_status_t soter_verify_init_rsa_pss_pkcs8(soter_sign_ctx_t* ctx, const void* private_key, const size_t private_key_length, const void* public_key, const size_t public_key_length);
soter_status_t soter_verify_update_rsa_pss_pkcs8(soter_sign_ctx_t* ctx, const void* data, const size_t data_length);
soter_status_t soter_verify_final_rsa_pss_pkcs8(soter_sign_ctx_t* ctx, void* signature, size_t signature_length);

#endif /*SOTER_SIGN_RSA_H*/
