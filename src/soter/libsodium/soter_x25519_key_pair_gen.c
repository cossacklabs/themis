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
#include "soter_x25519_key.h"
#include "soter_x25519_key_pair_gen.h"

#include <sodium.h>

#include <string.h>

struct soter_x25519_key_pair_gen_type{
    soter_x25519_priv_key_t priv;
    soter_x25519_pub_key_t pub;
};

soter_status_t soter_x25519_key_pair_gen_init(soter_x25519_key_pair_gen_t* ctx){
    if(!ctx){
	return SOTER_INVALID_PARAMETER;
    }
    if(0!=crypto_box_keypair(ctx->pub.key, ctx->priv.key)){
	return SOTER_FAIL;
    }
    memcpy(ctx->priv.hdr.tag, X25519_PRIV_KEY_TAG, SOTER_CONTAINER_TAG_LENGTH);
    ctx->priv.hdr.size=htonl(sizeof(soter_x25519_priv_key_t));
    soter_update_container_checksum(&(ctx->priv.hdr));
    memcpy(ctx->pub.hdr.tag, X25519_PUB_KEY_TAG, SOTER_CONTAINER_TAG_LENGTH);
    ctx->pub.hdr.size=htonl(sizeof(soter_x25519_pub_key_t));
    soter_update_container_checksum((soter_container_hdr_t*)&(ctx->pub));
    return SOTER_SUCCESS;
}

soter_x25519_key_pair_gen_t* soter_x25519_key_pair_gen_create(){
    soter_x25519_key_pair_gen_t *ctx = malloc(sizeof(soter_x25519_key_pair_gen_t));
    assert(ctx);
    if(SOTER_SUCCESS != soter_x25519_key_pair_gen_init(ctx)){
      soter_x25519_key_pair_gen_destroy(ctx);
      return NULL;
    }
    return ctx;
}

soter_status_t soter_x25519_key_pair_gen_cleanup(soter_x25519_key_pair_gen_t* ctx){
    if(!ctx){
	return SOTER_INVALID_PARAMETER;
    }
    memset(ctx, 0, sizeof(soter_x25519_key_pair_gen_t));
    return SOTER_SUCCESS;
}

soter_status_t soter_x25519_key_pair_gen_destroy(soter_x25519_key_pair_gen_t* ctx){
    if(!ctx){
	return SOTER_INVALID_PARAMETER;
    }
    free(ctx);
    return SOTER_SUCCESS;
}

soter_status_t soter_x25519_key_pair_gen_export_key(soter_x25519_key_pair_gen_t* ctx, void* key, size_t* key_length, bool isprivate){
    if(!ctx){
	return SOTER_INVALID_PARAMETER;
    }
    if (isprivate)
    {
      if(!key || (*key_length)<(sizeof(soter_x25519_priv_key_t))){
        *key_length=sizeof(soter_x25519_priv_key_t);
        return SOTER_BUFFER_TOO_SMALL;
      }
      memcpy(key, &(ctx->priv), sizeof(soter_x25519_priv_key_t));
      *key_length=sizeof(soter_x25519_priv_key_t);
      return SOTER_SUCCESS;
    }
    if(!key || (*key_length)<(sizeof(soter_x25519_pub_key_t))){
        *key_length=sizeof(soter_x25519_pub_key_t);
        return SOTER_BUFFER_TOO_SMALL;
    }
    memcpy(key, &(ctx->pub), sizeof(soter_x25519_pub_key_t));
    *key_length=sizeof(soter_x25519_pub_key_t);
    return SOTER_SUCCESS;
}
