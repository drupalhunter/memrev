#include "memrev.h"

#include <algorithm>
#include <cstring>
#include <stdint.h>
#include <gtest/gtest.h>

namespace {

// A 446-character string.
const char* kLoremIpsum =
"Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod "
"tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, "
"quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo "
"consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse "
"cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat "
"non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

template<typename T>
void memrev_test(size_t offset, size_t count, size_t byte_offset = 0) {
    char* actual = strdup(kLoremIpsum);
    char* expected = strdup(kLoremIpsum);

    T* expected_units = (T*) (expected + byte_offset);
    std::reverse(&expected_units[offset], &expected_units[offset + count]);
    memrev_reverse(actual + byte_offset + offset * sizeof(T), sizeof(T),
                   count);

    EXPECT_STREQ(expected, actual);

    std::free(actual);
    std::free(expected);
}

TEST(memrev, Char_Small) {
    memrev_test<char>(0, 5);
}

TEST(memrev, Char_Small_Misaligned) {
    memrev_test<char>(1, 4);
}

TEST(memrev, Char_Medium_BothAligned) {
    memrev_test<char>(0, 32);
}

TEST(memrev, Char_Medium_FrontMisaligned) {
    memrev_test<char>(1, 31);
}

TEST(memrev, Char_Medium_BackMisaligned) {
    memrev_test<char>(0, 33);
}

TEST(memrev, Char_Medium_BothMisaligned) {
    memrev_test<char>(1, 32);
}

TEST(memrev, Char_Large_BothAligned) {
    memrev_test<char>(0, 256);
}

TEST(memrev, Char_Large_FrontMisaligned) {
    memrev_test<char>(1, 255);
}

TEST(memrev, Char_Large_BackMisaligned) {
    memrev_test<char>(0, 257);
}

TEST(memrev, Char_Large_BothMisaligned) {
    memrev_test<char>(1, 256);
}

TEST(memrev, Int16_Small) {
    memrev_test<uint16_t>(0, 5);
}

TEST(memrev, Int16_Small_Misaligned) {
    memrev_test<uint16_t>(1, 4);
}

TEST(memrev, Int16_Medium_BothAligned) {
    memrev_test<uint16_t>(0, 16);
}

TEST(memrev, Int16_Medium_FrontMisaligned) {
    memrev_test<uint16_t>(1, 15);
}

TEST(memrev, Int16_Medium_BackMisaligned) {
    memrev_test<uint16_t>(0, 17);
}

TEST(memrev, Int16_Medium_BothMisaligned) {
    memrev_test<uint16_t>(1, 16);
}

TEST(memrev, Int16_Large_BothAligned) {
    memrev_test<uint16_t>(0, 216);
}

TEST(memrev, Int16_Large_FrontMisaligned) {
    memrev_test<uint16_t>(1, 215);
}

TEST(memrev, Int16_Large_BackMisaligned) {
    memrev_test<uint16_t>(0, 217);
}

TEST(memrev, Int16_Large_BothMisaligned) {
    memrev_test<uint16_t>(1, 216);
}

TEST(memrev, Int32_Small) {
    memrev_test<uint32_t>(0, 5);
}

TEST(memrev, Int32_Small_Misaligned) {
    memrev_test<uint32_t>(1, 4);
}

TEST(memrev, Int32_Medium_BothAligned) {
    memrev_test<uint32_t>(0, 8);
}

TEST(memrev, Int32_Medium_FrontMisaligned) {
    memrev_test<uint32_t>(1, 7);
}

TEST(memrev, Int32_Medium_BackMisaligned) {
    memrev_test<uint32_t>(0, 9);
}

TEST(memrev, Int32_Medium_BothMisaligned) {
    memrev_test<uint32_t>(1, 8);
}

TEST(memrev, Int32_Large_BothAligned) {
    memrev_test<uint32_t>(0, 108);
}

TEST(memrev, Int32_Large_FrontMisaligned) {
    memrev_test<uint32_t>(1, 107);
}

TEST(memrev, Int32_Large_BackMisaligned) {
    memrev_test<uint32_t>(0, 109);
}

TEST(memrev, Int32_Large_BothMisaligned) {
    memrev_test<uint32_t>(1, 108);
}

TEST(memrev, Int64_Small) {
    memrev_test<uint64_t>(0, 3);
}

TEST(memrev, Int64_Small_Misaligned) {
    memrev_test<uint64_t>(1, 2);
}

TEST(memrev, Int64_Medium_BothAligned) {
    memrev_test<uint64_t>(0, 4);
}

TEST(memrev, Int64_Medium_FrontMisaligned) {
    memrev_test<uint64_t>(1, 3);
}

TEST(memrev, Int64_Medium_BackMisaligned) {
    memrev_test<uint64_t>(0, 5);
}

TEST(memrev, Int64_Medium_BothMisaligned) {
    memrev_test<uint64_t>(1, 4);
}

TEST(memrev, Int64_Large_BothAligned) {
    memrev_test<uint64_t>(0, 54);
}

TEST(memrev, Int64_Large_FrontMisaligned) {
    memrev_test<uint64_t>(1, 53);
}

TEST(memrev, Int64_Large_BackMisaligned) {
    memrev_test<uint64_t>(0, 55);
}

TEST(memrev, Int64_Large_BothMisaligned) {
    memrev_test<uint64_t>(1, 54);
}

TEST(memrev, UnitMisaligned) {
    memrev_test<uint32_t>(0, 100, 1);
}

TEST(memrev, ZeroLengthInput) {
    memrev_test<uint32_t>(0, 0);
}

TEST(memrev, InvalidSize) {
    static char dummy[] = "dummy string";
    EXPECT_EQ(NULL, memrev_reverse(dummy, 3, 2));
}

} // namespace
