/**
 * @file
 *
 * (c) CossackLabs
 */

#include <string.h>
#include <common/test_utils.h>
#include <themis/secure_cell.h>

/* Fuzz parameters */
#define MAX_KEY_SIZE 256
#define MAX_CONTEXT_SIZE 2048
#define MAX_MESSAGE_SIZE 4096

static char passwd[]="password";
static char message[]="secure cell test message by Mnatsakanov Andrey from Cossack Labs";
static char user_context[]="secure cell user context";

static int secure_cell_full(){
  uint8_t* encrypted_message;
  size_t encrypted_message_length=0;
  if(themis_secure_cell_encrypt_full((uint8_t*)passwd, sizeof(passwd), NULL,0,(uint8_t*)message, sizeof(message), NULL, &encrypted_message_length)!=HERMES_BUFFER_TOO_SMALL || encrypted_message_length==0){
    testsuite_fail_if(true, "themis_secure_cell_encrypt_full (encrypted_message_length determination) fail");
    return -1;
  }
  encrypted_message=malloc(encrypted_message_length);
  if(encrypted_message==NULL){
    testsuite_fail_if(true, "encrypted_message malloc fail");
    return -2;
  }
  if(themis_secure_cell_encrypt_full((uint8_t*)passwd, sizeof(passwd), NULL,0,(uint8_t*)message, sizeof(message), encrypted_message, &encrypted_message_length)!=HERMES_SUCCESS){
    testsuite_fail_if(true, "themis_secure_cell_encrypt_full fail");
    free(encrypted_message);
    return -3;
  }
  
  uint8_t* decrypted_message;
  size_t decrypted_message_length=0;

  if(themis_secure_cell_decrypt_full((uint8_t*)passwd, sizeof(passwd), NULL,0,encrypted_message, encrypted_message_length, NULL, &decrypted_message_length)!=HERMES_BUFFER_TOO_SMALL || decrypted_message_length==0){
    testsuite_fail_if(true, "themis_secure_cell_decrypt_full (decrypted_message_length determination) fail");
    free(encrypted_message);
    return -4;
  }
  decrypted_message=malloc(decrypted_message_length);
  if(decrypted_message==NULL){
    testsuite_fail_if(true, "decrypted_message malloc fail");
    free(encrypted_message);
    return -5;
  }
  if(themis_secure_cell_decrypt_full((uint8_t*)passwd, sizeof(passwd), NULL,0,encrypted_message, encrypted_message_length, decrypted_message, &decrypted_message_length)!=HERMES_SUCCESS){
    testsuite_fail_if(true, "themis_secure_cell_decrypt_full fail");
    free(encrypted_message);
    free(decrypted_message);
    return -6;
  }

  if(sizeof(message)!=decrypted_message_length || memcmp((uint8_t*)message, decrypted_message, decrypted_message_length)!=0){
    testsuite_fail_if(true, "message and decrypted_message not equal");
    free(encrypted_message);
    free(decrypted_message);
    return -6;
  }
  free(encrypted_message);
  free(decrypted_message);
  return 0;
}

static int secure_cell_auto_split(){
  uint8_t* encrypted_message;
  size_t encrypted_message_length=0;
  uint8_t* context;
  size_t context_length=0;
  
  if(themis_secure_cell_encrypt_auto_split((uint8_t*)passwd, sizeof(passwd), NULL,0,(uint8_t*)message, sizeof(message), NULL, &context_length, NULL, &encrypted_message_length)!=HERMES_BUFFER_TOO_SMALL || encrypted_message_length==0){
    testsuite_fail_if(true, "themis_secure_cell_encrypt_auto_split (encrypted_message_length determination) fail");
    return -1;
  }
  encrypted_message=malloc(encrypted_message_length);
  if(encrypted_message==NULL){
    testsuite_fail_if(true, "encrypted_message malloc fail");
    return -2;
  }
  context=malloc(context_length);
  if(encrypted_message==NULL){
    testsuite_fail_if(true, "context malloc fail");
    return -2;
  }
  
  if(themis_secure_cell_encrypt_auto_split((uint8_t*)passwd, sizeof(passwd), NULL,0, (uint8_t*)message, sizeof(message), context, &context_length, encrypted_message, &encrypted_message_length)!=HERMES_SUCCESS){
    testsuite_fail_if(true, "themis_secure_cell_encrypt_auto_split fail");
    free(encrypted_message);
    free(context);
    return -3;
  }
  uint8_t* decrypted_message;
  size_t decrypted_message_length=0;

  if(themis_secure_cell_decrypt_auto_split((uint8_t*)passwd, sizeof(passwd), NULL,0,encrypted_message, encrypted_message_length, context, context_length, NULL, &decrypted_message_length)!=HERMES_BUFFER_TOO_SMALL || decrypted_message_length==0){
    testsuite_fail_if(true, "themis_secure_cell_decrypt_auto_split (decrypted_message_length determination) fail");
    free(encrypted_message);
    free(context);
    return -4;
  }
  decrypted_message=malloc(decrypted_message_length);
  if(decrypted_message==NULL){
    testsuite_fail_if(true, "decrypted_message malloc fail");
    free(encrypted_message);
    free(context);
    return -5;
  }
  if(themis_secure_cell_decrypt_auto_split((uint8_t*)passwd, sizeof(passwd), NULL,0,encrypted_message, encrypted_message_length, context, context_length, decrypted_message, &decrypted_message_length)!=HERMES_SUCCESS){
    testsuite_fail_if(true, "themis_secure_cell_decrypt_auto_split fail");
    free(encrypted_message);
    free(decrypted_message);
    free(context);
    return -6;
  }

  if(sizeof(message)!=decrypted_message_length || memcmp((uint8_t*)message, decrypted_message, decrypted_message_length)!=0){
    testsuite_fail_if(true, "message and decrypted_message not equal");
    free(encrypted_message);
    free(decrypted_message);
    free(context);
    return -7;
  }
  free(encrypted_message);
  free(decrypted_message);
  free(context);  
  return 0;
}

