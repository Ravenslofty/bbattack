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

#ifdef USE_DUMB7FILL

#include "bbattack-private.h"

template<Direction dir>
uint64_t Dumb7Fill(uint64_t empty, uint64_t fill)
{
    static_assert(dir >= 0 && dir <= 7, "Direction out of range");
    constexpr int shift = DirShift[dir];
    constexpr uint64_t mask = DirMask[dir];
    uint64_t flood = fill;
    empty &= mask;
    flood |= fill = Shift<shift>(fill) & empty;
    flood |= fill = Shift<shift>(fill) & empty;
    flood |= fill = Shift<shift>(fill) & empty;
    flood |= fill = Shift<shift>(fill) & empty;
    flood |= fill = Shift<shift>(fill) & empty;
    flood |= fill = Shift<shift>(fill) & empty;
    flood |=        Shift<shift>(fill) & empty;
    return          Shift<shift>(flood) & mask;
}

extern "C" {

uint64_t BBAttackBishop(const uint64_t occ, const unsigned int sq)
{
    uint64_t empty = ~occ;
    uint64_t bishop = 1ULL << sq;
    return Dumb7Fill<Direction::Northeast>(empty, bishop) |
           Dumb7Fill<Direction::Northwest>(empty, bishop) |
           Dumb7Fill<Direction::Southeast>(empty, bishop) |
           Dumb7Fill<Direction::Southwest>(empty, bishop);
}

uint64_t BBAttackRook(const uint64_t occ, const unsigned int sq)
{
    uint64_t empty = ~occ;
    uint64_t rook = 1ULL << sq;
    return Dumb7Fill<Direction::North>(empty, rook) |
           Dumb7Fill<Direction::South>(empty, rook) |
           Dumb7Fill<Direction::East >(empty, rook) |
           Dumb7Fill<Direction::West >(empty, rook);
}

void BBAttackInit()
{
    // No-op.
}
}

#endif // #ifdef USE_DUMB7FILL
