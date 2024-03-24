#ifndef SIGMAFOX_CORE_PRIMITIVES_H
#define SIGMAFOX_CORE_PRIMITIVES_H
#include <cstdint>
#include <cstddef>

// --- Primitive Remaps --------------------------------------------------------
//
// Shorthand remaps of command primitive datatypes with fixed sizing.
//

typedef uint8_t         u8;
typedef uint16_t        u16;
typedef uint32_t        u32;
typedef uint64_t        u64;

typedef int8_t          i8;
typedef int16_t         i16;
typedef int32_t         i32;
typedef int64_t         i64;

typedef float           r32;
typedef double          r64;

typedef int8_t          b8;
typedef int16_t         b16;
typedef int32_t         b32;
typedef int64_t         b64;

// --- Tag Definitions ---------------------------------------------------------
//
// SF_PERSIST
//      For static variables. This clearly marks their useage when they are used.
//
// SF_INTERNAL
//      For static function bodies in implementation files not meant to be extern'd
//      by other object files.
//
// SF_INLINE
//      A candidate for inlining, not meant for the compiler. Suggests for further
//      evaluation to enforce inlining using compiler intrinsics.
//
// SF_MAY_ASSERT
//      If a function is marked with this tag, that means that the function has
//      potential to assert. If a function has potential to assert, than it should
//      be appropriately evaluated at a later time to handle the condition with
//      an alternative solution.
//
//      Some functions may have assertion conditions in which critical failure
//      is acceptable (for example, unreachable conditions).
//
// SF_OPTIONAL
//      If a functional parameter can be optionally provided, it should be clearly
//      marked as so. Optional parameters are typically for functions that have
//      input parameters of pointers which operate on the data. In some instances,
//      the user may not care about the data operation and only the return value,
//      so therefore the parameter is optional.
//
//

#define SF_PERSIST      static
#define SF_INTERNAL     static
#define SF_INLINE       inline
#define SF_MAY_ASSERT
#define SF_OPTIONAL

#endif
