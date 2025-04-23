#ifndef SIGAMFOX_LIBRARY_DVECTOR_HPP
#define SIGAMFOX_LIBRARY_DVECTOR_HPP
#include <array>
#include <vector>
#include <cstdint>
#include <cmath>
#include <limits>
#include <iostream>
#include <initializer_list>

#if 1
#   if defined(SF_USE_INTRINSICS) && SF_USE_INTRINSICS == 1
#       include <xmmintrin.h>
#       include <emmintrin.h>
#       pragma message("Using SSE intrinsics.")
#       if defined(__AVX__) && SF_USE_AVX == 1
#           include <immintrin.h>
#           pragma message("Using AVX intrinsics.")
#           define SF_ENABLE_AVX 1
#       else
#           define SF_ENABLE_AVX 0
#       endif
#   endif
#endif

template <class T, size_t L>
class dvector
{

    public:
        inline          dvector();
        inline          dvector(const dvector<T,L> &vec);
        inline          dvector(const dvector<T,L> &&vec);
        inline          dvector(std::initializer_list<T> list);
        virtual inline ~dvector();

        inline T&       operator[](const size_t index);
        inline const T& operator[](const size_t index) const;

        inline T&       at(const size_t index);
        inline const T& at(const size_t index) const;

        inline size_t   size() const;

        inline bool     contains_nans() const;
        inline bool     contains_inf() const;
        inline bool     contains_positive_inf() const;
        inline bool     contains_negative_inf() const;
        inline bool     contains_valid_components() const;

        inline bool     valid_at(const size_t index) const;
        inline bool     valid_in_range(const size_t start, const size_t end) const;

        inline dvector& operator+=(double rhs);
        inline dvector& operator-=(double rhs);
        inline dvector& operator*=(double rhs);
        inline dvector& operator/=(double rhs);

        inline dvector& operator+=(const dvector<T,L> &rhs);
        inline dvector& operator-=(const dvector<T,L> &rhs);
        inline dvector& operator*=(const dvector<T,L> &rhs);
        inline dvector& operator/=(const dvector<T,L> &rhs);

        inline bool     operator==(const dvector<T,L> &rhs) const;
        inline bool     operator!=(const dvector<T,L> &rhs) const;

        inline dvector& component_wise_addition(double value);
        inline dvector& component_wise_subtraction(double value);
        inline dvector& component_wise_multiplication(double value);
        inline dvector& component_wise_division(double value);

        inline dvector& component_wise_addition(const dvector<T,L> &vector);
        inline dvector& component_wise_subtraction(const dvector<T,L> &vector);
        inline dvector& component_wise_multiplication(const dvector<T,L> &vector);
        inline dvector& component_wise_division(const dvector<T,L> &vector);

        inline bool     component_wise_compare(const dvector& rhs) const;

    protected:
        std::vector<T> components;

    protected:
        static constexpr size_t alignment = 4;

};

template <class T, size_t L> inline std::ostream& operator<<(std::ostream& os, const dvector<T,L> &rhs);

template <class T, size_t L> inline dvector<T,L>    operator+(double lhs, const dvector<T,L> &rhs);
template <class T, size_t L> inline dvector<T,L>    operator-(double lhs, const dvector<T,L> &rhs);
template <class T, size_t L> inline dvector<T,L>    operator*(double lhs, const dvector<T,L> &rhs);
template <class T, size_t L> inline dvector<T,L>    operator/(double lhs, const dvector<T,L> &rhs);

template <class T, size_t L> inline dvector<T,L>    operator+(const dvector<T,L> &lhs, double rhs);
template <class T, size_t L> inline dvector<T,L>    operator-(const dvector<T,L> &lhs, double rhs);
template <class T, size_t L> inline dvector<T,L>    operator*(const dvector<T,L> &lhs, double rhs);
template <class T, size_t L> inline dvector<T,L>    operator/(const dvector<T,L> &lhs, double rhs);

