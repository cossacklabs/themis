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

#include <themis/themis.h>

#include <string.h>
#include <assert.h>

typedef struct client_info_type client_info_t;

struct client_info_type
{
  const char *id;

  uint8_t *priv;
  size_t priv_length;

  uint8_t *pub;
  size_t pub_length;

  secure_session_t* session;
  secure_session_user_callbacks_t transport;
};

static client_info_t client = {"client", NULL, 0, NULL, 0, NULL, {0}};
static client_info_t server = {"server", NULL, 0, NULL, 0, NULL, {0}};

int init_client_info(client_info_t *c){
  if(THEMIS_BUFFER_TOO_SMALL!=themis_gen_key_pair(THEMIS_SECURE_SIGNED_MESSAGE_DEFAULT_ALG, NULL, &(c->priv_length), NULL, &(c->pub_length))){
    return 2;
  }
  c->priv = malloc(c->priv_length);
  assert(c->priv);
  c->pub = malloc(c->pub_length);
  assert(c->pub);

  if(THEMIS_SUCCESS!=themis_gen_key_pair(THEMIS_SECURE_SIGNED_MESSAGE_DEFAULT_ALG, c->priv, &(c->priv_length), c->pub, &(c->pub_length))){
    free(c->priv);
    free(c->pub);
    return 3;
  }
  return 0;
}

void clean_client_info(client_info_t *c){
  free(c->priv);
  free(c->pub);
}

static int on_get_public_key(const void *id, size_t id_length, void *key_buffer, size_t key_buffer_length, void *user_data)
{
  client_info_t *info = user_data;
  client_info_t *peer;

  if (info == &client)
    {
      /* The client should request server's public key */
      peer = &server;
    }
  else if (info == &server)
    {
      /* The server should request client's public key */
      peer = &client;
    }
  else
    {
      return -1;
    }

  if (memcmp(peer->id, id, id_length))
    {
      return -1;
    }

  if (peer->pub_length > key_buffer_length)
    {
      return -1;
    }

  memcpy(key_buffer, peer->pub, peer->pub_length);
  return 0;
}

static secure_session_user_callbacks_t transport =
  {
    NULL,
    NULL,
    NULL,
    on_get_public_key,
    NULL
  };


