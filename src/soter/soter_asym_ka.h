/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef SOTER_ASYM_KA_H
#define SOTER_ASYM_KA_H

#include <soter/soter.h>

enum soter_asym_ka_alg_type
{
	SOTER_ASYM_KA_EC_P256
};

typedef enum soter_asym_ka_alg_type soter_asym_ka_alg_t;

typedef struct soter_asym_ka_type soter_asym_ka_t;

soter_asym_ka_t* soter_asym_ka_create(soter_asym_ka_alg_t alg);
soter_status_t soter_asym_ka_gen_key(soter_asym_ka_t* asym_ka_ctx);
soter_status_t soter_asym_ka_export_key(soter_asym_ka_t* asym_ka_ctx, void* key, size_t* key_length, bool isprivate);
soter_status_t soter_asym_ka_import_key(soter_asym_ka_t* asym_ka_ctx, const void* key, size_t key_length);
soter_status_t soter_asym_ka_derive(soter_asym_ka_t* asym_ka_ctx, const void* peer_key, size_t peer_key_length, void *shared_secret, size_t* shared_secret_length);
soter_status_t soter_asym_ka_destroy(soter_asym_ka_t* asym_ka_ctx);

#endif /* SOTER_ASYM_KA_H */
