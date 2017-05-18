/*
 * Copyright (c) 2017 Cossack Labs Limited
 *
 * This file is part of Hermes.
 *
 * Hermes is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Hermes is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Hermes.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "soter_crypter.h"

#include <soter/soter.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/stat.h>
//#include <sys/shm.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "../soter_engine.h"


struct soter_crypter_type{
  soter_keys_db_t* db;
  sem_t* sem;
  int fd_main;
  int fd_in_params;
  int fd_out_params;
  soter_crypter_params_t* params_main;
};

soter_crypter_t* soter_crypter_create(soter_keys_db_t* db){
  if(!db){
    return NULL;
  }
  soter_crypter_t* res=calloc(1,sizeof(soter_crypter_t));
  assert(res);
  res->db=db;
  res->sem = sem_open (SOTER_CRYPTER_SEMAPHORE_NAME, O_CREAT | O_EXCL, 0644, 0);
  if(SEM_FAILED == res->sem){
    soter_crypter_destroy(&res);
    return NULL;
  }
  sem_unlink(SOTER_CRYPTER_SEMAPHORE_NAME);
  if(0 > (res->fd_main = shm_open(SOTER_CRYPTER_MAIN_SHM_NAME, O_RDWR, 0644))) {
    soter_crypter_destroy(&res);
    return NULL;
  }
  shm_unlink(SOTER_CRYPTER_MAIN_SHM_NAME);
  if(0 > ftruncate(res->fd_main, sizeof(soter_crypter_params_t))){
    soter_crypter_destroy(&res);
    return NULL;
  }
  res->params_main=mmap(NULL, sizeof(soter_crypter_params_t), PROT_READ|PROT_WRITE, MAP_SHARED, res->fd_main, 0);
  if(MAP_FAILED!=(res->params_main)){
    soter_crypter_destroy(&res);
    return NULL;    
  }
  if(0 > (res->fd_in_params = shm_open(SOTER_CRYPTER_IN_PARAMS_SHM_NAME, O_RDONLY | O_CREAT | O_EXCL, 0644))) {
    soter_crypter_destroy(&res);
    return NULL;
  }
  shm_unlink(SOTER_CRYPTER_OUT_PARAMS_SHM_NAME);
  if(0 > (res->fd_out_params = shm_open(SOTER_CRYPTER_OUT_PARAMS_SHM_NAME, O_RDWR | O_CREAT | O_EXCL, 0644))) {
    soter_crypter_destroy(&res);
    return NULL;
  }
  shm_unlink(SOTER_CRYPTER_OUT_PARAMS_SHM_NAME);  
  return res;
}

soter_status_t soter_crypter_destroy(soter_crypter_t** ks){
  if(!ks || !(*ks)){
    return SOTER_INVALID_PARAMETER;
  }
  if(((*ks)->sem) != SEM_FAILED){
    sem_destroy((*ks)->sem);
  }
  if(((*ks)->fd_main)>0){
    close((*ks)->fd_main);
  }
  if(((*ks)->fd_in_params)>0){
    close((*ks)->fd_in_params);
  }
  if(((*ks)->fd_out_params)>0){
    close((*ks)->fd_out_params);
  }
  free(*ks);
  *ks=NULL;
  return SOTER_SUCCESS;
}

soter_status_t soter_crypter_db_get_key(soter_crypter_t* ks, const uint8_t* id, const size_t id_length, uint8_t** key, size_t* key_length){
  if(!ks || !id || !id_length || !key || !key_length){
    return SOTER_INVALID_PARAMETER;
  }
  size_t key_length_=0;
  if(SOTER_BUFFER_TOO_SMALL!=soter_keys_db_get_key(ks->db, id, id_length, NULL, &key_length_)){
    return SOTER_FAIL;
  }
  *key = malloc(key_length_);
  assert(*key);
  if(SOTER_SUCCESS!=soter_keys_db_get_key(ks->db, id, id_length, *key, &key_length_)){
    free(*key);
    return SOTER_FAIL;
  }
  return SOTER_SUCCESS;
}

soter_status_t soter_crypter_main(soter_crypter_t* ks){
  if(!ks){
    return SOTER_INVALID_PARAMETER;
  }
  sem_wait(ks->sem);
  soter_status_t res=SOTER_SUCCESS;
  uint8_t *param1=NULL, *param2=NULL, *param3=NULL;
  //read params
  if(ks->params_main->param1_len+ks->params_main->param2_len+ks->params_main->param3_len > 0){
    if(0 > ftruncate(ks->fd_in_params, ks->params_main->param1_len+ks->params_main->param2_len+ks->params_main->param3_len)){
      return SOTER_FAIL;
    }
    if(MAP_FAILED!=(param1=mmap(NULL, ks->params_main->param1_len+ks->params_main->param2_len+ks->params_main->param3_len, PROT_READ, MAP_SHARED, ks->fd_in_params, 0))){
      return SOTER_FAIL;
    }
    param2=param1+ks->params_main->param1_len;
    param3=param2+ks->params_main->param2_len;
  }
  uint8_t *res1=NULL, *res2=NULL, *res3=NULL;
  uint8_t *key=NULL;
  size_t res_len=0, key_len=0;
  //res length determination
  switch(ks->params_main->op_code){
  case KA_CREATE:
  case CIPHER_CREATE:
  case SIGN_CREATE:
  case VERIFY_CREATE:
    res_len=sizeof(void*);
    break;
  case KA_DESTROY:
  case CIPHER_DESTROY:
  case SIGN_DESTROY:
  case VERIFY_DESTROY:
    break;
  case KA_DERIVE:
    if(ks->params_main->param1_len == sizeof(void*)){
      res=soter_asym_ka_derive((soter_asym_ka_t*)param1, param2, ks->params_main->param2_len, NULL, &res_len);      
    }
    break;
  case CIPHER_ENCRYPT:
    if(ks->params_main->param1_len == sizeof(void*)){
      res=soter_asym_cipher_encrypt((soter_asym_cipher_t*)param1, param2, ks->params_main->param2_len, NULL, &res_len);      
    }
    break;
  case CIPHER_DECRYPT:
    if(ks->params_main->param1_len == sizeof(void*)){
      res=soter_asym_cipher_decrypt((soter_asym_cipher_t*)param1, param2, ks->params_main->param2_len, NULL, &res_len);      
    }
    break;
  case SIGN_UPDATE:
    break;
  case SIGN_FINAL:
    if(ks->params_main->param1_len == sizeof(void*)){
      res=soter_sign_final((soter_sign_ctx_t*)param1, NULL, &res_len);      
    }    
    break;
  case VERIFY_UPDATE:
  case VERIFY_FINAL:
    break;
  default:
    res = SOTER_FAIL;
  }
  if(res!=SOTER_SUCCESS || res!=SOTER_BUFFER_TOO_SMALL){
    munmap(param1, ks->params_main->param1_len+ks->params_main->param2_len+ks->params_main->param3_len);
    sem_post(ks->sem);
    return res;
  }
  if(res_len){
    if(0 > ftruncate(ks->fd_out_params, res_len)){
        return SOTER_FAIL;
    }
    if(MAP_FAILED!=(res1=mmap(NULL, res_len, PROT_WRITE, MAP_SHARED, ks->fd_out_params, 0))){
      return SOTER_FAIL;
    }
  }
  switch(ks->params_main->op_code){
  case KA_CREATE:
    if(SOTER_SUCCESS!=soter_crypter_db_get_key(ks, param1, ks->params_main->param1_len, &key, &key_len)){
      ks->params_main->op_code=SOTER_FAIL;
      ks->params_main->param1_len=0;
      break;
    }
    res1 = (uint8_t*)soter_asym_ka_create((const int8_t*)param1, ks->params_main->param1_len);
    if(res1!=NULL){
      ks->params_main->op_code=SOTER_SUCCESS;
    } else {
      ks->params_main->op_code=SOTER_FAIL;
    }
    ks->params_main->param1_len=res_len;
    break;
  case CIPHER_CREATE:
    if(SOTER_SUCCESS!=soter_crypter_db_get_key(ks, param1, ks->params_main->param1_len, &key, &key_len)){
      ks->params_main->op_code=SOTER_FAIL;
      ks->params_main->param1_len=0;
      break;
    }
    res1 = (uint8_t*)soter_asym_cipher_create(param1, ks->params_main->param1_len);
    if(res1!=NULL){
      ks->params_main->op_code=SOTER_SUCCESS;
    } else {
      ks->params_main->op_code=SOTER_FAIL;
    }
    ks->params_main->param1_len=res_len;
    break;
  case SIGN_CREATE:
    if(SOTER_SUCCESS!=soter_crypter_db_get_key(ks, param1, ks->params_main->param1_len, &key, &key_len)){
      ks->params_main->op_code=SOTER_FAIL;
      ks->params_main->param1_len=0;
      break;
    }
    res1 = (uint8_t*)soter_sign_create(param1, ks->params_main->param1_len);
    if(res1!=NULL){
      ks->params_main->op_code=SOTER_SUCCESS;
    } else {
      ks->params_main->op_code=SOTER_FAIL;
    }
    ks->params_main->param1_len=res_len;
    break;    
  case VERIFY_CREATE:
    if(SOTER_SUCCESS!=soter_crypter_db_get_key(ks, param1, ks->params_main->param1_len, &key, &key_len)){
      ks->params_main->op_code=SOTER_FAIL;
      ks->params_main->param1_len=0;
      break;
    }
    res1 = (uint8_t*)soter_verify_create(param1, ks->params_main->param1_len);
    if(res1!=NULL){
      ks->params_main->op_code=SOTER_SUCCESS;
    } else {
      ks->params_main->op_code=SOTER_FAIL;
    }
    ks->params_main->param1_len=res_len;
    break;
  case KA_DESTROY:
    ks->params_main->op_code=soter_asym_ka_destroy((soter_asym_ka_t*)param1);
    break;
  case CIPHER_DESTROY:
    ks->params_main->op_code=soter_asym_cipher_destroy((soter_asym_cipher_t*)param1);
    break;
  case SIGN_DESTROY:
    ks->params_main->op_code=soter_sign_destroy((soter_sign_ctx_t*)param1);
    break;
  case VERIFY_DESTROY:
    ks->params_main->op_code=soter_verify_destroy((soter_verify_ctx_t*)param1);
    break;
  case KA_DERIVE:
    ks->params_main->op_code=soter_asym_ka_derive((soter_asym_ka_t*)param1, param2, ks->params_main->param2_len, res1, &res_len);      
    break;
  case CIPHER_ENCRYPT:
    ks->params_main->op_code=soter_asym_cipher_encrypt((soter_asym_cipher_t*)param1, param2, ks->params_main->param2_len, res1, &res_len);      
    break;
  case CIPHER_DECRYPT:
    ks->params_main->op_code=soter_asym_cipher_decrypt((soter_asym_cipher_t*)param1, param2, ks->params_main->param2_len, res1, &res_len);      
    break;
  case SIGN_UPDATE:
    ks->params_main->op_code=soter_sign_update((soter_sign_ctx_t*)param1, param2, ks->params_main->param2_len);
    break;
  case SIGN_FINAL:
    ks->params_main->op_code=soter_sign_final((soter_sign_ctx_t*)param1, res1, &res_len);      
    break;
  case VERIFY_UPDATE:
    ks->params_main->op_code=soter_verify_update((soter_verify_ctx_t*)param1, param2, ks->params_main->param2_len);
    break;    
  case VERIFY_FINAL:
    ks->params_main->op_code=soter_verify_final((soter_verify_ctx_t*)param1, param2, ks->params_main->param2_len);      
    break;
  default:
    res = SOTER_FAIL;
  }
  munmap(param1, ks->params_main->param1_len+ks->params_main->param2_len+ks->params_main->param3_len);
  munmap(res1, res_len);
  sem_post(ks->sem);
  return SOTER_SUCCESS;
}