static int secure_cell_user_split(){
  uint8_t* encrypted_message;
  size_t encrypted_message_length=0;
  
  if(themis_secure_cell_encrypt_user_split((uint8_t*)passwd, sizeof(passwd), (uint8_t*)message, sizeof(message), (uint8_t*)user_context, strlen(user_context), NULL, &encrypted_message_length)!=HERMES_BUFFER_TOO_SMALL || encrypted_message_length==0){
    testsuite_fail_if(true, "themis_secure_cell_encrypt_user_split (encrypted_message_length determination) fail");
    return -1;
  }
  encrypted_message=malloc(encrypted_message_length);
  if(encrypted_message==NULL){
    testsuite_fail_if(true, "encrypted_message malloc fail");
    return -2;
  }
  
  if(themis_secure_cell_encrypt_user_split((uint8_t*)passwd, sizeof(passwd), (uint8_t*)message, sizeof(message), (uint8_t*)user_context, strlen(user_context), encrypted_message, &encrypted_message_length)!=HERMES_SUCCESS){
    testsuite_fail_if(true, "themis_secure_cell_encrypt_user_split fail");
    free(encrypted_message);
    return -3;
  }

  uint8_t* decrypted_message;
  size_t decrypted_message_length=0;

  if(themis_secure_cell_decrypt_user_split((uint8_t*)passwd, sizeof(passwd), encrypted_message, encrypted_message_length, (uint8_t*)user_context, strlen(user_context), NULL, &decrypted_message_length)!=HERMES_BUFFER_TOO_SMALL || decrypted_message_length==0){
    testsuite_fail_if(true, "themis_secure_cell_decrypt_user_split (decrypted_message_length determination) fail");
    free(encrypted_message);
    return -4;
  }
  decrypted_message=malloc(decrypted_message_length);
  if(decrypted_message==NULL){
    testsuite_fail_if(true, "decrypted_message malloc fail");
    free(encrypted_message);
    return -5;
  }
    if(themis_secure_cell_encrypt_user_split((uint8_t*)passwd, sizeof(passwd), encrypted_message, encrypted_message_length, (uint8_t*)user_context, strlen(user_context), decrypted_message, &decrypted_message_length)!=HERMES_SUCCESS){
    testsuite_fail_if(true, "themis_secure_cell_decrypt_user_split fail");
    free(encrypted_message);
    free(decrypted_message);
    return -6;
  }

  if(sizeof(message)!=decrypted_message_length || memcmp((uint8_t*)message, decrypted_message, decrypted_message_length)!=0){
    testsuite_fail_if(true, "message and decrypted_message not equal");
    free(encrypted_message);
    free(decrypted_message);
    return -7;
  }
  free(encrypted_message);
  free(decrypted_message);
  return 0;
}


static void secure_cell_test(){
  testsuite_fail_if(secure_cell_full(),"secure cell full mode");
  testsuite_fail_if(secure_cell_auto_split(),"secure cell auto split mode");
  testsuite_fail_if(secure_cell_user_split(),"secure cell user split mode");
}

