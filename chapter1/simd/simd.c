typedef unsigned int u32;
#include <intrin.h>
#include <immintrin.h>
#include <smmintrin.h>
#include <wmmintrin.h>
#include "avx2intrin.h"
#include "avxintrin.h"

u32 SingleSSE(u32 count, u32 *input) {
    __m128i sum = _mm_setzero_si128();
    for (u32 index = 0; index < count; index += 4) {
        sum = _mm_add_epi32(sum, _mm_load_si128((__m128i *)&input[index]));
    }
    sum = _mm_hadd_epi32(sum, sum);
    sum = _mm_hadd_epi32(sum, sum);

    return _mm_cvtsi128_si32(sum);
}