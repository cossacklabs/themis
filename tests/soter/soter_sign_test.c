/**
 * @file
 *
 * (c) CossackLabs
 */
#include <string.h>
#include "soter_test.h"


static int sign_test(soter_sign_alg_t alg)
{
  uint8_t test_data[]="test message";
  size_t test_data_length=strlen(test_data);
  
  soter_sign_ctx_t* ctx=NULL;

  uint8_t* signature=NULL;
  size_t signature_length=0;

  ctx=soter_sign_create(alg,NULL,0,NULL,0);
  if(!ctx){
    testsuite_fail_if(!ctx, "soter_sign_ctx_t == NULL");
    return -1;
  }

  soter_status_t res;
  res=soter_sign_update(ctx, test_data, test_data_length);
  if(res!=HERMES_SUCCESS){
    testsuite_fail_if(res!=HERMES_SUCCESS, "soter_sign_update fail");
    soter_sign_destroy(ctx);
    return -2;
  }

  res=soter_sign_final(ctx, signature, &signature_length);
  if(res!=HERMES_BUFFER_TOO_SMALL){
    testsuite_fail_if(res!=HERMES_BUFFER_TOO_SMALL, "soter_sign_final (signature length determine) fail");
    soter_sign_destroy(ctx);
    return -3;
  }

  signature=malloc(signature_length);
  if(!signature){
    testsuite_fail_if(!signature, "out of memory");
    soter_sign_destroy(ctx);
    return -4;
  }

  res=soter_sign_final(ctx, signature, &signature_length);
  if(res!=HERMES_SUCCESS){
    testsuite_fail_if(res!=HERMES_SUCCESS, "soter_sign_final fail");
    soter_sign_destroy(ctx);
    return -5;
  }


  uint8_t private_key[8192];
  size_t private_key_length=sizeof(private_key);

  uint8_t public_key[8192];
  size_t public_key_length=sizeof(public_key);

  res=soter_sign_export_key(ctx, private_key, &private_key_length, true);
  if(res!=HERMES_SUCCESS){
    testsuite_fail_if(res!=HERMES_SUCCESS,"soter_sign_export_key (private key) fail");
    soter_sign_destroy(ctx);
    return -6;
  }

  res=soter_sign_export_key(ctx, public_key, &public_key_length, false);
  if(res!=HERMES_SUCCESS){
    testsuite_fail_if(res!=HERMES_SUCCESS,"soter_sign_export_key (public key) fail");
    soter_sign_destroy(ctx);
    return -7;
  }

  res=soter_sign_destroy(ctx);
  if(res!=HERMES_SUCCESS){
    testsuite_fail_if(res!=HERMES_SUCCESS,"soter_sign_destroy fail");
    return -8;
  }
  
  soter_verify_ctx_t* vctx=NULL;

  vctx=soter_verify_create(alg, private_key, private_key_length, public_key, public_key_length);
  if(!vctx){
    testsuite_fail_if(!vctx, "soter_verify_ctx_t == NULL");
    return -9;
  }

  res=soter_verify_update(ctx, test_data, test_data_length);
  if(res!=HERMES_SUCCESS){
    testsuite_fail_if(res!=HERMES_SUCCESS, "soter_verify_update fail");
    soter_verify_destroy(ctx);
    return -10;
  }

  res=soter_verify_final(ctx, signature, signature_length);
  if(res!=HERMES_SUCCESS){
    testsuite_fail_if(res!=HERMES_SUCCESS, "soter_verify_final fail");
    soter_verify_destroy(ctx);
    return -11;
  }
  return 0;
}

static void soter_sign_test()
{
  testsuite_fail_if(sign_test(SOTER_SIGN_rsa_pss_pkcs8),"soter sign SOTER_SIGN_rsa_pss_pkcs8");
  testsuite_fail_if(sign_test(SOTER_SIGN_ecdsa_none_pkcs8),"soter sign SOTER_SIGN_ecdsa_none_pkcs8");
}

void run_soter_sign_test(){
  testsuite_enter_suite("soter sign");
  testsuite_run_test(soter_sign_test);
}
