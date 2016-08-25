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
#include <common/test_utils.h>
#include <themis/secure_message.h>

/* Fuzz parameters */
#define MAX_MESSAGE_SIZE 2048
#define MESSAGES_TO_SEND 3

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
  themis_status_t res=THEMIS_FAIL;
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
  
  if(res!=THEMIS_SUCCESS){
    testsuite_fail_if(res!=THEMIS_SUCCESS, "themis_gen_key_pair fail");
    return -1;
  }

  uint8_t* wrapped_message=NULL;
  size_t wrapped_message_length=0;

  res=themis_secure_message_wrap(private_key ,private_key_length, NULL, 0, (uint8_t*)message, message_length, NULL, &wrapped_message_length);
  if(res!=THEMIS_BUFFER_TOO_SMALL){
    testsuite_fail_if(res!=THEMIS_BUFFER_TOO_SMALL, "themis_secure_message_wrap (wrapped_message_length determination) fail");
    return -2;
  }

  wrapped_message=malloc(wrapped_message_length);
  if(!wrapped_message){
    testsuite_fail_if(!wrapped_message, "malloc fail");
    return -3;
  }
  res=themis_secure_message_wrap(private_key, private_key_length, NULL, 0, (uint8_t*)message, message_length, wrapped_message, &wrapped_message_length);
  if(res!=THEMIS_SUCCESS){
    free(wrapped_message);
    testsuite_fail_if(res!=THEMIS_SUCCESS, "themis_secure_message_wrap fail");
    return -4;
  }

  uint8_t* unwrapped_message=NULL;
  size_t unwrapped_message_length=0;
  
  res=themis_secure_message_unwrap(NULL , 0, public_key, public_key_length, wrapped_message, wrapped_message_length, NULL, &unwrapped_message_length);
  if(res!=THEMIS_BUFFER_TOO_SMALL){
    free(wrapped_message);
    testsuite_fail_if(res!=THEMIS_BUFFER_TOO_SMALL, "themis_secure_message_unwrap (unwrapped_message_length determination) fail");
    return -5;
  }
  unwrapped_message=malloc(unwrapped_message_length);
  if(!unwrapped_message){
    free(wrapped_message);
    testsuite_fail_if(!unwrapped_message, "malloc fail");
    return -3;
  }
  res=themis_secure_message_unwrap(NULL, 0, public_key, public_key_length, wrapped_message, wrapped_message_length, unwrapped_message, &unwrapped_message_length);
  if(res!=THEMIS_SUCCESS){
    free(wrapped_message);
    free(unwrapped_message);
    testsuite_fail_if(res!=THEMIS_SUCCESS, "themis_secure_message_unwrap fail");
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


  test_check(themis_gen_key_pair(alg, private_key, &private_key_length, public_key, &public_key_length), THEMIS_SUCCESS, "gen key pair fail");
  test_check(themis_gen_key_pair(alg, peer_private_key, &peer_private_key_length, peer_public_key, &peer_public_key_length), THEMIS_SUCCESS, "gen peer key pair fail");

  uint8_t* wrapped_message=NULL;
  size_t wrapped_message_length=0;

  test_check(themis_secure_message_wrap(private_key ,private_key_length, peer_public_key, peer_public_key_length, (uint8_t*)message, message_length, NULL, &wrapped_message_length), THEMIS_BUFFER_TOO_SMALL, "themis secure message wrap (wrapped message length determination) failed");
  wrapped_message=malloc(wrapped_message_length);
  if(!wrapped_message){
    testsuite_fail_if(!wrapped_message, "malloc fail (wrapped_message)");
    return -2;
  }
  test_check_free(themis_secure_message_wrap(private_key ,private_key_length, peer_public_key, peer_public_key_length, (uint8_t*)message, message_length, wrapped_message, &wrapped_message_length), THEMIS_SUCCESS, "themis secure message wrap failed", free(wrapped_message));

  uint8_t* unwrapped_message=NULL;
  size_t unwrapped_message_length=0;

  test_check_free(themis_secure_message_unwrap(peer_private_key ,private_key_length, public_key, public_key_length, (uint8_t*)wrapped_message, wrapped_message_length, NULL, &unwrapped_message_length), THEMIS_BUFFER_TOO_SMALL, "themis secure message unwrap (unwrapped message length determination) failed", free(wrapped_message));
  unwrapped_message=malloc(unwrapped_message_length);
  if(!unwrapped_message){
    testsuite_fail_if(!unwrapped_message, "malloc fail (unwrapped_message)");
    return -2;
  }
  test_check_free(themis_secure_message_unwrap(peer_private_key ,peer_private_key_length, public_key, public_key_length, (uint8_t*)wrapped_message, wrapped_message_length, unwrapped_message, &unwrapped_message_length), THEMIS_SUCCESS, "themis secure message unwrap failed", (free(wrapped_message), free(unwrapped_message)));

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

static void secure_message_api_test(void)
{
	uint8_t plaintext[MAX_MESSAGE_SIZE];
	size_t plaintext_length = 2048; //rand_int(MAX_MESSAGE_SIZE);

	uint8_t ciphertext[MAX_MESSAGE_SIZE+52]; //chipther text allwais bigger then plain text (24 bytes of header + 12 bytes of iv + 16 bytes of auth_tag)
	size_t ciphertext_length = sizeof(ciphertext);

	uint8_t decryptext[MAX_MESSAGE_SIZE];
	size_t decryptext_length = sizeof(decryptext);

	uint8_t priv[MAX_MESSAGE_SIZE];
	size_t priv_length = sizeof(priv);

	uint8_t pub[MAX_MESSAGE_SIZE];
	size_t pub_length = sizeof(pub);

	uint8_t peer_priv[MAX_MESSAGE_SIZE];
	size_t peer_priv_length = sizeof(peer_priv);

	uint8_t peer_pub[MAX_MESSAGE_SIZE];
	size_t peer_pub_length = sizeof(peer_pub);

	themis_status_t res;

	testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL == themis_gen_ec_key_pair(NULL, &priv_length, pub, &pub_length), "themis_gen_ec_key_pair: get output size (NULL out buffer for private key)");
	priv_length--;
	testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL == themis_gen_ec_key_pair(priv, &priv_length, pub, &pub_length), "themis_gen_ec_key_pair: get output size (small out buffer for private key)");

	testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL == themis_gen_ec_key_pair(peer_priv, &priv_length, NULL, &pub_length), "themis_gen_ec_key_pair: get output size (NULL out buffer for public key)");
	pub_length--;
	testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL == themis_gen_ec_key_pair(peer_priv, &priv_length, pub, &pub_length), "themis_gen_ec_key_pair: get output size (small out buffer for public key)");

	res = themis_gen_ec_key_pair(priv, &priv_length, pub, &pub_length);
	if (THEMIS_SUCCESS != res)
	{
		testsuite_fail_if(true, "themis_gen_ec_key_pair fail");
		return;
	}

	res = themis_gen_ec_key_pair(peer_priv, &peer_priv_length, peer_pub, &peer_pub_length);
	if (THEMIS_SUCCESS != res)
	{
		testsuite_fail_if(true, "themis_gen_ec_key_pair fail");
		return;
	}

	if (THEMIS_SUCCESS != soter_rand(plaintext, plaintext_length))
	{
		testsuite_fail_if(true, "soter_rand fail");
		return;
	}

	testsuite_fail_unless(THEMIS_INVALID_PARAMETER == themis_secure_message_wrap(NULL, priv_length, peer_pub, peer_pub_length, plaintext, plaintext_length, ciphertext, &ciphertext_length), "themis_secure_message_wrap: invalid private key");
	testsuite_fail_unless(THEMIS_INVALID_PARAMETER == themis_secure_message_wrap(priv, priv_length - 1, peer_pub, peer_pub_length, plaintext, plaintext_length, ciphertext, &ciphertext_length), "themis_secure_message_wrap: invalid private key length");
	testsuite_fail_unless(THEMIS_INVALID_PARAMETER == themis_secure_message_wrap(priv, priv_length, peer_pub, peer_pub_length - 1, plaintext, plaintext_length, ciphertext, &ciphertext_length), "themis_secure_message_wrap: invalid peer public key length");
	testsuite_fail_unless(THEMIS_INVALID_PARAMETER == themis_secure_message_wrap(priv, priv_length, peer_pub, peer_pub_length, NULL, plaintext_length, ciphertext, &ciphertext_length), "themis_secure_message_wrap: invalid plaintext");
	testsuite_fail_unless(THEMIS_INVALID_PARAMETER == themis_secure_message_wrap(priv, priv_length, peer_pub, peer_pub_length, plaintext, 0, ciphertext, &ciphertext_length), "themis_secure_message_wrap: invalid plaintext length");
	testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL == themis_secure_message_wrap(priv, priv_length, pub, pub_length, plaintext, plaintext_length, NULL, &ciphertext_length), "themis_secure_message_wrap: get output size (NULL out buffer)");
	ciphertext_length--;
	testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL == themis_secure_message_wrap(priv, priv_length, peer_pub, peer_pub_length, plaintext, plaintext_length, ciphertext, &ciphertext_length), "themis_secure_message_wrap: get output size (small out buffer)");

	res = themis_secure_message_wrap(priv, priv_length, peer_pub, peer_pub_length, plaintext, plaintext_length, ciphertext, &ciphertext_length);
	if (THEMIS_SUCCESS != res)
	{
		testsuite_fail_if(true, "themis_secure_message_wrap fail");
		return;
	}

	testsuite_fail_unless(THEMIS_INVALID_PARAMETER == themis_secure_message_unwrap(NULL, peer_priv_length, pub, pub_length, ciphertext, ciphertext_length, decryptext, &decryptext_length), "themis_secure_message_unwrap: invalid private key");
	testsuite_fail_unless(THEMIS_INVALID_PARAMETER == themis_secure_message_unwrap(peer_priv, peer_priv_length - 1, pub, pub_length, ciphertext, ciphertext_length, decryptext, &decryptext_length), "themis_secure_message_unwrap: invalid private key length");
	testsuite_fail_unless(THEMIS_INVALID_PARAMETER == themis_secure_message_unwrap(peer_priv, peer_priv_length, pub, pub_length - 1, ciphertext, ciphertext_length, decryptext, &decryptext_length), "themis_secure_message_unwrap: invalid peer public key length");
	testsuite_fail_unless(THEMIS_INVALID_PARAMETER == themis_secure_message_unwrap(peer_priv, peer_priv_length, pub, pub_length, NULL, ciphertext_length, decryptext, &decryptext_length), "themis_secure_message_unwrap: invalid ciphertext");
	testsuite_fail_unless(THEMIS_INVALID_PARAMETER == themis_secure_message_unwrap(peer_priv, peer_priv_length, pub, pub_length, ciphertext, 0, decryptext, &decryptext_length), "themis_secure_message_unwrap: invalid ciphertext length");
	testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL == themis_secure_message_unwrap(peer_priv, peer_priv_length, pub, pub_length, ciphertext, ciphertext_length, NULL, &decryptext_length), "themis_secure_message_unwrap: get output size (NULL out buffer)");
	decryptext_length--;
	testsuite_fail_unless(THEMIS_BUFFER_TOO_SMALL == themis_secure_message_unwrap(peer_priv, peer_priv_length, pub, pub_length, ciphertext, ciphertext_length, decryptext, &decryptext_length), "themis_secure_message_unwrap: get output size (small out buffer)");

	testsuite_fail_unless(THEMIS_INVALID_PARAMETER == themis_secure_message_unwrap(peer_priv, peer_priv_length, NULL, pub_length, ciphertext, ciphertext_length, decryptext, &decryptext_length), "themis_secure_message_unwrap: treating encrypted message as signed (NULL peer public key)");
	testsuite_fail_unless(THEMIS_INVALID_PARAMETER == themis_secure_message_unwrap(peer_priv, peer_priv_length, pub, 0, ciphertext, ciphertext_length, decryptext, &decryptext_length), "themis_secure_message_unwrap: treating encrypted message as signed (zero peer public key length)");

	res = themis_secure_message_unwrap(peer_priv, peer_priv_length, pub, pub_length, ciphertext, ciphertext_length, decryptext, &decryptext_length);
	if (THEMIS_SUCCESS != res)
	{
		testsuite_fail_if(true, "themis_secure_message_unwrap fail");
		return;
	}

	testsuite_fail_unless((decryptext_length == plaintext_length), "generic secure message: normal flow");
	testsuite_fail_unless((!memcmp(plaintext, decryptext, plaintext_length)), "generic secure message: normal flow 2");
}

void run_secure_message_test(){
  testsuite_enter_suite("generic secure message");
  testsuite_run_test(themis_secure_message_test);

  testsuite_enter_suite("generic secure message: api test");
  testsuite_run_test(secure_message_api_test);
}













