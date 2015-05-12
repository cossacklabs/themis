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

#include <string.h>
#include <themis/error.h>
#include <themis/message.h>

themis_message_t* themis_message_init(const uint8_t* message, const size_t message_length){
  themis_message_t* msg=malloc(sizeof(themis_message_t));
  if(!msg){
    return NULL;
  }
  msg->length=0;
  msg->data=NULL;
  if(message!=NULL && message_length!=0){
    if(themis_message_set(msg, message, message_length)==THEMIS_FAIL){
      themis_message_destroy(msg);
      return NULL;
    }
  }
  return msg;
}

themis_status_t themis_message_set(themis_message_t* ctx, const uint8_t* message, const size_t message_length){
  THEMIS_CHECK(ctx);
  THEMIS_CHECK(message);
  THEMIS_CHECK(message_length!=0);
  if(ctx->length<message_length){
    if(!ctx){
	ctx->data=malloc(message_length);
    } else {
	ctx->data=realloc(ctx->data, message_length);
    }
    if(!(ctx->data)){
      ctx->length=0;
      return THEMIS_FAIL;
    }
    ctx->length=message_length;
  }
  memcpy(ctx->data, message, message_length);
  return THEMIS_SUCCESS;
}

const uint8_t* themis_message_get_data(themis_message_t* ctx)
{
  if(!ctx){
    return NULL;
  }
  return ctx->data;
}

size_t themis_message_get_length(themis_message_t* ctx){
  if(!ctx){
    return 0;
  }
  return ctx->length;
}

themis_status_t themis_message_destroy(themis_message_t* ctx){
  THEMIS_CHECK(ctx);
  if(ctx->data!=NULL){
    free(ctx->data);
  }
  free(ctx);
  ctx=NULL;
  return THEMIS_SUCCESS;
}