template <class T, size_t L> inline dvector<T,L>    operator+(const dvector<T,L> &lhs, const dvector<T,L> &rhs);
template <class T, size_t L> inline dvector<T,L>    operator-(const dvector<T,L> &lhs, const dvector<T,L> &rhs);
template <class T, size_t L> inline dvector<T,L>    operator*(const dvector<T,L> &lhs, const dvector<T,L> &rhs);
template <class T, size_t L> inline dvector<T,L>    operator/(const dvector<T,L> &lhs, const dvector<T,L> &rhs);

template <class T, size_t L> inline dvector<T,L>::         
dvector()
{

    constexpr size_t size = (L + dvector::alignment - 1) & ~std::size_t(dvector::alignment - 1);
    this->components.resize(size);

    for (auto c : this->components) 
    {
        c = { };
    }

}

template <class T, size_t L> inline dvector<T,L>::         
dvector(std::initializer_list<T> list)
{

    constexpr size_t size = (L + dvector::alignment - 1) & ~std::size_t(dvector::alignment - 1);
    this->components.resize(size);
    this->components = list;

}

template <class T, size_t L> inline dvector<T,L>::         
dvector(const dvector<T,L> &vec)
{

    constexpr size_t size = (L + dvector::alignment - 1) & ~std::size_t(dvector::alignment - 1);
    this->components.resize(size);
    for (size_t i = 0; i < L; ++i)
    {

        this->components[i] = vec.components[i];

    }

}

template <class T, size_t L> inline dvector<T,L>::
dvector(const dvector<T,L> &&vec)
{

    this->components = std::move(vec.components);

}

template <class T, size_t L> inline dvector<T,L>::
~dvector()
{



}

template <class T, size_t L> inline T& dvector<T,L>::
operator[](const size_t index)
{

    T& result = this->components[index];
    return result;

}

template <class T, size_t L> inline const T& dvector<T,L>::
operator[](const size_t index) const
{

    const T& result = this->components[index];
    return result;

}

template <class T, size_t L> inline T& dvector<T,L>::
at(const size_t index)
{

    T& result = this->components[index];
    return result;

}

template <class T, size_t L> inline const T& dvector<T,L>::
at(const size_t index) const
{

    const T& result = this->components[index];
    return result;

}

template <class T, size_t L> inline size_t dvector<T,L>::
size() const
{

    const size_t result = this->components.size();
    return result;

}

template <class T, size_t L> inline bool dvector<T,L>::
contains_nans() const
{

    bool contains_nan = false;
    for (size_t i = 0; i < L; ++i)
    {
        if (std::isnan(this->components[i]))
        {
            contains_nan = true;
            break;
        }
    }

    return contains_nan;

}

template <class T, size_t L> inline bool dvector<T,L>::
contains_inf() const
{

    bool contains_inf = false;
    for (size_t i = 0; i < L; ++i)
    {
        if (std::isinf(this->components[i]))
        {
            contains_inf = true;
            break;
        }
    }

    return contains_inf;

}

template <class T, size_t L> inline bool dvector<T,L>::
contains_positive_inf() const
{

    bool contains_pos_inf = false;
    for (size_t i = 0; i < L; ++i)
    {

        const auto c = this->components[i];
        if (std::isinf(c) && c >= 0)
        {
            contains_pos_inf = true;
            break;
        }
    }

    return contains_pos_inf;

}

template <class T, size_t L> inline bool dvector<T,L>::
contains_negative_inf() const
{

    bool contains_neg_inf = false;
    for (size_t i = 0; i < L; ++i)
    {

        const auto c = this->components[i];
        if (std::isinf(c) && c < 0)
        {
            contains_neg_inf = true;
            break;
        }
    }

    return contains_neg_inf;

}

template <class T, size_t L> inline bool dvector<T,L>::
contains_valid_components() const
{

    bool contains_nans  = this->contains_nans();
    bool contains_inf   = this->contains_inf();

    if (contains_nans || contains_inf) return false;

    bool all_normal = true;
    for (size_t i = 0; i < L; ++i)
    {

        const auto c = this->components[i];

        // Perhaps subnormal?
        if (show_classification(c) != FP_NORMAL)
        {
            all_normal = false;
            break;
        }

    }

    return all_normal;

}

