/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef THEMIS_H
#define THEMIS_H

typedef int themis_status_t; 

typedef themis_authorized_message_type themis_authorized_ctx_t;
themis_authorised_ctx_t* themis_authorised_ctx_create(const void* private_key, const size_t private_key_length);
themis_status_t themis_authorised_ctx_proceed_message(themis_authorized_ctx_t* ctx, const void* message, const size_t message_length, void* authorised_message);
themis_status_t themis_authorised_ctx_export_key(themis_authorised_ctx_t* ctx, void* private_key, size_t* private_key_length, void* public_key, size_t* public_key_length);
themis_status_t themis_authorised_ctx_destroy(themis_authorised_ctx_t* ctx);

typedef themis_verified_message_type themis_verified_ctx_t;
themis_verified_ctx_t* themis_verified_ctx_create(const void* public_key, const size_t public_key_length);
themis_status_t themis_verified_ctx_proceed_message(themis_authorized_ctx_t* ctx, const void* authorised_message, void* message, size_t* message_length);
themis_status_t themis_verified_ctx_destroy(themis_authorised_ctx_t* ctx);


/* sample
** generate keys
    themis_authorised_ctx_t* ctx=themis_authorised_ctx_create(NULL,0);
    themis_authorised_ctx_export_key(ctx, private_key, &private_key_length, public_key, &public_key_length);
    themis_authorised_ctx_destroy(ctx);

** authorise message
    themis_authorised_ctx_t* ctx=themis_authorised_ctx_create(private_key,private_key_length);
    themis_authorised_ctx_proceed_message(ctx, message, message_length, authorised_message, &authorised_message_length);
    themis_authorised_ctx_destroy(ctx);

** verify message
    themis_verified_ctx_t* ctx=themis_verified_ctx_create(public_key_key,public_key_length);
    themis_verified_ctx_proceed_message(ctx, authorised_message, &authorised_message_length, message, message_length, );
    themis_authorised_ctx_destroy(ctx);

*/
#endif

