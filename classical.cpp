/*
 * The MIT License (MIT)
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

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "bbattack.h"

#ifdef USE_CLASSICAL

#include "bbattack-private.h"

namespace {
    uint64_t ClassicalAttacks[64][8];

    uint64_t LSB(uint64_t x)
    {
        return __builtin_ctzll(x);
    }

    uint64_t MSB(uint64_t x)
    {
        return 63 ^ __builtin_clzll(x);
    }

    template<Direction dir> uint64_t Classical(const uint64_t occ, const unsigned int sq)
    {
        const uint64_t attacks = ClassicalAttacks[sq][dir];
        const uint64_t blocker = attacks & occ;

        if (DirShift[dir] > 0) {
            return attacks & ~ClassicalAttacks[LSB(blocker | (1ULL << 63))][dir];
        } else {
            return attacks & ~ClassicalAttacks[MSB(blocker | 1ULL)][dir];
        }
    }
}

extern "C" {
uint64_t BBAttackBishop(const uint64_t occ, const unsigned int sq)
{
    return Classical<Northeast>(occ, sq) |
        Classical<Southeast>(occ, sq) |
        Classical<Southwest>(occ, sq) |
        Classical<Northwest>(occ, sq);
}

uint64_t BBAttackRook(const uint64_t occ, const unsigned int sq)
{
    return Classical<North>(occ, sq) |
        Classical<East>(occ, sq) |
        Classical<South>(occ, sq) |
        Classical<West>(occ, sq);
}

void BBAttackInit()
{
    int sq;

    for (sq = 0; sq < 64; sq++) {
        ClassicalAttacks[sq][North] = GenMask<North, false>(sq);
        ClassicalAttacks[sq][South] = GenMask<South, false>(sq);
        ClassicalAttacks[sq][East] = GenMask<East, false>(sq);
        ClassicalAttacks[sq][West] = GenMask<West, false>(sq);
        ClassicalAttacks[sq][Northeast] = GenMask<Northeast, false>(sq);
        ClassicalAttacks[sq][Southeast] = GenMask<Southeast, false>(sq);
        ClassicalAttacks[sq][Southwest] = GenMask<Southwest, false>(sq);
        ClassicalAttacks[sq][Northwest] = GenMask<Northwest, false>(sq);
    }
}
}

#endif // #ifdef USE_CLASSICAL
