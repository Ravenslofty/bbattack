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

#endif // #ifndef BBATTACK_PRIVATE_H