template <class T, size_t L> inline bool dvector<T,L>::
valid_at(const size_t index) const
{

    bool value_is_normal = true;
    if (show_classification(this->components[index]) != FP_NORMAL)
    {

        // Perhaps subnormal?
        value_is_normal = false;

    }

    return value_is_normal;

}

template <class T, size_t L> inline bool dvector<T,L>::
valid_in_range(const size_t start, const size_t end) const
{

    bool values_are_normal = true;
    for (size_t i = start; i < std::min(end, L); ++i)
    {

        if (show_classification(this->components[i]) != FP_NORMAL)
        {

            // Perhaps subnormal?
            values_are_normal= false;
            break;

        }


    }

    return values_are_normal;

}

template <class T, size_t L> inline bool dvector<T,L>::
operator==(const dvector<T,L> &rhs) const
{

    auto result = this->component_wise_compare(rhs);
    return result;

}

template <class T, size_t L> inline bool dvector<T,L>::
operator!=(const dvector<T,L> &rhs) const
{

    auto result = !(this->component_wise_compare(rhs));
    return result;

}

template <class T, size_t L> inline dvector<T,L>& dvector<T,L>::
operator+=(double rhs)
{

    return this->component_wise_addition(rhs);

}

template <class T, size_t L> inline dvector<T,L>& dvector<T,L>::
operator-=(double rhs)
{

    return this->component_wise_subtraction(rhs);

}

template <class T, size_t L> inline dvector<T,L>& dvector<T,L>::
operator*=(double rhs)
{

    return this->component_wise_multiplication(rhs);

}

template <class T, size_t L> inline dvector<T,L>& dvector<T,L>::
operator/=(double rhs)
{

    return this->component_wise_division(rhs);

}

template <class T, size_t L> inline dvector<T,L>& dvector<T,L>::
operator+=(const dvector<T,L> &rhs)
{

    return this->component_wise_addition(rhs);

}

template <class T, size_t L> inline dvector<T,L>& dvector<T,L>::
operator-=(const dvector<T,L> &rhs)
{

    return this->component_wise_subtraction(rhs);

}

template <class T, size_t L> inline dvector<T,L>& dvector<T,L>::
operator*=(const dvector<T,L> &rhs)
{

    return this->component_wise_multiplication(rhs);

}

template <class T, size_t L> inline dvector<T,L>& dvector<T,L>::
operator/=(const dvector<T,L> &rhs)
{

    return this->component_wise_division(rhs);

}

template <class T, size_t L> inline dvector<T,L>& dvector<T,L>::
component_wise_addition(double value)
{

#   if defined(SF_USE_INTRINSICS) && SF_USE_INTRINSICS == 1

        if constexpr (std::is_same_v<T, float>)
        {

#           if SF_ENABLE_AVX
                __m256 v = _mm256_set1_ps(static_cast<float>(value));
                for (size_t i = 0; i < L; i += 8)
                {
                    __m256 c = _mm256_load_ps(&components[i]);
                    c = _mm256_add_ps(c, v);
                    _mm256_store_ps(&components[i], c);
                }
#           else
                __m128 v = _mm_set1_ps(static_cast<float>(value));
                for (size_t i = 0; i < L; i += 4)
                {
                    __m128 c = _mm_load_ps(&components[i]);
                    c = _mm_add_ps(c, v);
                    _mm_store_ps(&components[i], c);
                }
#           endif

        }

        else if constexpr (std::is_same_v<T, double>)
        {

#           if SF_ENABLE_AVX
                __m256d v = _mm256_set1_pd(value);
                for (size_t i = 0; i < L; i += 4)
                {
                    __m256d c = _mm256_load_pd(&components[i]);
                    c = _mm256_add_pd(c, v);
                    _mm256_store_pd(&components[i], c);
                }
#           else
                __m128d v = _mm_set1_pd(value);
                for (size_t i = 0; i < L; i += 2)
                {
                    __m128d c = _mm_load_pd(&components[i]);
                    c = _mm_add_pd(c, v);
                    _mm_store_pd(&components[i], c);
                }
#           endif
        }

#   else

        for (size_t i = 0; i < L; ++i)
        {

            this->components[i] += value;

        }

#   endif


    return *this;

}

