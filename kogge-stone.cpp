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

#include "bbattack.h"

#ifdef USE_KOGGE_STONE

enum {
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

template<int dir>
uint64_t KoggeStone(uint64_t empty, uint64_t fill)
{
    static_assert(dir >= 0 && dir <= 7, "Direction out of range");
    constexpr int shift = DirShift[dir];
    constexpr uint64_t mask = DirMask[dir];
    empty &= mask;
    fill |= empty & Shift<shift  >(fill);
    empty = empty & Shift<shift  >(empty);
    fill |= empty & Shift<shift*2>(fill);
    empty = empty & Shift<shift*2>(empty);
    fill |= empty & Shift<shift*4>(fill);
    return  mask  & Shift<shift  >(fill);
}

extern "C" {

uint64_t BBAttackBishop(const uint64_t occ, const unsigned int sq)
{
    uint64_t empty = ~occ;
    uint64_t bishop = 1ULL << sq;
    return KoggeStone<Northeast>(empty, bishop) |
           KoggeStone<Northwest>(empty, bishop) |
           KoggeStone<Southeast>(empty, bishop) |
           KoggeStone<Southwest>(empty, bishop);
}

uint64_t BBAttackRook(const uint64_t occ, const unsigned int sq)
{
    uint64_t empty = ~occ;
    uint64_t rook = 1ULL << sq;
    return KoggeStone<North>(empty, rook) |
           KoggeStone<South>(empty, rook) |
           KoggeStone<East >(empty, rook) |
           KoggeStone<West >(empty, rook);
}

void BBAttackInit()
{
    // No-op.
}
}

#endif // #ifdef USE_KOGGE_STONE
