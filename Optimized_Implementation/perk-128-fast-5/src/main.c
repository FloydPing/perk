
/**
 * @file main.c
 * @brief NIST api test
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/random.h>
#include "api.h"
#include "parameters.h"
#include "rng.h"

static void init_randomness(void) {
    unsigned char seed[48] = {0};

#ifndef VERBOSE
    if (0 != getentropy(seed, sizeof(seed))) {
        printf("failed to get entropy for randombytes()\n");
        exit(1);
    }
#endif

    randombytes_init(seed, NULL, 256);
}


static int compare_double(const void* a, const void* b) { return *(uint64_t*)a > *(uint64_t*)b; }

inline static
unsigned long long cycles(void) {
    unsigned long long result;
    __asm__ volatile(".byte 15;.byte 49;shlq $32,%%rdx;orq %%rdx,%%rax"
            : "=a"(result)::"%rdx");
    return result;
}

int main(void) {
    init_randomness();

    printf("\n");
    printf("*****************************\n");
    printf("**** %s-%d ****\n", CRYPTO_ALGNAME, SECURITY_BYTES * 8);
    printf("*****************************\n");

    printf("\n");
    printf("n: %d   ", PARAM_N1);
    printf("m: %d   ", PARAM_M);
    printf("Q: %d   ", PARAM_Q);
    printf("N: %d   ", PARAM_N);
    printf("tau: %d   ", PARAM_TAU);
    printf("Sec: %d bits   ", SECURITY_BYTES * 8);
    printf("Public key size: %d   ", CRYPTO_PUBLICKEYBYTES);
    printf("Private key size: %d   ", CRYPTO_SECRETKEYBYTES);
    printf("Signature size: %d   ", CRYPTO_BYTES);
    printf("\n");

    unsigned char pk[CRYPTO_PUBLICKEYBYTES];
    unsigned char sk[CRYPTO_SECRETKEYBYTES];

    unsigned char m[32] = {0};
    unsigned long long mlen = sizeof(m);

    unsigned long long smlen;
    unsigned char sm[sizeof(m) + CRYPTO_BYTES];

    int ret;

    crypto_sign_keypair(pk, sk);
    crypto_sign(sm, &smlen, m, sizeof(m), sk);
    ret = crypto_sign_open(m, &mlen, sm, smlen, pk);
    if (ret == 0) {
        printf("\nDone");
    } else {
        printf("\nFailed");
    }

    printf("\n\n");


  printf("Making an histogram...........:\n");
  uint64_t num_sigs = 1024;
  uint64_t* tsig = malloc(num_sigs * sizeof(uint64_t));
  uint64_t* tver = malloc(num_sigs * sizeof(uint64_t));
  for (uint64_t i = 0; i < num_sigs; i++) {
    uint64_t t1 = cycles();
    crypto_sign(sm, &smlen, m, sizeof(m), sk);
    uint64_t t2 = cycles();
    ret = crypto_sign_open(m, &mlen, sm, smlen, pk);
    uint64_t t3 = cycles();

    if (ret) { 
        printf("error\n");
        exit(1);
    }
    tsig[i] = (t2 - t1);
    tver[i] = (t3 - t2);
  }
  qsort(tsig, num_sigs, 8, compare_double);
  qsort(tver, num_sigs, 8, compare_double);
  printf("percentile      sig_time     ver_time\n");
  for (uint64_t i = 0; i <= 100; i += 10) {
    // printf("%3" PRId64 "%%            %.6lf     %.6lf\n", i, tsig[i * 2], tver[i * 2]);
    printf("%" PRId64 "%%            %" PRId64 "     %" PRId64 "\n", i, tsig[i * 2], tver[i * 2]);
  }


    return ret;
}