template <class T, size_t L> inline dvector<T,L>& dvector<T,L>::
component_wise_subtraction(double value)
{

#   if defined(SF_USE_INTRINSICS) && SF_USE_INTRINSICS == 1

        if constexpr (std::is_same_v<T, float>)
        {

#           if SF_ENABLE_AVX
                __m256 v = _mm256_set1_ps(static_cast<float>(value));
                for (size_t i = 0; i < L; i += 8)
                {
                    __m256 c = _mm256_load_ps(&components[i]);
                    c = _mm256_sub_ps(c, v);
                    _mm256_store_ps(&components[i], c);
                }
#           else
                __m128 v = _mm_set1_ps(static_cast<float>(value));
                for (size_t i = 0; i < L; i += 4)
                {
                    __m128 c = _mm_load_ps(&components[i]);
                    c = _mm_sub_ps(c, v);
                    _mm_store_ps(&components[i], c);
                }
#           endif

        }

        else if constexpr (std::is_same_v<T, double>)
        {

#           if SF_ENABLE_AVX
                __m256d v = _mm256_set1_pd(value);
                for (size_t i = 0; i < L; i += 4)
                {
                    __m256d c = _mm256_load_pd(&components[i]);
                    c = _mm256_sub_pd(c, v);
                    _mm256_store_pd(&components[i], c);
                }
#           else
                __m128d v = _mm_set1_pd(value);
                for (size_t i = 0; i < L; i += 2)
                {
                    __m128d c = _mm_load_pd(&components[i]);
                    c = _mm_sub_pd(c, v);
                    _mm_store_pd(&components[i], c);
                }
#           endif

        }

#   else

        for (size_t i = 0; i < L; ++i)
        {

            this->components[i] -= value;

        }

#   endif

    return *this;

}

template <class T, size_t L> inline dvector<T,L>& dvector<T,L>::
component_wise_multiplication(double value)
{
    
#   if defined(SF_USE_INTRINSICS) && SF_USE_INTRINSICS == 1

        if constexpr (std::is_same_v<T, float>)
        {

#           if SF_ENABLE_AVX
                __m256 v = _mm256_set1_ps(static_cast<float>(value));
                for (size_t i = 0; i < L; i += 8)
                {
                    __m256 c = _mm256_load_ps(&components[i]);
                    c = _mm256_mul_ps(c, v);
                    _mm256_store_ps(&components[i], c);
                }
#           else
                __m128 v = _mm_set1_ps(static_cast<float>(value));
                for (size_t i = 0; i < L; i += 4)
                {
                    __m128 c = _mm_load_ps(&components[i]);
                    c = _mm_mul_ps(c, v);
                    _mm_store_ps(&components[i], c);
                }
#           endif

        }

        else if constexpr (std::is_same_v<T, double>)
        {

#           if SF_ENABLE_AVX
                __m256d v = _mm256_set1_pd(value);
                for (size_t i = 0; i < L; i += 4)
                {
                    __m256d c = _mm256_load_pd(&components[i]);
                    c = _mm256_mul_pd(c, v);
                    _mm256_store_pd(&components[i], c);
                }
#           else
                __m128d v = _mm_set1_pd(value);
                for (size_t i = 0; i < L; i += 2)
                {
                    __m128d c = _mm_load_pd(&components[i]);
                    c = _mm_mul_pd(c, v);
                    _mm_store_pd(&components[i], c);
                }
#           endif

        }

#   else

        for (size_t i = 0; i < L; ++i)
        {

            this->components[i] *= value;

        }

#   endif

    return *this;

}

