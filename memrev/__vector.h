/* Copyright 2012 Philip Puryear
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef REV_VECTOR_H_
#define REV_VECTOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#define SIMD_WIDTH 16
typedef uint8_t  Vector8  __attribute__((vector_size(SIMD_WIDTH)));
typedef uint16_t Vector16 __attribute__((vector_size(SIMD_WIDTH)));
typedef uint32_t Vector32 __attribute__((vector_size(SIMD_WIDTH)));
typedef uint64_t Vector64 __attribute__((vector_size(SIMD_WIDTH)));

#define REVERSE_MASK_8  15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define REVERSE_MASK_16 7, 6, 5, 4, 3, 2, 1, 0
#define REVERSE_MASK_32 3, 2, 1, 0
#define REVERSE_MASK_64 1, 0

#if defined(__clang__)
#if __has_builtin(__builtin_shufflevector)
#define REVERSE_VECTOR_FUNC(WIDTH) \
    static inline                                                             \
    Vector ## WIDTH ReverseVector ## WIDTH(Vector ## WIDTH vec) {             \
        return __builtin_shufflevector(vec, vec, REVERSE_MASK_ ## WIDTH);     \
    }
#else
#error "A version of clang with __builtin_shufflevector is required."
#endif
/* XXX: GCC's __builtin_shuffle is not enabled in C++ for some reason. */
/*
#elif defined(__GNUC__)
#define REVERSE_VECTOR_FUNC(WIDTH) \
    static inline                                                             \
    Vector ## WIDTH ReverseVector ## WIDTH(Vector ## WIDTH vec) {             \
        const static Vector ## WIDTH mask = {REVERSE_MASK_ ## WIDTH};         \
        return __builtin_shuffle(vec, mask);                                  \
    }
#endif
*/
#else
#error "Compiler not supported."
#endif

REVERSE_VECTOR_FUNC(8);
REVERSE_VECTOR_FUNC(16);
REVERSE_VECTOR_FUNC(32);
REVERSE_VECTOR_FUNC(64);

#ifdef __cplusplus
}
#endif

#endif /* REV_VECTOR_H_ */
