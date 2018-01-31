/***********************************************************************
 * From: https://github.com/jasinb/sha1
 *
 * A simple implementation of the SHA-1 hashing algorithm as per RFC 3174, in
 * C, mainly for learning purposes.
 *
 **************************************************************************/



#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sha1.h"


static uint32_t rotl32(uint32_t x, int b)
{
  return (x << b) | (x >> (32-b));
}

// switch endianness
static uint32_t get32 (const void* p)
{
  const uint8_t *x = (const uint8_t*)p;
  return (x[0] << 24) | (x[1] << 16) | (x[2] << 8) | x[3];
}

static uint32_t f (int t, uint32_t b, uint32_t c, uint32_t d)
{
  assert(0 <= t && t < 80);

  if (t < 20)
    return (b & c) | ((~b) & d);
  if (t < 40)
    return b ^ c ^ d;
  if (t < 60)
    return (b & c) | (b & d) | (c & d);
  //if (t < 80)
  return b ^ c ^ d;
}

struct _Sha1Ctx
{
  uint8_t block[64];
  uint32_t h[5];
  uint64_t bytes;
  uint32_t cur;
};

void Sha1Ctx_reset (Sha1Ctx* ctx)
{
  ctx->h[0] = 0x67452301;
  ctx->h[1] = 0xefcdab89;
  ctx->h[2] = 0x98badcfe;
  ctx->h[3] = 0x10325476;
  ctx->h[4] = 0xc3d2e1f0;
  ctx->bytes = 0;
  ctx->cur = 0;
}

Sha1Ctx* Sha1Ctx_create (void)
{
  // TODO custom allocator support
  Sha1Ctx* ctx = (Sha1Ctx*)malloc(sizeof(Sha1Ctx));
  Sha1Ctx_reset(ctx);
  return ctx;
}

void Sha1Ctx_release (Sha1Ctx* ctx)
{
  free(ctx);
}

static void processBlock (Sha1Ctx* ctx)
{
  static const uint32_t k[4] =
      {
          0x5A827999,
          0x6ED9EBA1,
          0x8F1BBCDC,
          0xCA62C1D6
      };

  uint32_t w[16];
  uint32_t a = ctx->h[0];
  uint32_t b = ctx->h[1];
  uint32_t c = ctx->h[2];
  uint32_t d = ctx->h[3];
  uint32_t e = ctx->h[4];
  int t;

  for (t = 0; t < 16; t++)
    w[t] = get32(&((uint32_t*)ctx->block)[t]);

  for (t = 0; t < 80; t++)
  {
    int s = t & 0xf;
    uint32_t temp;
    if (t >= 16)
      w[s] = rotl32(w[(s + 13) & 0xf] ^ w[(s + 8) & 0xf] ^ w[(s + 2) & 0xf] ^ w[s], 1);

    temp = rotl32(a, 5) + f(t, b,c,d) + e + w[s] + k[t/20];

    e = d; d = c; c = rotl32(b, 30); b = a; a = temp;
  }

  ctx->h[0] += a;
  ctx->h[1] += b;
  ctx->h[2] += c;
  ctx->h[3] += d;
  ctx->h[4] += e;
}

void Sha1Ctx_write (Sha1Ctx* ctx, const void* msg, uint64_t bytes)
{
  ctx->bytes += bytes;

  const uint8_t* src = (const uint8_t *)msg;
  while (bytes--)
  {
    // TODO: could optimize the first and last few bytes, and then copy
    // 128 bit blocks with SIMD in between
    ctx->block[ctx->cur++] = *src++;
    if (ctx->cur == 64)
    {
      processBlock(ctx);
      ctx->cur = 0;
    }
  }
}

Sha1Digest Sha1Ctx_getDigest (Sha1Ctx* ctx)
{
  // append separator
  ctx->block[ctx->cur++] = 0x80;
  if (ctx->cur > 56)
  {
    // no space in block for the 64-bit message length, flush
    memset(&ctx->block[ctx->cur], 0, 64 - ctx->cur);
    processBlock(ctx);
    ctx->cur = 0;
  }

  memset(&ctx->block[ctx->cur], 0, 56 - ctx->cur);
  uint64_t bits = ctx->bytes * 8;

  // TODO a few instructions could be shaven
  ctx->block[56] = (uint8_t)(bits >> 56 & 0xff);
  ctx->block[57] = (uint8_t)(bits >> 48 & 0xff);
  ctx->block[58] = (uint8_t)(bits >> 40 & 0xff);
  ctx->block[59] = (uint8_t)(bits >> 32 & 0xff);
  ctx->block[60] = (uint8_t)(bits >> 24 & 0xff);
  ctx->block[61] = (uint8_t)(bits >> 16 & 0xff);
  ctx->block[62] = (uint8_t)(bits >> 8  & 0xff);
  ctx->block[63] = (uint8_t)(bits >> 0  & 0xff);
  processBlock(ctx);

  {
    Sha1Digest ret;
    int i;
    for (i = 0; i < 5; i++)
      ret.digest[i] = get32(&ctx->h[i]);
    Sha1Ctx_reset(ctx);
    return ret;
  }
}