template <class T, size_t L> inline dvector<T,L>& dvector<T,L>::
component_wise_division(double value)
{

#   if defined(SF_USE_INTRINSICS) && SF_USE_INTRINSICS == 1

        if constexpr (std::is_same_v<T, float>)
        {

#           if SF_ENABLE_AVX
                __m256 v = _mm256_set1_ps(static_cast<float>(value));
                for (size_t i = 0; i < L; i += 8)
                {
                    __m256 c = _mm256_load_ps(&components[i]);
                    c = _mm256_div_ps(c, v);
                    _mm256_store_ps(&components[i], c);
                }
#           else
                __m128 v = _mm_set1_ps(static_cast<float>(value));
                for (size_t i = 0; i < L; i += 4)
                {
                    __m128 c = _mm_load_ps(&components[i]);
                    c = _mm_div_ps(c, v);
                    _mm_store_ps(&components[i], c);
                }
#           endif

        }

        else if constexpr (std::is_same_v<T, double>)
        {

#           if SF_ENABLE_AVX
                __m256d v = _mm256_set1_pd(value);
                for (size_t i = 0; i < L; i += 4)
                {
                    __m256d c = _mm256_load_pd(&components[i]);
                    c = _mm256_div_pd(c, v);
                    _mm256_store_pd(&components[i], c);
                }
#           else
                __m128d v = _mm_set1_pd(value);
                for (size_t i = 0; i < L; i += 2)
                {
                    __m128d c = _mm_load_pd(&components[i]);
                    c = _mm_div_pd(c, v);
                    _mm_store_pd(&components[i], c);
                }
#           endif

        }

#   else

        for (size_t i = 0; i < L; ++i)
        {

            this->components[i] /= value;

        }

#   endif

    return *this;

}


template <class T, size_t L> inline dvector<T,L>& dvector<T,L>::
component_wise_addition(const dvector<T,L> &vector)
{

#   if defined(SF_USE_INTRINSICS) && SF_USE_INTRINSICS == 1

        if constexpr (std::is_same_v<T, float>)
        {

#           if SF_ENABLE_AVX
                for (size_t i = 0; i < L; i += 8)
                {
                    __m256 a = _mm256_load_ps(&this->components[i]);
                    __m256 b = _mm256_load_ps(&vector.components[i]);
                    a = _mm256_add_ps(a, b);
                    _mm256_store_ps(&this->components[i], a);
                }
#           else
                for (size_t i = 0; i < L; i += 4)
                {
                    __m128 a = _mm_load_ps(&this->components[i]);
                    __m128 b = _mm_load_ps(&vector.components[i]);
                    a = _mm_add_ps(a, b);
                    _mm_store_ps(&this->components[i], a);
                }
#           endif

        }

        else if constexpr (std::is_same_v<T, double>)
        {

#           if SF_ENABLE_AVX
                for (size_t i = 0; i < L; i += 4)
                {
                    __m256d a = _mm256_load_pd(&this->components[i]);
                    __m256d b = _mm256_load_pd(&vector.components[i]);
                    a = _mm256_add_pd(a, b);
                    _mm256_store_pd(&this->components[i], a);
                }
#           else
                for (size_t i = 0; i < L; i += 2)
                {
                    __m128d a = _mm_load_pd(&this->components[i]);
                    __m128d b = _mm_load_pd(&vector.components[i]);
                    a = _mm_add_pd(a, b);
                    _mm_store_pd(&this->components[i], a);
                }
#           endif

        }

#   else

        for (size_t index = 0; index < L; ++index)
        {

            this->components[index] += vector.components[index];

        }

#   endif

    return *this;

}

