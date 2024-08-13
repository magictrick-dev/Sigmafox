#include <core/memops.h>
#include <intrin.h>
#include <immintrin.h>
#include <emmintrin.h>

void 
memory_copy_simple(void *dest, const void *source, u64 size)
{

    for (u64 i = 0; i < size; ++i)
    {

        u8* dst_loc = (u8*)dest + i;
        u8* src_loc = (u8*)source + i;
        *dst_loc = *src_loc;

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

        u8* dst_loc = (u8*)dest + i;
        u8* src_loc = (u8*)source + i;
        *dst_loc = *src_loc;

    }

    assert(i == size);

}

void 
memory_set_zero_simple(void *dest, u64 size)
{

    for (u64 i = 0; i < size; ++i)
    {
        u8* loc = (u8*)dest + i;
        *loc = 0x00;
    }

}

void 
memory_set_zero_ext(void *dest, u64 size)
{

    __m256 zero_value = _mm256_setzero_ps();

    u64 remainder = size % 128;
    u64 i;
    for (i = 0; i < size; i += 128)
    {

        float *dest_a = (float*)((__m256*)dest + 0);
        float *dest_b = (float*)((__m256*)dest + 1);
        float *dest_c = (float*)((__m256*)dest + 2);
        float *dest_d = (float*)((__m256*)dest + 3);

        _mm256_store_ps(dest_a, zero_value);
        _mm256_store_ps(dest_b, zero_value);
        _mm256_store_ps(dest_c, zero_value);
        _mm256_store_ps(dest_d, zero_value);

    }


    // Remaining bytes.
    for (; i < size; ++i)
    {
        u8* loc = (u8*)dest + i;
        *loc = 0x00;
    }

    assert(i == size);
    

}

void 
memory_set_byte_simple(void *dest, u8 byte, u64 size)
{

    for (u64 i = 0; i < size; ++i)
    {
        u8* loc = (u8*)dest + i;
        *loc = byte;
    }

}

