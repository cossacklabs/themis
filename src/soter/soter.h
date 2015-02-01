/**
 * @file
 *
 * (c) CossackLabs
 */

#ifndef SOTER_H
#define SOTER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/**
 * @brief Function return codes
 *
 * TODO: Should define detailed error codes. What codes will we use for errors? It would be good to make them consistent with errno.h where applicable.
 */

typedef int soter_status_t;

#include <soter/soter_rand.h>
#include <soter/soter_hash.h>
#include <soter/soter_hmac.h>
#include <soter/soter_sym.h>
#include <soter/soter_asym_cipher.h>
#include <soter/soter_asym_ka.h>
#include <soter/soter_asym_sign.h>
#include <soter/soter_kdf.h>

#endif /* SOTER_H */