static void secure_cell_api_test_full(void)
{
	uint8_t key[MAX_KEY_SIZE];
	uint8_t message[MAX_MESSAGE_SIZE];

	size_t key_length = rand_int(MAX_KEY_SIZE);
	size_t message_length = rand_int(MAX_MESSAGE_SIZE);

	uint8_t *encrypted = NULL;
	size_t encrypted_length;

	uint8_t *decrypted = NULL;
	size_t decrypted_length;

	if (HERMES_SUCCESS != soter_rand(key, key_length))
	{
		testsuite_fail_if(true, "soter_rand fail");
		return;
	}

	if (HERMES_SUCCESS != soter_rand(message, message_length))
	{
		testsuite_fail_if(true, "soter_rand fail");
		return;
	}

	testsuite_fail_unless(HERMES_BUFFER_TOO_SMALL == themis_secure_cell_encrypt_full(key, key_length, NULL,0,message, message_length, NULL, &encrypted_length), "themis_secure_cell_encrypt_full: get output size (NULL out buffer)");
	encrypted = malloc(encrypted_length);
	if (!encrypted)
	{
		testsuite_fail_if(true, "malloc fail");
		return;
	}

	testsuite_fail_unless(HERMES_INVALID_PARAMETER == themis_secure_cell_encrypt_full(NULL, key_length, NULL,0,message, message_length, encrypted, &encrypted_length), "themis_secure_cell_encrypt_full: invalid key");
	testsuite_fail_unless(HERMES_INVALID_PARAMETER == themis_secure_cell_encrypt_full(key, 0, NULL,0,message, message_length, encrypted, &encrypted_length), "themis_secure_cell_encrypt_full: invalid key length");

	testsuite_fail_unless(HERMES_INVALID_PARAMETER == themis_secure_cell_encrypt_full(key, key_length, NULL,0,NULL, message_length, encrypted, &encrypted_length), "themis_secure_cell_encrypt_full: invalid message");
	testsuite_fail_unless(HERMES_INVALID_PARAMETER == themis_secure_cell_encrypt_full(key, key_length, NULL,0,message, 0, encrypted, &encrypted_length), "themis_secure_cell_encrypt_full: invalid message length");

	encrypted_length--;
	testsuite_fail_unless(HERMES_BUFFER_TOO_SMALL == themis_secure_cell_encrypt_full(key, key_length, NULL,0,message, message_length, encrypted, &encrypted_length), "themis_secure_cell_encrypt_full: get output size (small out buffer)");

	testsuite_fail_unless(HERMES_SUCCESS == themis_secure_cell_encrypt_full(key, key_length, NULL,0,message, message_length, encrypted, &encrypted_length), "themis_secure_cell_encrypt_full: normal flow");

	testsuite_fail_unless(HERMES_BUFFER_TOO_SMALL == themis_secure_cell_decrypt_full(key, key_length, NULL,0,encrypted, encrypted_length, NULL, &decrypted_length), "themis_secure_cell_decrypt_full: get output size (NULL out buffer)");

	decrypted = malloc(decrypted_length);
	if (!decrypted)
	{
		testsuite_fail_if(true, "malloc fail");
		free(encrypted);
		return;
	}

	testsuite_fail_unless(HERMES_INVALID_PARAMETER == themis_secure_cell_decrypt_full(NULL, key_length, NULL,0,encrypted, encrypted_length, decrypted, &decrypted_length), "themis_secure_cell_decrypt_full: invalid key");
	testsuite_fail_unless(HERMES_INVALID_PARAMETER == themis_secure_cell_decrypt_full(key, 0, NULL,0,encrypted, encrypted_length, decrypted, &decrypted_length), "themis_secure_cell_decrypt_full: invalid key length");

	testsuite_fail_unless(HERMES_INVALID_PARAMETER == themis_secure_cell_decrypt_full(key, key_length, NULL,0,NULL, encrypted_length, decrypted, &decrypted_length), "themis_secure_cell_decrypt_full: invalid message");
	testsuite_fail_unless(HERMES_INVALID_PARAMETER == themis_secure_cell_decrypt_full(key, key_length, NULL,0,encrypted, 0, decrypted, &decrypted_length), "themis_secure_cell_decrypt_full: invalid message length");

	decrypted_length--;
	testsuite_fail_unless(HERMES_BUFFER_TOO_SMALL == themis_secure_cell_decrypt_full(key, key_length, NULL,0,encrypted, encrypted_length, decrypted, &decrypted_length), "themis_secure_cell_decrypt_full: get output size (small out buffer)");

	encrypted[0]++;
	testsuite_fail_unless(HERMES_FAIL == themis_secure_cell_decrypt_full(key, key_length, NULL,0,encrypted, encrypted_length, decrypted, &decrypted_length), "themis_secure_cell_decrypt_full: header corrupt");
	encrypted[0]--;

	encrypted[encrypted_length / 2]++;
	testsuite_fail_unless(HERMES_FAIL == themis_secure_cell_decrypt_full(key, key_length, NULL,0,encrypted, encrypted_length, decrypted, &decrypted_length), "themis_secure_cell_decrypt_full: message corrupt");
	encrypted[encrypted_length / 2]--;

	encrypted[encrypted_length - 1]++;
	testsuite_fail_unless(HERMES_FAIL == themis_secure_cell_decrypt_full(key, key_length, NULL,0,encrypted, encrypted_length, decrypted, &decrypted_length), "themis_secure_cell_decrypt_full: tag corrupt");
	encrypted[encrypted_length - 1]--;

	testsuite_fail_unless(HERMES_SUCCESS == themis_secure_cell_decrypt_full(key, key_length, NULL,0,encrypted, encrypted_length, decrypted, &decrypted_length) && (message_length == decrypted_length) && !memcmp(message, decrypted, message_length), "themis_secure_cell_decrypt_full: normal flow");

	free(decrypted);
	free(encrypted);
}

