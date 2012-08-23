// Copyright 2012 Philip Puryear
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "memrev.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cstdlib>

#define UNREACHABLE() __builtin_unreachable()

using namespace std;

namespace {

#ifdef USE_VECTOR
#include "__vector.h"

/// Returns a pointer whose integer value is the largest multiple of
/// \a boundary less than or equal to \a ptr.
template<typename T>
inline T* AlignBack(const void* ptr, size_t boundary) {
    return (T*) (((uintptr_t) ptr / boundary) * boundary);
}

/// Returns a pointer whose integer value is the smallest multiple of
/// \a boundary greater than or equal to \a ptr.
template<typename T>
inline T* AlignFront(const void* ptr, size_t boundary) {
    return (T*) ((((uintptr_t) ptr + boundary - 1) / boundary) * boundary);
}

/// Returns a copy of \a vec with the units reversed.
template<typename Unit, typename Vector>
inline Vector ReverseVector(const Vector& vec) {
    switch (sizeof(Unit)) {
    case 1:
        return ReverseVector8(vec);
    case 2:
        return ReverseVector16(vec);
    case 4:
        return ReverseVector32(vec);
    case 8:
        return ReverseVector64(vec);
    }
    UNREACHABLE();
}

/// The intermediate stage of the vector reversal algorithm.
/// (See MemrevImpl)
void ShiftMiddleAndSwapSides(char* data, size_t data_length,
                             size_t start_length, size_t end_length) {
    char* data_end = data + data_length;
    char* sides_temp;
    size_t sides_length = start_length + end_length;
    // Save the sides.
    if (sides_length > 0) {
        sides_temp = new char[sides_length];
        memcpy(sides_temp, data, start_length);
        memcpy(sides_temp + start_length, data_end - end_length, end_length);
    }

    // Shift the middle.
    if (start_length != end_length) {
        memmove(data + end_length, data + start_length,
                data_length - sides_length);
    }

    // Restore the sides, swapped.
    if (sides_length > 0) {
        memcpy(data, sides_temp + start_length, end_length);
        memcpy(data_end - start_length, sides_temp, start_length);
        delete [] sides_temp;
    }
}
#endif // USE_VECTOR

/// Reverses the order of the first \a count units of \a data using a simple
/// swap algorithm.
template<typename T>
void Reverse(T* data, size_t count) {
    T* data_end = data + count;
    for (size_t i = 0; i < count / 2; i++) {
        T e = data[i];
        data[i] = data_end[-1 - i];
        data_end[-1 - i] = e;
    }
}

#ifdef USE_VECTOR
template<typename Vector, typename T>
#else
template<typename T>
#endif
T* MemrevImpl(T* data, size_t count) {
    size_t remaining_units = count;

#ifdef USE_VECTOR
    const size_t kMinVectorBytes = 0;
    // First, we need to find a subset of the array with 16-byte-aligned bounds
    // and a length that is a multiple of 32 bytes.
    Vector* vector_start = AlignFront<Vector>(data, SIMD_WIDTH);
    Vector* vector_end = AlignBack<Vector>(&data[count], SIMD_WIDTH);
    ptrdiff_t total_vector_bytes =
            (uintptr_t) vector_end - (uintptr_t) vector_start;
    if (total_vector_bytes % (2 * SIMD_WIDTH) != 0) {
        total_vector_bytes -= SIMD_WIDTH;
        vector_end = (Vector*) (((char*) vector_end) - SIMD_WIDTH);
    }
    if (total_vector_bytes > kMinVectorBytes) {
        // We've found a suitable and sufficiently large subset.
        // Do the actual swapping.
        for (size_t i = 0; i < total_vector_bytes / (2 * SIMD_WIDTH); i++) {
            Vector v = ReverseVector<T>(vector_start[i]);
            vector_start[i] = ReverseVector<T>(vector_end[-1 - i]);
            vector_end[-1 - i] = v;
        }

        // Our subset of the array is now reversed. But:
        //  (a) There may be unreversed bytes at the beginning and/or end.
        //  (b) The reversed subset may need shifting to its final position.
        // Take care of both of these at once.
        // Note: |remaining_end_length| refers to the number of unreversed
        // bytes that will *end up* at the end of the array, not the ones that
        // are currently at the end (likewise for |remaining_start_length|).
        size_t total_bytes = count * sizeof(T);
        size_t remaining_end_length =
                (uintptr_t) vector_start - (uintptr_t) data;
        size_t remaining_start_length =
                total_bytes - total_vector_bytes - remaining_end_length;
        ShiftMiddleAndSwapSides((char*) data, total_bytes,
                                remaining_end_length, remaining_start_length);

        // Reverse the end.
        size_t remaining_end_count = remaining_end_length / sizeof(T);
        if (remaining_end_count > 0)
            Reverse(&data[count - remaining_end_count], remaining_end_count);
        remaining_units = remaining_start_length / sizeof(T);
    }
#endif

    // Reverse the remaining units.
    if (remaining_units > 0)
        Reverse(data, remaining_units);
    return data;
}

} // namespace

char* strrev(char* str) {
    return (char*) memrev(str, 1, strlen(str));
}

void* memrev(void* data, int size, size_t count) {
    const static int kMaxUnitSize = 8;
    if (size <= 0 || size > kMaxUnitSize || size & (size - 1))
        return NULL;

#ifdef USE_VECTOR
#define CALL_MEMREV_IMPL(WIDTH) \
    MemrevImpl<Vector ## WIDTH>((uint ## WIDTH ##_t*) data, count);
#else
#define CALL_MEMREV_IMPL(WIDTH) \
    MemrevImpl((uint ## WIDTH ##_t*) data, count);
#endif

    switch (size) {
    case 1:
        return CALL_MEMREV_IMPL(8);
    case 2:
        return CALL_MEMREV_IMPL(16);
    case 4:
        return CALL_MEMREV_IMPL(32);
    case 8:
        return CALL_MEMREV_IMPL(64);
    }
    UNREACHABLE();
}
