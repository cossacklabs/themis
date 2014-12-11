/**
 * @file
 *
 * (c) CossackLabs
 */

#include <string.h>
#include "themis/message.h"

themis_message_t* themis_message_init(){
  themis_message_t* msg=malloc(sizeof(themis_message_t));
  if(!msg){
    return NULL;
  }
  msg->length=0;
  msg->data=NULL;
  return msg;
}
themis_status_t themis_message_set(themis_message_t* ctx, const uint8_t* message, const size_t message_length){
  HERMES_CHECK(ctx);
  HERMES_CHECK(message);
  HERMES_CHECK(message_length!=0);
  if(ctx->length<message_length){
    if(!ctx){
	ctx->data=malloc(message_length);
    } else {
	ctx->data=realloc(ctx->data, message_length);
    }
    if(!(ctx->data)){
      ctx->length=0;
      return HERMES_FAIL;
    }
    ctx->length=message_length;
  }
  memcpy(ctx->data, message, message_length);
  return HERMES_SUCCESS;
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
  HERMES_CHECK(ctx);
  if(ctx->data!=NULL){
    free(ctx->data);
  }
  free(ctx);
  return HERMES_SUCCESS;
}












