/**
 * @f
 *
 * (c) CossackLabs
 */

#ifndef SOTER_SIGN_RSA_H
#define SOTER_SIGN_RSA_H

#include "soter/soter.h"

soter_status_t soter_sign_init_rsa_pss_pkcs8(soter_sign_ctx_t* ctx, soter_sign_alg_t alg, const void* key, const size_t key_length);
soter_status_t soter_sign_gen_key_rsa_pss_pkcs8(soter_sign_ctx_t* ctx);
soter_status_t soter_sign_export_key_rsa_pss_pkcs8(soter_sign_ctx_t* ctx, void* key, size_t* key_length);
soter_status_t soter_sign_update_rsa_pss_pkcs8(soter_sign_ctx_t* ctx, const void* data, const size_t data_length);
soter_status_t soter_sign_final_rsa_pss_pkcs8(soter_sign_ctx_t* ctx, void* signature, size_t* signature_length);

#endif /*SOTER_SIGN_RSA_H*/