template <class T, size_t L> inline dvector<T,L>& dvector<T,L>::
component_wise_subtraction(const dvector<T,L> &vector)
{

#   if defined(SF_USE_INTRINSICS) && SF_USE_INTRINSICS == 1

        if constexpr (std::is_same_v<T, float>)
        {

#           if SF_ENABLE_AVX
                for (size_t i = 0; i < L; i += 8)
                {
                    __m256 a = _mm256_load_ps(&this->components[i]);
                    __m256 b = _mm256_load_ps(&vector.components[i]);
                    a = _mm256_sub_ps(a, b);
                    _mm256_store_ps(&this->components[i], a);
                }
#           else
                for (size_t i = 0; i < L; i += 4)
                {
                    __m128 a = _mm_load_ps(&this->components[i]);
                    __m128 b = _mm_load_ps(&vector.components[i]);
                    a = _mm_sub_ps(a, b);
                    _mm_store_ps(&this->components[i], a);
                }
#           endif

        }

        else if constexpr (std::is_same_v<T, double>)
        {

#           if SF_ENABLE_AVX
                for (size_t i = 0; i < L; i += 4)
                {
                    __m256d a = _mm256_load_pd(&this->components[i]);
                    __m256d b = _mm256_load_pd(&vector.components[i]);
                    a = _mm256_sub_pd(a, b);
                    _mm256_store_pd(&this->components[i], a);
                }
#           else
                for (size_t i = 0; i < L; i += 2)
                {
                    __m128d a = _mm_load_pd(&this->components[i]);
                    __m128d b = _mm_load_pd(&vector.components[i]);
                    a = _mm_sub_pd(a, b);
                    _mm_store_pd(&this->components[i], a);
                }
#           endif

        }

#   else

        for (size_t index = 0; index < L; ++index)
        {

            this->components[index] -= vector.components[index];

        }

#   endif

    return *this;

}

template <class T, size_t L> inline dvector<T,L>& dvector<T,L>::
component_wise_multiplication(const dvector<T,L> &vector)
{

#   if defined(SF_USE_INTRINSICS) && SF_USE_INTRINSICS == 1

        if constexpr (std::is_same_v<T, float>)
        {

#           if SF_ENABLE_AVX
                for (size_t i = 0; i < L; i += 8)
                {
                    __m256 a = _mm256_load_ps(&this->components[i]);
                    __m256 b = _mm256_load_ps(&vector.components[i]);
                    a = _mm256_mul_ps(a, b);
                    _mm256_store_ps(&this->components[i], a);
                }
#           else
                for (size_t i = 0; i < L; i += 4)
                {
                    __m128 a = _mm_load_ps(&this->components[i]);
                    __m128 b = _mm_load_ps(&vector.components[i]);
                    a = _mm_mul_ps(a, b);
                    _mm_store_ps(&this->components[i], a);
                }
#           endif

        }

        else if constexpr (std::is_same_v<T, double>)
        {

#           if SF_ENABLE_AVX
                for (size_t i = 0; i < L; i += 4)
                {
                    __m256d a = _mm256_load_pd(&this->components[i]);
                    __m256d b = _mm256_load_pd(&vector.components[i]);
                    a = _mm256_mul_pd(a, b);
                    _mm256_store_pd(&this->components[i], a);
                }
#           else
                for (size_t i = 0; i < L; i += 2)
                {
                    __m128d a = _mm_load_pd(&this->components[i]);
                    __m128d b = _mm_load_pd(&vector.components[i]);
                    a = _mm_mul_pd(a, b);
                    _mm_store_pd(&this->components[i], a);
                }
#           endif

        }

#   else

        for (size_t index = 0; index < L; ++index)
        {

            this->components[index] *= vector.components[index];

        }

#   endif

    return *this;

}

template <class T, size_t L> inline dvector<T,L>& dvector<T,L>::
component_wise_division(const dvector<T,L> &vector)
{

#   if defined(SF_USE_INTRINSICS) && SF_USE_INTRINSICS == 1

        if constexpr (std::is_same_v<T, float>)
        {

#           if SF_ENABLE_AVX
                for (size_t i = 0; i < L; i += 8)
                {
                    __m256 a = _mm256_load_ps(&this->components[i]);
                    __m256 b = _mm256_load_ps(&vector.components[i]);
                    a = _mm256_div_ps(a, b);
                    _mm256_store_ps(&this->components[i], a);
                }
#           else
                for (size_t i = 0; i < L; i += 4)
                {
                    __m128 a = _mm_load_ps(&this->components[i]);
                    __m128 b = _mm_load_ps(&vector.components[i]);
                    a = _mm_div_ps(a, b);
                    _mm_store_ps(&this->components[i], a);
                }
#           endif

        }

        else if constexpr (std::is_same_v<T, double>)
        {

#           if SF_ENABLE_AVX
                for (size_t i = 0; i < L; i += 4)
                {
                    __m256d a = _mm256_load_pd(&this->components[i]);
                    __m256d b = _mm256_load_pd(&vector.components[i]);
                    a = _mm256_div_pd(a, b);
                    _mm256_store_pd(&this->components[i], a);
                }
#           else
                for (size_t i = 0; i < L; i += 2)
                {
                    __m128d a = _mm_load_pd(&this->components[i]);
                    __m128d b = _mm_load_pd(&vector.components[i]);
                    a = _mm_div_pd(a, b);
                    _mm_store_pd(&this->components[i], a);
                }
#           endif

        }

#   else

        for (size_t index = 0; index < L; ++index)
        {

            this->components[index] /= vector.components[index];

        }

#   endif

    return *this;

}

