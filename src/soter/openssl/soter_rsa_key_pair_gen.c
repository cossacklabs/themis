/*
* Copyright (c) 2015 Cossack Labs Limited
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include <soter/error.h>
#include "soter_openssl.h"
#include <soter/soter_rsa_key_pair_gen.h>

#include <openssl/evp.h>
#include <openssl/rsa.h>

#include <string.h>

static unsigned rsa_key_length(const int size){
    switch (size){
    case RSA_KEY_LENGTH_1024:
	return 1024;
    case RSA_KEY_LENGTH_2048:
	return 2048;
    case RSA_KEY_LENGTH_4096:
	return 4096;
    case RSA_KEY_LENGTH_8192:
	return 8192;
    default:
	return 0;
    }
    return 0;
}

soter_rsa_key_pair_gen_t* soter_rsa_key_pair_gen_create(const unsigned key_length){
    SOTER_CHECK_PARAM_(rsa_key_length(key_length)>0);
    soter_rsa_key_pair_gen_t *ctx = malloc(sizeof(soter_rsa_key_pair_gen_t));
    SOTER_CHECK_MALLOC_(ctx);
    SOTER_IF_FAIL_(soter_rsa_key_pair_gen_init(ctx, key_length)==SOTER_SUCCESS, free(ctx));
    return ctx;
}
soter_status_t soter_rsa_key_pair_gen_init(soter_rsa_key_pair_gen_t* ctx, const unsigned key_length){
    EVP_PKEY *pkey;
    pkey = EVP_PKEY_new();
    SOTER_CHECK(pkey);
    /* Only RSA supports asymmetric encryption */
    SOTER_IF_FAIL(EVP_PKEY_set_type(pkey, EVP_PKEY_RSA), EVP_PKEY_free(pkey));
    ctx->pkey_ctx = EVP_PKEY_CTX_new(pkey, NULL);
    SOTER_IF_FAIL(ctx->pkey_ctx,EVP_PKEY_free(pkey));
    BIGNUM *pub_exp;
    SOTER_IF_FAIL(EVP_PKEY_keygen_init(ctx->pkey_ctx), EVP_PKEY_CTX_free(ctx->pkey_ctx));

    /* Although it seems that OpenSSL/LibreSSL use 0x10001 as default public exponent, we will set it explicitly just in case */
    pub_exp = BN_new();
    SOTER_CHECK(pub_exp);
    SOTER_IF_FAIL(BN_set_word(pub_exp, RSA_F4), (BN_free(pub_exp), EVP_PKEY_CTX_free(ctx->pkey_ctx)));

    SOTER_IF_FAIL(1 <= EVP_PKEY_CTX_ctrl(ctx->pkey_ctx, -1, -1, EVP_PKEY_CTRL_RSA_KEYGEN_PUBEXP, 0, pub_exp), (BN_free(pub_exp), EVP_PKEY_CTX_free(ctx->pkey_ctx)));
    /* Override default key size for RSA key. Currently OpenSSL has default key size of 1024. LibreSSL has 2048. We will put 2048 explicitly */
    SOTER_IF_FAIL((1 <= EVP_PKEY_CTX_ctrl(ctx->pkey_ctx, -1, -1, EVP_PKEY_CTRL_RSA_KEYGEN_BITS, rsa_key_length(key_length), NULL)), (BN_free(pub_exp), EVP_PKEY_CTX_free(ctx->pkey_ctx)));
    SOTER_IF_FAIL(EVP_PKEY_keygen(ctx->pkey_ctx, &pkey), (BN_free(pub_exp), EVP_PKEY_CTX_free(ctx->pkey_ctx)));
    return SOTER_SUCCESS;
}

soter_status_t soter_rsa_key_pair_gen_cleanup(soter_rsa_key_pair_gen_t* ctx){
    SOTER_CHECK_PARAM(ctx);
    if (ctx->pkey_ctx)
    {
	EVP_PKEY_CTX_free(ctx->pkey_ctx);
    }
    return SOTER_SUCCESS;
}

soter_status_t soter_rsa_key_pair_gen_destroy(soter_rsa_key_pair_gen_t* ctx){
    SOTER_CHECK_PARAM(ctx);
    SOTER_CHECK(soter_rsa_key_pair_gen_cleanup(ctx)==SOTER_SUCCESS);
    free(ctx);
    return SOTER_SUCCESS;
}

soter_status_t soter_rsa_key_pair_gen_export_key(soter_rsa_key_pair_gen_t* ctx, void* key, size_t* key_length, bool isprivate){
    EVP_PKEY *pkey;
    SOTER_CHECK_PARAM(ctx);
    pkey = EVP_PKEY_CTX_get0_pkey(ctx->pkey_ctx);
    SOTER_CHECK_PARAM(pkey);
    SOTER_CHECK_PARAM(EVP_PKEY_RSA == EVP_PKEY_id(pkey));
    if (isprivate)
    {
	return soter_engine_specific_to_rsa_priv_key((const soter_engine_specific_rsa_key_t *)pkey, (soter_container_hdr_t *)key, key_length);
    }
    else
    {
	return soter_engine_specific_to_rsa_pub_key((const soter_engine_specific_rsa_key_t *)pkey, (soter_container_hdr_t *)key, key_length);
    }
}