int main(int argc, char* argv[]){
  if(argc != 2){
    return 1;
  }

  if(0!=init_client_info(&client)){
    return 2;
  }
  if(0!=init_client_info(&server)){
    clean_client_info(&client);
    return 3;
  }

  memcpy(&(client.transport), &transport, sizeof(secure_session_user_callbacks_t));
  client.transport.user_data = &client;

  memcpy(&(server.transport), &transport, sizeof(secure_session_user_callbacks_t));
  server.transport.user_data = &server;

  client.session = secure_session_create(client.id, strlen(client.id), client.priv, client.priv_length, &(client.transport));
  if (!(client.session)){
    clean_client_info(&client);
    clean_client_info(&server);
      return 4;
  }

  server.session = secure_session_create(server.id, strlen(server.id), server.priv, server.priv_length, &(server.transport));
  if (!(server.session)){
      secure_session_destroy((client.session));
      clean_client_info(&client);
      clean_client_info(&server);
      return 5;
  }

  uint8_t cdata[20480];
  size_t cdata_length = sizeof(cdata);

  if(THEMIS_SUCCESS!=secure_session_generate_connect_request(client.session, cdata, &cdata_length)){
    secure_session_destroy(client.session);
    secure_session_destroy(server.session);
    clean_client_info(&client);
    clean_client_info(&server);
    return 6;
  }

  uint8_t sdata[20480];
  size_t sdata_length = sizeof(sdata);

  int res=secure_session_unwrap(server.session, cdata, cdata_length, sdata, &sdata_length);
  if(THEMIS_SSESSION_SEND_OUTPUT_TO_PEER != res){
    secure_session_destroy(client.session);
    secure_session_destroy(server.session);
    clean_client_info(&client);
    clean_client_info(&server);
    return res;
  }

  if(THEMIS_SSESSION_SEND_OUTPUT_TO_PEER != secure_session_unwrap(client.session, sdata, sdata_length, cdata, &cdata_length)){
    secure_session_destroy(client.session);
    secure_session_destroy(server.session);
    clean_client_info(&client);
    clean_client_info(&server);
    return 8;
  }

  if(THEMIS_SSESSION_SEND_OUTPUT_TO_PEER != secure_session_unwrap(server.session, cdata, cdata_length, sdata, &sdata_length)){
    secure_session_destroy(client.session);
    secure_session_destroy(server.session);
    clean_client_info(&client);
    clean_client_info(&server);
    return 9;
  }

  if(THEMIS_SUCCESS != secure_session_unwrap(client.session, sdata, sdata_length, cdata, &cdata_length) && 0==cdata_length){
    secure_session_destroy(client.session);
    secure_session_destroy(server.session);
    clean_client_info(&client);
    clean_client_info(&server);
    return 10;
  }


  size_t data_length = atoi(argv[1]);
  uint8_t* data=malloc(data_length);
  assert(data);

  if(THEMIS_SUCCESS!=soter_rand(data, data_length) ){
    secure_session_destroy(client.session);
    secure_session_destroy(server.session);
    clean_client_info(&client);
    clean_client_info(&server);
    free(data);
    return 11;
  }
  size_t enc_data_length=0;
  if(THEMIS_BUFFER_TOO_SMALL != secure_session_wrap(client.session, data, data_length, NULL, &enc_data_length)){
    secure_session_destroy(client.session);
    secure_session_destroy(server.session);
    clean_client_info(&client);
    clean_client_info(&server);
    free(data);
    return 12;
  }
  uint8_t* enc_data=malloc(enc_data_length);
  assert(enc_data);
  if(THEMIS_SUCCESS != secure_session_wrap(client.session, data, data_length, enc_data, &enc_data_length)){
    secure_session_destroy(client.session);
    secure_session_destroy(server.session);
    clean_client_info(&client);
    clean_client_info(&server);
    free(data);
    free(enc_data);
    return 13;
  }
  free(data);
  if(THEMIS_BUFFER_TOO_SMALL != secure_session_unwrap(server.session, enc_data, enc_data_length, NULL, &data_length)){
    secure_session_destroy(client.session);
    secure_session_destroy(server.session);
    clean_client_info(&client);
    clean_client_info(&server);
    free(enc_data);
    return 14;
  }
  data=malloc(data_length);
  assert(data);
  if(THEMIS_SUCCESS != secure_session_unwrap(server.session, enc_data, enc_data_length, data, &data_length)){
    secure_session_destroy(client.session);
    secure_session_destroy(server.session);
    clean_client_info(&client);
    clean_client_info(&server);
    free(enc_data);
    free(data);
    return 15;
  }
  free(enc_data);

  if(THEMIS_BUFFER_TOO_SMALL != secure_session_wrap(server.session, data, data_length, NULL, &enc_data_length)){
    secure_session_destroy(client.session);
    secure_session_destroy(server.session);
    clean_client_info(&client);
    clean_client_info(&server);
    free(data);
    return 12;
  }
  enc_data=malloc(enc_data_length);
  assert(enc_data);
  if(THEMIS_SUCCESS != secure_session_wrap(server.session, data, data_length, enc_data, &enc_data_length)){
    secure_session_destroy(client.session);
    secure_session_destroy(server.session);
    clean_client_info(&client);
    clean_client_info(&server);
    free(data);
    free(enc_data);
    return 13;
  }
  free(data);
  if(THEMIS_BUFFER_TOO_SMALL != secure_session_unwrap(client.session, enc_data, enc_data_length, NULL, &data_length)){
    secure_session_destroy(client.session);
    secure_session_destroy(server.session);
    clean_client_info(&client);
    clean_client_info(&server);
    free(enc_data);
    return 14;
  }
  data=malloc(data_length);
  assert(data);
  if(THEMIS_SUCCESS != secure_session_unwrap(client.session, enc_data, enc_data_length, data, &data_length)){
    secure_session_destroy(client.session);
    secure_session_destroy(server.session);
    clean_client_info(&client);
    clean_client_info(&server);
    free(enc_data);
    free(data);
    return 15;
  }


  
  

  
  
  

  

  secure_session_destroy(client.session);
  secure_session_destroy(server.session);
  clean_client_info(&client);
  clean_client_info(&server);

  return 0;
}