Sha1Digest Sha1_get (const void* msg, uint64_t bytes)
{
  Sha1Ctx ctx;
  Sha1Ctx_reset(&ctx);
  Sha1Ctx_write(&ctx, msg, bytes);
  return Sha1Ctx_getDigest(&ctx);
}


Sha1Digest Sha1Digest_fromStr (const char* src)
{
  Sha1Digest d;
  int i;

  assert(src); // also, src must be at least 40 bytes
  for (i = 0; i < 20 && src[i]; i++)
  {
    // \todo just use atoi or something
    int c0 = tolower(*src++);
    int c1 = tolower(*src++);

    c0 = '0' <= c0 && c0 <= '9' ? c0 - '0' : ('a' <= c0 && c0 <= 'f' ? 0xa + c0 - 'a' : -1);
    c1 = '0' <= c1 && c1 <= '9' ? c1 - '0' : ('a' <= c1 && c1 <= 'f' ? 0xa + c1 - 'a' : -1);
    ((uint8_t*)d.digest)[i] = (uint8_t)((c0 << 4) | c1);
  }

  return d;
}

void Sha1Digest_toStr (const Sha1Digest* digest, char* dst)
{
  int i;
  assert(digest && dst); // dst must be at least 41 bytes (terminator)
  for (i = 0; i < 20; i++)
  {
    int c0 = ((uint8_t*)digest->digest)[i] >> 4;
    int c1 = ((uint8_t*)digest->digest)[i] & 0xf;

    assert(0 <= c0 && c0 <= 0xf);
    assert(0 <= c1 && c1 <= 0xf);
    c0 = c0 <= 9 ? '0' + c0 : 'a' + c0 - 0xa;
    c1 = c1 <= 9 ? '0' + c1 : 'a' + c1 - 0xa;

    *dst++ = (char)c0;
    *dst++ = (char)c1;
  }
  *dst = '\0';
}


#define LENGTH_OF_ARRAY(x) (sizeof(x)/sizeof((x)[0]))

int runTests (void)
{
  typedef struct _TestVec
  {
    const char* src;
    const char* dst;
  } TestVec;

  const TestVec tests[] =
      {
          // empty string
          {
              "",
              "da39a3ee5e6b4b0d3255bfef95601890afd80709"
          },
          // simple strings
          {
              "abc",
              "a9993e364706816aba3e25717850c26c9cd0d89d"
          },
          {
              "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
              "84983e441c3bd26ebaae4aa1f95129e5e54670f1"
          },
          {   // From wikipedia
              "The quick brown fox jumps over the lazy dog",
              "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12"
          },
          {
              "The quick brown fox jumps over the lazy cog",
              "de9f2c7fd25e1b3afad3e85a0bd17d9b100db4b3"
          },

          // exactly 512 bits
          {
              "0123456701234567012345670123456701234567012345670123456701234567",
              "e0c094e867ef46c350ef54a7f59dd60bed92ae83"
          },
          // exceeding 512 bits
          {
              "Jaska ajaa allaskaljaa. Jaskalla jalalla kaljaa. Kassalla jalka, jalalla kassa. Lakas kalja.",
              "9cd84ad78816c6c39fbed822ae8188fd8e6afd11"
          },
          // multiple of 512
          {
              "hoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAX"
                  "hoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAX"
                  "hoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAX"
                  "hoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAX",
              "5350efaf647d6b227d235e1263007e957f3151f4"
          }
      };

  int i, failCount;
  for (i = 0, failCount = 0; i < LENGTH_OF_ARRAY(tests); i++)
  {
    const TestVec* test = &tests[i];
    Sha1Digest computed = Sha1_get(test->src, strlen(test->src));
    Sha1Digest expected = Sha1Digest_fromStr(test->dst);

    printf("Testing %d/%zu...", i+1, LENGTH_OF_ARRAY(tests));

    if (memcmp(&computed, &expected, sizeof(Sha1Digest)))
    {
      char cStr[41];  // two bytes per digit plus terminator
      printf("failed!\n");
      Sha1Digest_toStr(&computed, cStr);
      printf("Expected %s, got %s\n", test->dst, cStr);
      failCount++;
    }
    else
      printf("success.\n");
  }

  return failCount;
}
