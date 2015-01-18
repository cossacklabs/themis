/**
 * @file
 *
 * (c) CossackLabs
 */


#include <string.h>
#include <common/test_utils.h>
#include <themis/secure_message.h>

#define RSA_ALG 1
#define EC_ALG  2

#define test_check(function_call, success_res, msg) {			\
  themis_status_t res=function_call;					\
  if(res!=success_res){							\
    testsuite_fail_if(true, msg);					\
    return -1;								\
  }}

#define test_check_free(function_call, success_res, msg, free_condition) {		\
  themis_status_t res=function_call;					\
  if(res!=success_res){							\
    testsuite_fail_if(true, msg);					\
    free_condition;							\
    return -1;								\
  }}


static themis_status_t themis_gen_key_pair(int alg, uint8_t* private_key, size_t* private_key_length, uint8_t* public_key, size_t* public_key_length){
  themis_status_t res;
  if(alg==RSA_ALG){
    res=themis_gen_rsa_key_pair(private_key, private_key_length, public_key, public_key_length);
  }
  else if(alg==EC_ALG){
    res=themis_gen_ec_key_pair(private_key, private_key_length, public_key, public_key_length);
  }
  return res;
}

static int themis_secure_signed_message_generic_test(int alg, const char* message, const size_t message_length){
  uint8_t private_key[10240];
  size_t private_key_length=10240;
  uint8_t public_key[10240];
  size_t public_key_length=10240;

  themis_status_t res;

  res=themis_gen_key_pair(alg, private_key, &private_key_length, public_key, &public_key_length);
  
  if(res!=HERMES_SUCCESS){
    testsuite_fail_if(res!=HERMES_SUCCESS, "themis_gen_key_pair fail");
    return -1;
  }

  uint8_t* wrapped_message=NULL;
  size_t wrapped_message_length=0;

  res=themis_secure_message_wrap(private_key ,private_key_length, NULL, 0, (uint8_t*)message, message_length, NULL, &wrapped_message_length);
  if(res!=HERMES_BUFFER_TOO_SMALL){
    testsuite_fail_if(res!=HERMES_BUFFER_TOO_SMALL, "themis_secure_message_wrap (wrapped_message_length determination) fail");
    return -2;
  }

  wrapped_message=malloc(wrapped_message_length);
  if(!wrapped_message){
    testsuite_fail_if(!wrapped_message, "malloc fail");
    return -3;
  }
  res=themis_secure_message_wrap(private_key, private_key_length, NULL, 0, (uint8_t*)message, message_length, wrapped_message, &wrapped_message_length);
  if(res!=HERMES_SUCCESS){
    free(wrapped_message);
    testsuite_fail_if(res!=HERMES_SUCCESS, "themis_secure_message_wrap fail");
    return -4;
  }

  uint8_t* unwrapped_message=NULL;
  size_t unwrapped_message_length=0;
  
  res=themis_secure_message_unwrap(NULL , 0, public_key, public_key_length, wrapped_message, wrapped_message_length, NULL, &unwrapped_message_length);
  if(res!=HERMES_BUFFER_TOO_SMALL){
    free(wrapped_message);
    testsuite_fail_if(res!=HERMES_BUFFER_TOO_SMALL, "themis_secure_message_unwrap (unwrapped_message_length determination) fail");
    return -5;
  }
  unwrapped_message=malloc(unwrapped_message_length);
  if(!wrapped_message){
    free(wrapped_message);
    testsuite_fail_if(!wrapped_message, "malloc fail");
    return -3;
  }
  res=themis_secure_message_unwrap(NULL, 0, public_key, public_key_length, wrapped_message, wrapped_message_length, unwrapped_message, &unwrapped_message_length);
  if(res!=HERMES_SUCCESS){
    free(wrapped_message);
    free(unwrapped_message);
    testsuite_fail_if(res!=HERMES_SUCCESS, "themis_secure_message_unwrap fail");
    return -2;
  }

  if((message_length!=unwrapped_message_length) || (memcmp(message, unwrapped_message, message_length)!=0)){
    free(wrapped_message);
    free(unwrapped_message);
    testsuite_fail_if(true, "message not equal unwrapped_message_length");
    return -3;
  }
  free(wrapped_message);
  free(unwrapped_message);
  return 0;
}