template <class T, size_t L> inline bool dvector<T,L>::
component_wise_compare(const dvector<T,L> &vector) const
{

    bool components_equal = true;
    for (size_t i = 0; i < L; ++i)
    {

        if (this->components[i] != vector.components[i])
        {

            components_equal = false;
            break;

        }

    }

    return components_equal;

}

template <class T, size_t L> inline std::ostream& 
operator<<(std::ostream& os, const dvector<T,L> &rhs)
{

    os << "[";
    for (size_t i = 0; i < L; ++i)
    {
        os << rhs[i];
        if (i < L - 1) os << ", ";
    }
    os << "]";

    return os;

}

template <class T, size_t L> inline dvector<T,L>    
operator+(double lhs, const dvector<T,L> &rhs)
{

    dvector<T,L> result = rhs;
    result += lhs;
    return result;

}

template <class T, size_t L> inline dvector<T,L>    
operator-(double lhs, const dvector<T,L> &rhs)
{

    dvector<T,L> result = rhs;
    result -= lhs;
    return result;

}

template <class T, size_t L> inline dvector<T,L>    
operator*(double lhs, const dvector<T,L> &rhs)
{

    dvector<T,L> result = rhs;
    result *= lhs;
    return result;

}

template <class T, size_t L> inline dvector<T,L>    
operator/(double lhs, const dvector<T,L> &rhs)
{

    dvector<T,L> result = rhs;
    result /= lhs;
    return result;

}

template <class T, size_t L> inline dvector<T,L>    
operator+(const dvector<T,L> &lhs, double rhs)
{

    dvector<T,L> result = lhs;
    result += rhs;
    return result;

}

template <class T, size_t L> inline dvector<T,L>    
operator-(const dvector<T,L> &lhs, double rhs)
{

    dvector<T,L> result = lhs;
    result -= rhs;
    return result;

}

template <class T, size_t L> inline dvector<T,L>    
operator*(const dvector<T,L> &lhs, double rhs)
{

    dvector<T,L> result = lhs;
    result *= rhs;
    return result;

}

template <class T, size_t L> inline dvector<T,L>    
operator/(const dvector<T,L> &lhs, double rhs)
{

    dvector<T,L> result = lhs;
    result /= rhs;
    return result;

}

template <class T, size_t L> inline dvector<T,L>    
operator+(const dvector<T,L> &lhs, const dvector<T,L> &rhs)
{

    dvector<T,L> result = lhs;
    result += rhs;
    return result;

}

template <class T, size_t L> inline dvector<T,L>    
operator-(const dvector<T,L> &lhs, const dvector<T,L> &rhs)
{

    dvector<T,L> result = lhs;
    result -= rhs;
    return result;

}

template <class T, size_t L> inline dvector<T,L>    
operator*(const dvector<T,L> &lhs, const dvector<T,L> &rhs)
{

    dvector<T,L> result = lhs;
    result *= rhs;
    return result;

}

template <class T, size_t L> inline dvector<T,L>    
operator/(const dvector<T,L> &lhs, const dvector<T,L> &rhs)
{

    dvector<T,L> result = lhs;
    result /= rhs;
    return result;

}


#endif
