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

#include <assert.h>

#include <soter/error.h>
#include "soter_engine.h"
#include "soter_ec_key_pair_gen.h"

#include <openssl/evp.h>
#include <openssl/ecdsa.h>

#include <string.h>

static unsigned ec_curve_nid(const int size){
    switch (size){
    case 1://256:
	return NID_X9_62_prime256v1;
    case 2://384:
	return NID_secp384r1;
    case 3://521:
	return NID_secp521r1;
    default:
	return 0;
    }
    return 0;
}

soter_ec_key_pair_gen_t* soter_ec_key_pair_gen_create(const unsigned key_length){
    assert(ec_curve_nid(key_length)>0);
    soter_ec_key_pair_gen_t *ctx = malloc(sizeof(soter_ec_key_pair_gen_t));
    assert(ctx);
    if(SOTER_SUCCESS != soter_ec_key_pair_gen_init(ctx, key_length)){
      soter_ec_key_pair_gen_destroy(ctx);
      return NULL;
    }
    return ctx;
}
soter_status_t soter_ec_key_pair_gen_init(soter_ec_key_pair_gen_t* ctx, const unsigned key_length){
    EVP_PKEY *pkey;
    pkey = EVP_PKEY_new();
    assert(pkey);
    assert(EVP_PKEY_set_type(pkey, EVP_PKEY_EC));
    ctx->pkey_ctx = EVP_PKEY_CTX_new(pkey, NULL);
    assert(ctx->pkey_ctx);
    assert(EVP_PKEY_paramgen_init(ctx->pkey_ctx));
    assert(EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx->pkey_ctx, ec_curve_nid(key_length)));
    assert(EVP_PKEY_paramgen(ctx->pkey_ctx, &pkey));
    EC_KEY *ec = EVP_PKEY_get0(pkey);
    assert(ec);
    assert(EC_KEY_generate_key(ec));
    return SOTER_SUCCESS;
}

soter_status_t soter_ec_key_pair_gen_cleanup(soter_ec_key_pair_gen_t* ctx){
    if(!ctx){
	return SOTER_INVALID_PARAMETER;
    }
    if (ctx->pkey_ctx)
    {
        EVP_PKEY* pkey = EVP_PKEY_CTX_get0_pkey(ctx->pkey_ctx);
	EVP_PKEY_CTX_free(ctx->pkey_ctx);
	if(pkey){
	    EVP_PKEY_free(pkey);
	}
    }
    return SOTER_SUCCESS;
}

soter_status_t soter_ec_key_pair_gen_destroy(soter_ec_key_pair_gen_t* ctx){
    if(!ctx){
	return SOTER_INVALID_PARAMETER;
    }
    if(SOTER_SUCCESS != soter_ec_key_pair_gen_cleanup(ctx)){
	return SOTER_FAIL;
    }
    free(ctx);
    return SOTER_SUCCESS;
}

soter_status_t soter_ec_key_pair_gen_export_key(soter_ec_key_pair_gen_t* ctx, void* key, size_t* key_length, bool isprivate){
    if(!ctx){
	return SOTER_INVALID_PARAMETER;
    }
    EVP_PKEY *pkey;
    pkey = EVP_PKEY_CTX_get0_pkey(ctx->pkey_ctx);
    SOTER_CHECK_PARAM(pkey);
    SOTER_CHECK_PARAM(EVP_PKEY_EC == EVP_PKEY_id(pkey));
    if (isprivate)
    {
	return soter_engine_specific_to_ec_priv_key((const soter_engine_specific_ec_key_t *)pkey, (soter_container_hdr_t *)key, key_length);
    }
    else
    {
	return soter_engine_specific_to_ec_pub_key((const soter_engine_specific_ec_key_t *)pkey, (soter_container_hdr_t *)key, key_length);
    }
}
