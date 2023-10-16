#pragma once

#ifdef _MSC_VER

#define ALWAYS_INLINE __forceinline
#define NEVER_INLINE __declspec(noinline)
#define DECLSPEC_ALLOCATOR __declspec(allocator)
#define ASSUME_IMPL(x) __assume(x)
#else // GCC/Clang
#define ALWAYS_INLINE __attribute__((always_inline))
#define NEVER_INLINE __attribute__((noinline))
#ifdef __clang__
#define ASSUME_IMPL(x) __builtin_assume(x)
#define DECLSPEC_ALLOCATOR __declspec(allocator)
#else // GCC
#define ASSUME_IMPL(x) if (!(x)) __builtin_unreachable()
#define DECLSPEC_ALLOCATOR __attribute__((malloc))
#endif
#define abstract
#endif
