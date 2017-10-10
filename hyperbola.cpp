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

#ifdef USE_HYPERBOLA

static struct {
    uint64_t DiagMask;
    uint64_t AntiDiagMask;
    uint64_t FileMask;
    uint64_t RankMask;
} HyperbolaMasks[64];

uint8_t RankAttacks[64*8];

namespace Detail {
    enum MaskType {
        Diagonal,
        Antidiagonal,
        File
    };

    template<MaskType type> uint64_t Mask(const unsigned int sq);

    template<> uint64_t Mask<MaskType::Diagonal>(const unsigned int sq)
    {
        assert(sq >= 0 && sq <= 63);
        return HyperbolaMasks[sq].DiagMask;
    }

    template<> uint64_t Mask<MaskType::Antidiagonal>(const unsigned int sq)
    {
        assert(sq >= 0 && sq <= 63);
        return HyperbolaMasks[sq].AntiDiagMask;
    }

    template<> uint64_t Mask<MaskType::File>(const unsigned int sq)
    {
        assert(sq >= 0 && sq <= 63);
        return HyperbolaMasks[sq].FileMask;
    }

    template<MaskType type> uint64_t Hyperbola(const uint64_t occ, const unsigned int sq)
    {
        const uint64_t o = occ & Mask<type>(sq);
        const uint64_t r = Swap(o);
        const uint64_t forward = o - (1ULL << sq);
        const uint64_t reverse = Swap(r - (1ULL << (sq ^ 56)));

        return (forward ^ reverse) & Mask<type>(sq);
    }

    uint64_t GetRankAttacks(const uint64_t occ, const int sq)
    {
        unsigned int file = sq & 7;
        unsigned int rank = sq & 56;
        unsigned char occbyte = (occ >> rank) & 2*63;
        uint64_t attacks = RankAttacks[4*occbyte + file];
        return attacks << rank;
    }
}

extern "C" {
uint64_t BBAttackBishop(const uint64_t occ, const unsigned int sq)
{
    return Detail::Hyperbola<Detail::MaskType::Diagonal>(occ, sq) | 
        Detail::Hyperbola<Detail::MaskType::Antidiagonal>(occ, sq);
}

uint64_t BBAttackRook(const uint64_t occ, const unsigned int sq)
{
    return Detail::GetRankAttacks(occ, sq) | 
        Detail::Hyperbola<Detail::MaskType::File>(occ, sq);
}

void BBAttackInit()
{
    int sq, dest;

    const uint64_t file_mask = 0x0101010101010101ULL;
    const uint64_t rank_mask = 0x00000000000000FFULL;

    for (sq = 0; sq < 64; sq++) {
        const int file = sq % 8;
        const int rank = sq / 8;

        const uint64_t bit = 1ULL << sq;

        HyperbolaMasks[sq].FileMask = (file_mask << file) & ~bit;
        HyperbolaMasks[sq].RankMask = (rank_mask << 8*rank) & ~bit;

        HyperbolaMasks[sq].DiagMask = 0;
        for (dest = sq + 9; (dest % 8) != 0 && dest < 64; dest += 9) {
            HyperbolaMasks[sq].DiagMask |= 1ULL << dest;
        }
        for (dest = sq - 7; (dest % 8) != 0 && dest >= 0; dest -= 7) {
            HyperbolaMasks[sq].DiagMask |= 1ULL << dest;
        }

        HyperbolaMasks[sq].AntiDiagMask = 0;
        for (dest = sq + 7; (dest % 8) != 7 && dest < 64; dest += 7) {
            HyperbolaMasks[sq].AntiDiagMask |= 1ULL << dest;
        }
        for (dest = sq - 9; (dest % 8) != 7 && dest >= 0; dest -= 9) {
            HyperbolaMasks[sq].AntiDiagMask |= 1ULL << dest;
        }
    }

    for (uint8_t occ = 0; occ < 64; occ++) {
        for (int file = 0; file < 8; file++) {
            int index = occ * 8 + file;

            RankAttacks[index] = 0;

            for (dest = file + 1; dest < 8; dest++) {
                RankAttacks[index] |= 1 << dest;

                if ((1 << dest) & (occ << 1)) {
                    break;
                }
            }
            
            for (dest = file - 1; dest >= 0; dest--) {
                RankAttacks[index] |= 1 << dest;

                if ((1 << dest) & (occ << 1)) {
                    break;
                }
            }
        }
    }
}
}

#endif // #ifdef USE_HYPERBOLA