static int themis_secure_encrypted_message_generic_test(int alg, const char* message, const size_t message_length){
  uint8_t private_key[10240];
  size_t private_key_length=10240;
  uint8_t public_key[10240];
  size_t public_key_length=10240;

  uint8_t peer_private_key[10240];
  size_t peer_private_key_length=10240;
  uint8_t peer_public_key[10240];
  size_t peer_public_key_length=10240;


  test_check(themis_gen_key_pair(alg, private_key, &private_key_length, public_key, &public_key_length), HERMES_SUCCESS, "gen key pair fail");
  test_check(themis_gen_key_pair(alg, peer_private_key, &peer_private_key_length, peer_public_key, &peer_public_key_length), HERMES_SUCCESS, "gen peer key pair fail");

  uint8_t* wrapped_message=NULL;
  size_t wrapped_message_length=0;

  test_check(themis_secure_message_wrap(private_key ,private_key_length, peer_public_key, peer_public_key_length, (uint8_t*)message, message_length, NULL, &wrapped_message_length), HERMES_BUFFER_TOO_SMALL, "themis secure message wrap (wrapped message length determination) failed");
  wrapped_message=malloc(wrapped_message_length);
  if(!wrapped_message){
    testsuite_fail_if(!wrapped_message, "malloc fail (wrapped_message)");
    return -2;
  }
  test_check_free(themis_secure_message_wrap(private_key ,private_key_length, peer_public_key, peer_public_key_length, (uint8_t*)message, message_length, wrapped_message, &wrapped_message_length), HERMES_SUCCESS, "themis secure message wrap failed", free(wrapped_message));

  uint8_t* unwrapped_message=NULL;
  size_t unwrapped_message_length=0;

  test_check_free(themis_secure_message_unwrap(peer_private_key ,private_key_length, public_key, public_key_length, (uint8_t*)wrapped_message, wrapped_message_length, NULL, &unwrapped_message_length), HERMES_BUFFER_TOO_SMALL, "themis secure message unwrap (unwrapped message length determination) failed", free(wrapped_message));
  unwrapped_message=malloc(unwrapped_message_length);
  if(!unwrapped_message){
    testsuite_fail_if(!unwrapped_message, "malloc fail (unwrapped_message)");
    return -2;
  }
  test_check_free(themis_secure_message_unwrap(peer_private_key ,peer_private_key_length, public_key, public_key_length, (uint8_t*)wrapped_message, wrapped_message_length, unwrapped_message, &unwrapped_message_length), HERMES_SUCCESS, "themis secure message unwrap failed", (free(wrapped_message), free(unwrapped_message)));

  if((message_length!=unwrapped_message_length) || (memcmp(message, unwrapped_message, message_length)!=0)){
    free(wrapped_message);
    free(unwrapped_message);
    testsuite_fail_if(true, "message not equal unwrapped_message");
    return -3;
  }
  free(wrapped_message);
  free(unwrapped_message);
  return 0;
}

static void themis_secure_message_test(){
  char    message[]="Hit http://ftp.us.debian.org[1] wheezy Release.gpg"
                    "Hit http://ftp.us.debian.org[2] wheezy-updates Release.gpg"
                    "Hit http://ftp.us.debian.org[3] wheezy Release"
                    "Hit http://ftp.us.debian.org[4] wheezy-updates Release"
                    "Hit http://ftp.us.debian.org[5] wheezy/main Sources"
                    "Hit http://ftp.us.debian.org[6] wheezy/non-free Sources"
                    "Hit http://ftp.us.debian.org[7] wheezy/contrib Sources"
                    "Hit http://ftp.us.debian.org[8] wheezy/main i386 Packages"
                    "Hit http://ftp.us.debian.org[9] wheezy/non-free i386 Packages"
                    "Hit http://ftp.us.debian.org[10] wheezy/contrib i386 Packages"
                    "Hit http://ftp.us.debian.org[11] wheezy/contrib Translation-en";

  size_t message_length=strlen(message);
  
  testsuite_fail_if(themis_secure_signed_message_generic_test(RSA_ALG, message, message_length), "themis secure signed message (RSA)");
  testsuite_fail_if(themis_secure_signed_message_generic_test(EC_ALG, message,message_length), "themis secure signed message (EC)");
  testsuite_fail_if(themis_secure_encrypted_message_generic_test(RSA_ALG, message, message_length), "themis secure encrypted message (RSA)");
  testsuite_fail_if(themis_secure_encrypted_message_generic_test(EC_ALG, message,message_length), "themis secure encrypted message (EC)");
}

void run_secure_message_test(){
  testsuite_enter_suite("generic secure message");
  testsuite_run_test(themis_secure_message_test);
}













