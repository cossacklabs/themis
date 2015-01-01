/**
 * @f
 *
 * (c) CossackLabs
 */

#ifndef SOTER_SIGN_ECDSA_H
#define SOTER_SIGN_ECDSA_H

#include "soter/soter.h"

soter_status_t soter_sign_init_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx, const void* private_key, const size_t private_key_length, const void* public_key, const size_t public_key_length);
soter_status_t soter_sign_update_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx, const void* data, const size_t data_length);
soter_status_t soter_sign_final_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx, void* signature, size_t* signature_length);
soter_status_t soter_sign_export_key_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx, void* key, size_t* key_length, bool isprivate);

soter_status_t soter_verify_init_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx, const void* private_key, const size_t private_key_length, const void* public_key, const size_t public_key_length);
soter_status_t soter_verify_update_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx, const void* data, const size_t data_length);
soter_status_t soter_verify_final_ecdsa_none_pkcs8(soter_sign_ctx_t* ctx, const void* signature, const size_t signature_length);

#endif /*SOTER_SIGN_ECDSA_H*/
