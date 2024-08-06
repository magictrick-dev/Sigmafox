#include <core/memops.h>
#include <intrin.h>
#include <immintrin.h>
#include <emmintrin.h>

void 
memory_copy_simple(void *dest, const void *source, u64 size)
{

    for (u64 i = 0; i < size; ++i)
    {
        *((u8*)dest + i) = *((u8*)source + i);
    }

}

void 
memory_copy_ext(void *dest, const void *source, u64 size)
{

    u64 remainder = size % 128;
    u64 i;
    for (i = 0; i < size; i += 128)
    {
        __m256i *dest_a = ((__m256i*)dest) + 0;
        __m256i *dest_b = ((__m256i*)dest) + 1;
        __m256i *dest_c = ((__m256i*)dest) + 2;
        __m256i *dest_d = ((__m256i*)dest) + 3;
        _mm256_storeu_si256(dest_a, *(((__m256i*)source) + 0));
        _mm256_storeu_si256(dest_b, *(((__m256i*)source) + 1));
        _mm256_storeu_si256(dest_c, *(((__m256i*)source) + 2));
        _mm256_storeu_si256(dest_d, *(((__m256i*)source) + 3));
    }

    for (; i < size; ++i)
    {
        *((u8*)dest + i) = *((u8*)source + i);
    }

    assert(i == size);

}

