#ifndef BASE_H
#define BASE_H

#if defined(INLINE)
// do noting
#elif defined(__GNUC__) || defined(__clang__)
#define INLINE inline __attribute__((always_inline))
#elif defined(_MSC_VER)
#define INLINE __forceinline
#else
#define INLINE inline
#endif

#endif // BASE_H