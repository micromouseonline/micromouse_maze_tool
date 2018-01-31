/***********************************************************************
 * From: https://github.com/jasinb/sha1
 *
 * A simple implementation of the SHA-1 hashing algorithm as per RFC 3174, in
 * C, mainly for learning purposes.
 *
 **************************************************************************/

#pragma once

#include <stdint.h>

// don't just forward declare as we want to pass it around by value
typedef struct _Sha1Digest
{
  uint32_t digest[5];
} Sha1Digest;
Sha1Digest Sha1Digest_fromStr (const char* src);
void       Sha1Digest_toStr   (const Sha1Digest* digest, char* dst);

// Streamable hashing
typedef struct _Sha1Ctx Sha1Ctx;
Sha1Ctx*   Sha1Ctx_create    (void);
void       Sha1Ctx_reset     (Sha1Ctx*);
void       Sha1Ctx_write     (Sha1Ctx*, const void* msg, uint64_t bytes);
Sha1Digest Sha1Ctx_getDigest (Sha1Ctx*);
void       Sha1Ctx_release   (Sha1Ctx*);

// Helper for one-off buffer hashing
Sha1Digest Sha1_get (const void* msg, uint64_t bytes);

int runTests();
