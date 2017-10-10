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

#ifdef USE_OBSTRUCTION

#include "bbattack-private.h"

static struct {
    uint64_t Upper;
    uint64_t Lower;
} ObstructionMasks[64][4];

namespace {
    enum MaskType {
        Diagonal,
        Antidiagonal,
        File,
        Rank
    };

    template<MaskType type> uint64_t MaskUpper(const unsigned int sq)
    {
        assert(sq <= 63);

        return ObstructionMasks[sq][type].Upper;
    }

    template<MaskType type> uint64_t MaskLower(const unsigned int sq)
    {
        assert(sq <= 63);

        return ObstructionMasks[sq][type].Lower;
    }

    uint64_t LSB(uint64_t x)
    {
        return __builtin_ctzll(x);
    }

    uint64_t MSB(uint64_t x)
    {
        return 63 ^ __builtin_clzll(x);
    }

    template<MaskType type> uint64_t Obstruction(const uint64_t occ, const unsigned int sq)
    {
        const uint64_t upper = MaskUpper<type>(sq) & occ;
        const uint64_t lower = MaskLower<type>(sq) & occ;

        const uint64_t highest_low = -1ULL << MSB(lower | 1);
        const uint64_t lowest_high = upper & -upper;

        const uint64_t diff = 2 * lowest_high + highest_low;

        return (MaskUpper<type>(sq) | MaskLower<type>(sq)) & diff;
    }
}

extern "C" {
uint64_t BBAttackBishop(const uint64_t occ, const unsigned int sq)
{
    return Obstruction<MaskType::Diagonal>(occ, sq) | 
        Obstruction<MaskType::Antidiagonal>(occ, sq);
}

uint64_t BBAttackRook(const uint64_t occ, const unsigned int sq)
{
    return Obstruction<MaskType::Rank>(occ, sq) |
        Obstruction<MaskType::File>(occ, sq);
}

void BBAttackInit()
{
    int sq, dest;
    uint64_t mask;

    for (sq = 0; sq < 64; sq++) {

        ObstructionMasks[sq][MaskType::Rank].Upper = GenMask<East, false>(sq);
        ObstructionMasks[sq][MaskType::Rank].Lower = GenMask<West, false>(sq);

        ObstructionMasks[sq][MaskType::File].Upper = GenMask<North, false>(sq);
        ObstructionMasks[sq][MaskType::File].Lower = GenMask<South, false>(sq);
   
        ObstructionMasks[sq][MaskType::Diagonal].Upper = GenMask<Northeast, false>(sq);
        ObstructionMasks[sq][MaskType::Diagonal].Lower = GenMask<Southwest, false>(sq);
        
        ObstructionMasks[sq][MaskType::Antidiagonal].Upper = GenMask<Northwest, false>(sq);
        ObstructionMasks[sq][MaskType::Antidiagonal].Lower = GenMask<Southeast, false>(sq);
    }
}
}

#endif // #ifdef USE_OBSTRUCTION
