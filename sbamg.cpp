
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

#ifdef USE_SBAMG

#include "bbattack-private.h"

static struct {
    uint64_t Lower;
    uint64_t Line;
    uint64_t Outer;
} SBAMGMasks[64][4];

namespace {
    enum MaskType {
        Diagonal,
        Antidiagonal,
        File,
        Rank
    };

    template<MaskType type> uint64_t MaskLower(const unsigned int sq)
    {
        assert(sq <= 63);

        return SBAMGMasks[sq][type].Lower;
    }

    template<MaskType type> uint64_t MaskLine(const unsigned int sq)
    {
        assert(sq <= 63);

        return SBAMGMasks[sq][type].Line;
    }
    
    template<MaskType type> uint64_t MaskOuter(const unsigned int sq)
    {
        assert(sq <= 63);

        return SBAMGMasks[sq][type].Outer;
    }

    uint64_t MSB(uint64_t x)
    {
        return 63 ^ __builtin_clzll(x);
    }

    template<Direction dir> uint64_t GenOuter(const int sq)
    {
        return GenMask<dir, false>(sq) & ~GenMask<dir, true>(sq);
    }

    template<MaskType type> uint64_t SBAMG(const uint64_t occ, const unsigned int sq)
    {
        const uint64_t line = (occ & MaskLine<type>(sq)) | MaskOuter<type>(sq);

        const uint64_t blocker = 3ULL << MSB(line & MaskLower<type>(sq));

        return (line ^ (line - blocker)) & MaskLine<type>(sq);
    }
}

extern "C" {
uint64_t BBAttackBishop(const uint64_t occ, const unsigned int sq)
{
    return SBAMG<MaskType::Diagonal>(occ, sq) | 
        SBAMG<MaskType::Antidiagonal>(occ, sq);
}

uint64_t BBAttackRook(const uint64_t occ, const unsigned int sq)
{
    return SBAMG<MaskType::Rank>(occ, sq) |
        SBAMG<MaskType::File>(occ, sq);
}

void BBAttackInit()
{
    int sq, dest;
    uint64_t mask;

    for (sq = 0; sq < 64; sq++) {

        SBAMGMasks[sq][MaskType::Rank].Lower = (sq == 0) ? 1ULL : ((1ULL << sq) - 1);
        SBAMGMasks[sq][MaskType::Rank].Line  = GenMask<East, false>(sq) | GenMask<West, false>(sq);
        SBAMGMasks[sq][MaskType::Rank].Outer = GenOuter<East>(sq) | GenOuter<West>(sq) | 1;

        SBAMGMasks[sq][MaskType::File].Lower = (sq == 0) ? 1ULL : ((1ULL << sq) - 1);
        SBAMGMasks[sq][MaskType::File].Line  = GenMask<North, false>(sq) | GenMask<South, false>(sq);
        SBAMGMasks[sq][MaskType::File].Outer = GenOuter<North>(sq) | GenOuter<South>(sq) | 1;

        SBAMGMasks[sq][MaskType::Diagonal].Lower = (sq == 0) ? 1ULL : ((1ULL << sq) - 1);
        SBAMGMasks[sq][MaskType::Diagonal].Line  = GenMask<Northeast, false>(sq) | GenMask<Southwest, false>(sq);
        SBAMGMasks[sq][MaskType::Diagonal].Outer = GenOuter<Northeast>(sq) | GenOuter<Southwest>(sq) | 1;

        SBAMGMasks[sq][MaskType::Antidiagonal].Lower = (sq == 0) ? 1ULL : ((1ULL << sq) - 1);
        SBAMGMasks[sq][MaskType::Antidiagonal].Line  = GenMask<Northwest, false>(sq) | GenMask<Southeast, false>(sq);
        SBAMGMasks[sq][MaskType::Antidiagonal].Outer = GenOuter<Northwest>(sq) | GenOuter<Southeast>(sq) | 1;
    }
}
}

#endif // #ifdef USE_SBAMG