static void secure_cell_api_test_user_split(void)
{
	uint8_t key[MAX_KEY_SIZE];
	uint8_t context[MAX_CONTEXT_SIZE];
	uint8_t message[MAX_MESSAGE_SIZE];

	size_t key_length = rand_int(MAX_KEY_SIZE);
	size_t context_length = rand_int(MAX_CONTEXT_SIZE);
	size_t message_length = rand_int(MAX_MESSAGE_SIZE);
	int i=0;

	uint8_t encrypted[MAX_MESSAGE_SIZE];
	size_t encrypted_length = message_length;

	uint8_t decrypted[MAX_MESSAGE_SIZE];
	size_t decrypted_length = message_length;

	if (HERMES_SUCCESS != soter_rand(key, key_length))
	{
		testsuite_fail_if(true, "soter_rand fail");
		return;
	}

	if (HERMES_SUCCESS != soter_rand(context, context_length))
	{
		testsuite_fail_if(true, "soter_rand fail");
		return;
	}

	if (HERMES_SUCCESS != soter_rand(message, message_length))
	{
		testsuite_fail_if(true, "soter_rand fail");
		return;
	}

	testsuite_fail_unless(HERMES_INVALID_PARAMETER == themis_secure_cell_encrypt_user_split(NULL, key_length, message, message_length, context, context_length, encrypted, &encrypted_length), "themis_secure_cell_encrypt_user_split: invalid key");
	testsuite_fail_unless(HERMES_INVALID_PARAMETER == themis_secure_cell_encrypt_user_split(key, 0, message, message_length, context, context_length, encrypted, &encrypted_length), "themis_secure_cell_encrypt_user_split: invalid key length");
	
	testsuite_fail_unless(HERMES_INVALID_PARAMETER == themis_secure_cell_encrypt_user_split(key, key_length, NULL, message_length, context, context_length, encrypted, &encrypted_length), "themis_secure_cell_encrypt_user_split: invalid message");
	testsuite_fail_unless(HERMES_INVALID_PARAMETER == themis_secure_cell_encrypt_user_split(key, key_length, message, 0, context, context_length, encrypted, &encrypted_length), "themis_secure_cell_encrypt_user_split: invalid message length");

	testsuite_fail_unless(HERMES_INVALID_PARAMETER == themis_secure_cell_encrypt_user_split(key, key_length, message, message_length, NULL, context_length, encrypted, &encrypted_length), "themis_secure_cell_encrypt_user_split: invalid context");
	testsuite_fail_unless(HERMES_INVALID_PARAMETER == themis_secure_cell_encrypt_user_split(key, key_length, message, message_length, context, 0, encrypted, &encrypted_length), "themis_secure_cell_encrypt_user_split: invalid context length");

	testsuite_fail_unless((HERMES_BUFFER_TOO_SMALL == themis_secure_cell_encrypt_user_split(key, key_length, message, message_length, context, context_length, NULL, &encrypted_length)) && (encrypted_length == message_length), "themis_secure_cell_encrypt_user_split: get output size (NULL out buffer)");

	 encrypted_length--;
	testsuite_fail_unless((HERMES_BUFFER_TOO_SMALL == themis_secure_cell_encrypt_user_split(key, key_length, message, message_length, context, context_length, encrypted, &encrypted_length)) && (encrypted_length == message_length) , "themis_secure_cell_encrypt_user_split: get output size (small out buffer)");

	testsuite_fail_unless((HERMES_SUCCESS == themis_secure_cell_encrypt_user_split(key, key_length, message, message_length, context, context_length, encrypted, &encrypted_length)) && (encrypted_length == message_length), "themis_secure_cell_encrypt_user_split: normal flow");

	testsuite_fail_unless(HERMES_INVALID_PARAMETER == themis_secure_cell_decrypt_user_split(NULL, key_length, encrypted, encrypted_length, context, context_length, decrypted, &decrypted_length), "themis_secure_cell_decrypt_user_split: invalid key");
	testsuite_fail_unless(HERMES_INVALID_PARAMETER == themis_secure_cell_decrypt_user_split(key, 0, encrypted, encrypted_length, context, context_length, decrypted, &decrypted_length), "themis_secure_cell_decrypt_user_split: invalid key length");

	testsuite_fail_unless(HERMES_INVALID_PARAMETER == themis_secure_cell_decrypt_user_split(key, key_length, NULL, encrypted_length, context, context_length, decrypted, &decrypted_length), "themis_secure_cell_decrypt_user_split: invalid message");
	testsuite_fail_unless(HERMES_INVALID_PARAMETER == themis_secure_cell_decrypt_user_split(key, key_length, encrypted, 0, context, context_length, decrypted, &decrypted_length), "themis_secure_cell_decrypt_user_split: invalid message length");

	testsuite_fail_unless(HERMES_INVALID_PARAMETER == themis_secure_cell_decrypt_user_split(key, key_length, encrypted, encrypted_length, NULL, context_length, decrypted, &decrypted_length), "themis_secure_cell_decrypt_user_split: invalid context");
	testsuite_fail_unless(HERMES_INVALID_PARAMETER == themis_secure_cell_decrypt_user_split(key, key_length, encrypted, encrypted_length, context, 0, decrypted, &decrypted_length), "themis_secure_cell_decrypt_user_split: invalid context length");

	testsuite_fail_unless((HERMES_BUFFER_TOO_SMALL == themis_secure_cell_decrypt_user_split(key, key_length, encrypted, encrypted_length, context, context_length, NULL, &decrypted_length)) && (decrypted_length == encrypted_length), "themis_secure_cell_decrypt_user_split: get output size (NULL out buffer)");

	decrypted_length--;
	testsuite_fail_unless((HERMES_BUFFER_TOO_SMALL == themis_secure_cell_decrypt_user_split(key, key_length, encrypted, encrypted_length, context, context_length, decrypted, &decrypted_length)) && (decrypted_length == encrypted_length), "themis_secure_cell_decrypt_user_split: get output size (small out buffer)");

	encrypted[0]++;
	testsuite_fail_unless(HERMES_FAIL == themis_secure_cell_decrypt_user_split(key, key_length, encrypted, encrypted_length, context, context_length, decrypted, &decrypted_length), "themis_secure_cell_decrypt_user_split: header corrupt");
	encrypted[0]--;

	encrypted[encrypted_length / 2]++;
	testsuite_fail_unless(HERMES_FAIL == themis_secure_cell_decrypt_user_split(key, key_length, encrypted, encrypted_length, context, context_length, decrypted, &decrypted_length), "themis_secure_cell_decrypt_user_split: message corrupt");
	encrypted[encrypted_length / 2]--;

	encrypted[encrypted_length - 1]++;
	testsuite_fail_unless(HERMES_FAIL == themis_secure_cell_decrypt_user_split(key, key_length, encrypted, encrypted_length, context, context_length, decrypted, &decrypted_length), "themis_secure_cell_decrypt_user_split: tag corrupt");
	encrypted[encrypted_length - 1]--;

	testsuite_fail_unless(HERMES_SUCCESS == themis_secure_cell_decrypt_user_split(key, key_length, encrypted, encrypted_length, context, context_length, decrypted, &decrypted_length) && (message_length == decrypted_length) && (!memcmp(message, decrypted, message_length)), "themis_secure_cell_decrypt_user_split: normal flow");
}

static void secure_cell_api_test(void)
{
	secure_cell_api_test_full();
	secure_cell_api_test_user_split();
}

void run_secure_cell_test(){
  testsuite_enter_suite("secure cell: basic flow");
  testsuite_run_test(secure_cell_test);

  testsuite_enter_suite("secure cell: api test");
  testsuite_run_test(secure_cell_api_test);
}
