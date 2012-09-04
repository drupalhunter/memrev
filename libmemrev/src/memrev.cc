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
#include <cstring>
#include <cstdint>
#include <cstdlib>

#define UNREACHABLE() __builtin_unreachable()

using namespace std;

namespace {

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

/// Returns a pointer whose integer value is the largest multiple of
/// alignof(T) less than or equal to \a ptr.
template<typename T>
inline T* AlignBack(const void* ptr) {
    const size_t alignment = alignof(T);
    return (T*) (((uintptr_t) ptr / alignment) * alignment);
}

/// Returns a pointer whose integer value is the smallest multiple of
/// alignof(T) greater than or equal to \a ptr.
template<typename T>
inline T* AlignFront(const void* ptr) {
    const size_t alignment = alignof(T);
    return (T*) ((((uintptr_t) ptr + alignment - 1) / alignment) * alignment);
}

#ifdef USE_VECTOR
#include "__vector.h"

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

template<typename Vector, typename T>
void MemrevImpl(T* data, size_t count) {
    // First, we need to find a subset of the array with SIMD-aligned bounds
    // and a length that is a multiple of twice the SIMD register width.
    Vector* vector_start = AlignFront<Vector>(data);
    Vector* vector_end = AlignBack<Vector>(&data[count]);
    ptrdiff_t total_vectors = vector_end - vector_start;
    if (total_vectors % 2) {
        total_vectors--;
        vector_end--;
    }

    // Only use vector acceleration if
    //  (1) we have a large enough array for it to make a difference, and
    //  (2) the vector region is aligned with |data|
    const ptrdiff_t kMinVectors = 2;
    size_t vector_offset = (uintptr_t) vector_start - (uintptr_t) data;
    if (total_vectors < kMinVectors || vector_offset % sizeof(T) != 0) {
        Reverse(data, count);
        return;
    }

    // We've found a suitable subset. Do the actual swapping.
    for (size_t i = 0; i < total_vectors / 2; i++) {
        Vector v = ReverseVector(vector_start[i]);
        vector_start[i] = ReverseVector(vector_end[-1 - i]);
        vector_end[-1 - i] = v;
    }

    // Our subset of the array is now reversed. But:
    //  (a) There may be unreversed bytes at the beginning and/or end.
    //  (b) The reversed subset may need shifting to its final position.
    size_t vector_end_offset = (uintptr_t) &data[count] -
                                (uintptr_t) vector_end;
    ShiftMiddleAndSwapSides((char*) data, count * sizeof(T),
                            vector_offset, vector_end_offset);

    // Reverse the remaining start and end units. Note that after
    // ShiftMiddleAndSwapSides(), |vector_offset| is the number of unreversed
    // bytes at the *end* of the array.
    Reverse(data, vector_end_offset / sizeof(T));
    size_t remaining_end_units = vector_offset / sizeof(T);
    Reverse(&data[count - remaining_end_units], remaining_end_units);
}

#else // USE_VECTOR

template<typename T>
void MemrevImpl(T* data, size_t count) {
    Reverse(data, count);
}
#endif // USE_VECTOR

} // namespace

void* memrev_reverse(void* data, size_t size, size_t count) {
#ifdef USE_VECTOR
#define CALL_MEMREV_IMPL(WIDTH) \
    MemrevImpl<Vector ## WIDTH>((uint ## WIDTH ##_t*) data, count)
#else
#define CALL_MEMREV_IMPL(WIDTH) \
    MemrevImpl((uint ## WIDTH ##_t*) data, count)
#endif

    switch (size) {
    case 1:
        CALL_MEMREV_IMPL(8);
        break;
    case 2:
        CALL_MEMREV_IMPL(16);
        break;
    case 4:
        CALL_MEMREV_IMPL(32);
        break;
    case 8:
        CALL_MEMREV_IMPL(64);
        break;
    default:
        return NULL;
    }
    return data;
}
