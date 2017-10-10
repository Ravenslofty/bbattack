/*
 * MIT License
 *
 * Copyright (c) 2017 Dan Ravensloft
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef BBATTACK_PRIVATE_H
#define BBATTACK_PRIVATE_H

#include <stdint.h>

enum Direction {
    North,
    South,
    East,
    West,
    Northeast,
    Southeast,
    Southwest,
    Northwest
};

constexpr int DirShift[8] = {
    +8, // North
    -8, // South
    +1, // East
    -1, // West
    +9, // Northeast
    -7, // Southeast
    -9, // Southwest
    +7  // Northwest
};

constexpr uint64_t DirMask[8] = {
    0xFFFFFFFFFFFFFFFFULL, // North     (no mask)
    0xFFFFFFFFFFFFFFFFULL, // South     (no mask)
    0xFEFEFEFEFEFEFEFEULL, // East      (A-file)
    0x7F7F7F7F7F7F7F7FULL, // West      (H-file)
    0xFEFEFEFEFEFEFEFEULL, // Northeast (A-file)
    0xFEFEFEFEFEFEFEFEULL, // Southeast (A-file)
    0x7F7F7F7F7F7F7F7FULL, // Southwest (H-file)
    0x7F7F7F7F7F7F7F7FULL, // Northwest (H-file)
};

constexpr bool DirIsEast[8] = {
    false, // North
    false, // South
    true,  // East
    false, // West
    true,  // Northeast
    true,  // Southeast
    false, // Southwest
    false  // Northwest
};

constexpr bool DirIsWest[8] = {
    false, // North
    false, // South
    false, // East
    true,  // West
    false, // Northeast
    false, // Southeast
    true,  // Southwest
    true   // Northwest
};

template<int shift>
uint64_t Shift(uint64_t x)
{
    if (shift > 0) {
        return x << shift;
    } else {
        return x >> -shift;
    }
}

uint64_t Swap(uint64_t x)
{
    return __builtin_bswap64(x);
}

template<Direction dir>
bool OnBoard(const int sq)
{
    constexpr int inc = DirShift[dir];
    const int file = sq % 8;

    if (inc > 0 && sq >= 64) {
        return false;
    }

    if (inc < 0 && sq < 0) {
        return false;
    }

    if (DirIsEast[dir] && file == 0) {
        return false;
    }

    if (DirIsWest[dir] && file == 7) {
        return false;
    }

    return true;
}

template<Direction dir, bool exclude_outer>
uint64_t GenMask(const unsigned int sq)
{
    static_assert(dir >= 0 && dir <= 7, "Direction out of range");

    uint64_t bb = 0;
    constexpr int inc = DirShift[dir];
    int dest;

    const uint64_t outer_mask[8] = {
        0x00FFFFFFFFFFFFFFULL, // North
        0xFFFFFFFFFFFFFF00ULL, // South
        0x7F7F7F7F7F7F7F7FULL, // East
        0xFEFEFEFEFEFEFEFEULL, // West
        0x007F7F7F7F7F7F7FULL, // Northeast
        0x7F7F7F7F7F7F7F00ULL, // Southeast
        0xFEFEFEFEFEFEFE00ULL, // Southwest
        0x00FEFEFEFEFEFEFEULL  // Northwest
    };

    for (dest = sq + inc; OnBoard<dir>(dest); dest += inc) {
        bb |= 1ULL << dest;
    }

    // Ignore outer bits if requested.
    if (exclude_outer) {
        bb &= outer_mask[dir];
    }

    return bb;
}

#endif // #ifndef BBATTACK_PRIVATE_H
