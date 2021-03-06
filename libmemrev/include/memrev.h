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

#ifndef MEMREV_H_
#define MEMREV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#define MEMREV_VECTOR_SIZE 16

/**
 * Reverses the order of a block of memory.
 *
 * The memory block is treated as an array of length @a count containing ints
 * of size @size.
 *
 * Performance note: When vector acceleration is enabled, this function can
 * achieve much higher performance when the input bounds are aligned to the
 * vector register width (exposed as the preprocessor macro
 * MEMREV_VECTOR_SIZE). Specifically, the highest performance is achieved when
 *  - @a data is aligned to MEMREV_VECTOR_SIZE, and
 *  - @a count * @a size is a multiple of 2 * MEMREV_VECTOR_SIZE
 *
 * @param data A pointer to the start of the area of memory to be reversed.
 * @param size The size of each unit of @a data, in bytes. Must be one of the
               following: 1, 2, 4, 8.
 * @param len The number of units to reverse.
 * @return A pointer to @a data, or NULL if @a size was invalid.
 */
void* memrev_reverse(void* data, size_t size, size_t count);

#ifdef __cplusplus
}
#endif

#endif /* MEMREV_H_ */
