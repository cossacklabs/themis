/**
 * @file
 *
 * (c) CossackLabs
 */

#include <string.h>
#include <common/test_utils.h>
#include <themis/secure_cell.h>

char passwd[]="password";
char message[]="secure cell test message by Mnatsakanov Andrey from Cossack Labs";
char user_context[]="secure cell user context";


static int secure_cell_full(){
  uint8_t* encrypted_message;
  size_t encrypted_message_length=0;
  if(themis_secure_cell_encrypt_full((uint8_t*)passwd, sizeof(passwd), (uint8_t*)message, sizeof(message), NULL, &encrypted_message_length)!=HERMES_BUFFER_TOO_SMALL || encrypted_message_length==0){
    testsuite_fail_if(true, "themis_secure_cell_encrypt_full (encrypted_message_length determination) fail");
    return -1;
  }
  encrypted_message=malloc(encrypted_message_length);
  if(encrypted_message==NULL){
    testsuite_fail_if(true, "encrypted_message malloc fail");
    return -2;
  }
  if(themis_secure_cell_encrypt_full((uint8_t*)passwd, sizeof(passwd), (uint8_t*)message, sizeof(message), encrypted_message, &encrypted_message_length)!=HERMES_SUCCESS){
    testsuite_fail_if(true, "themis_secure_cell_encrypt_full fail");
    free(encrypted_message);
    return -3;
  }
  
  uint8_t* decrypted_message;
  size_t decrypted_message_length=0;

  if(themis_secure_cell_decrypt_full((uint8_t*)passwd, sizeof(passwd), encrypted_message, encrypted_message_length, NULL, &decrypted_message_length)!=HERMES_BUFFER_TOO_SMALL || decrypted_message_length==0){
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
  if(themis_secure_cell_decrypt_full((uint8_t*)passwd, sizeof(passwd), encrypted_message, encrypted_message_length, decrypted_message, &decrypted_message_length)!=HERMES_SUCCESS){
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
  
  if(themis_secure_cell_encrypt_auto_split((uint8_t*)passwd, sizeof(passwd), (uint8_t*)message, sizeof(message), NULL, &context_length, NULL, &encrypted_message_length)!=HERMES_BUFFER_TOO_SMALL || encrypted_message_length==0){
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
  
  if(themis_secure_cell_encrypt_auto_split((uint8_t*)passwd, sizeof(passwd), (uint8_t*)message, sizeof(message), context, &context_length, encrypted_message, &encrypted_message_length)!=HERMES_SUCCESS){
    testsuite_fail_if(true, "themis_secure_cell_encrypt_auto_split fail");
    free(encrypted_message);
    free(context);
    return -3;
  }
  uint8_t* decrypted_message;
  size_t decrypted_message_length=0;

  if(themis_secure_cell_decrypt_auto_split((uint8_t*)passwd, sizeof(passwd), encrypted_message, encrypted_message_length, context, context_length, NULL, &decrypted_message_length)!=HERMES_BUFFER_TOO_SMALL || decrypted_message_length==0){
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
  if(themis_secure_cell_decrypt_auto_split((uint8_t*)passwd, sizeof(passwd), encrypted_message, encrypted_message_length, context, context_length, decrypted_message, &decrypted_message_length)!=HERMES_SUCCESS){
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
  
  if(themis_secure_cell_encrypt_user_split((uint8_t*)passwd, sizeof(passwd), (uint8_t*)message, sizeof(message), user_context, strlen(user_context), NULL, &encrypted_message_length)!=HERMES_BUFFER_TOO_SMALL || encrypted_message_length==0){
    testsuite_fail_if(true, "themis_secure_cell_encrypt_user_split (encrypted_message_length determination) fail");
    return -1;
  }
  encrypted_message=malloc(encrypted_message_length);
  if(encrypted_message==NULL){
    testsuite_fail_if(true, "encrypted_message malloc fail");
    return -2;
  }
  
  if(themis_secure_cell_encrypt_user_split((uint8_t*)passwd, sizeof(passwd), (uint8_t*)message, sizeof(message), user_context, strlen(user_context), encrypted_message, &encrypted_message_length)!=HERMES_SUCCESS){
    testsuite_fail_if(true, "themis_secure_cell_encrypt_user_split fail");
    free(encrypted_message);
    return -3;
  }

  uint8_t* decrypted_message;
  size_t decrypted_message_length=0;

  if(themis_secure_cell_decrypt_user_split((uint8_t*)passwd, sizeof(passwd), encrypted_message, encrypted_message_length, user_context, strlen(user_context), NULL, &decrypted_message_length)!=HERMES_BUFFER_TOO_SMALL || decrypted_message_length==0){
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
    if(themis_secure_cell_encrypt_user_split((uint8_t*)passwd, sizeof(passwd), encrypted_message, encrypted_message_length, user_context, strlen(user_context), decrypted_message, &decrypted_message_length)!=HERMES_SUCCESS){
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

void run_secure_cell_test(){
  testsuite_enter_suite("secure cell");
  testsuite_run_test(secure_cell_test);
}
