/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef THEMIS2_H
#define THEMIS2_H

typedef int themis_status_t; 

typedef themis_message_type themis_message_t;
themis_message_t* themis_message_create(const void* message, const size_t message_length);
themis_status_t* themis_message_gen_key_pair(void* private_key, size_t* private_key_length, void* public_key, size_t* public_key_length);
themis_status_t themis_message_authorise(themis_message_t* message, const void* private_key, const size_t private_key_length, void* authorised_message, size_t* authorised_message_length);
themis_status_t themis_message_verify(themis_message_t* message, const void* public_key, const size_t public_key_length, void* verified_message, size_t* verified_message_length);
themis_status_t themis_message_destroy(themis_message_t* message);

/* sample
** generate keys
    themis_message_t* message=themis_message_create(message,message_length);
    themis_message_gen_key_pair(private_key, &private_key_length, public_key, &public_key_length);
    themis_message_destroy(message);

** authorise message
    themis_message_t* message=themis_message_create(message,message_length);
    themis_message_authorise(message, private_key, private_key_length, authorised_message, &authorised_message_length);
    themis_message_destroy(message);

** verify message
    themis_message_t* message=themis_message_create(message,message_length);
    themis_message_verify(message, ppublic_key, public_key_length, verified_message, &verified_message_length);
    themis_message_destroy(message);

*/
#endif

