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

#ifdef USE_MAGIC

#include "bbattack-private.h"

// Using the code kindly provided by Volker Annuss:
// http://www.talkchess.com/forum/viewtopic.php?topic_view=threads&p=670709&t=60065

static uint64_t MagicTable[89524]; // < 700KB, well done Volker!

static uint64_t BishopMask[64];
static uint64_t RookMask[64];

static const uint64_t BishopMagic[64] = {
    0x404040404040ULL, 0xa060401007fcULL, 0x401020200000ULL, 0x806004000000ULL,
    0x440200000000ULL, 0x80100800000ULL, 0x104104004000ULL, 0x20020820080ULL,
    0x40100202004ULL, 0x20080200802ULL, 0x10040080200ULL, 0x8060040000ULL,
    0x4402000000ULL, 0x21c100b200ULL, 0x400410080ULL, 0x3f7f05fffc0ULL,
    0x4228040808010ULL, 0x200040404040ULL, 0x400080808080ULL, 0x200200801000ULL,
    0x240080840000ULL, 0x18000c03fff8ULL, 0xa5840208020ULL, 0x58408404010ULL,
    0x2022000408020ULL, 0x402000408080ULL, 0x804000810100ULL, 0x100403c0403ffULL,
    0x78402a8802000ULL, 0x101000804400ULL, 0x80800104100ULL, 0x400480101008ULL,
    0x1010102004040ULL, 0x808090402020ULL, 0x7fefe08810010ULL, 0x3ff0f833fc080ULL,
    0x7fe08019003042ULL, 0x202040008040ULL, 0x1004008381008ULL, 0x802003700808ULL,
    0x208200400080ULL, 0x104100200040ULL, 0x3ffdf7f833fc0ULL, 0x8840450020ULL,
    0x20040100100ULL, 0x7fffdd80140028ULL, 0x202020200040ULL, 0x1004010039004ULL,
    0x40041008000ULL, 0x3ffefe0c02200ULL, 0x1010806000ULL, 0x08403000ULL,
    0x100202000ULL, 0x40100200800ULL, 0x404040404000ULL, 0x6020601803f4ULL,
    0x3ffdfdfc28048ULL, 0x820820020ULL, 0x10108060ULL, 0x00084030ULL,
    0x01002020ULL, 0x40408020ULL, 0x4040404040ULL, 0x404040404040ULL
};

static const uint64_t * BishopOffset[64] = {
    MagicTable+33104, MagicTable+4094, MagicTable+24764, MagicTable+13882,
    MagicTable+23090, MagicTable+32640, MagicTable+11558, MagicTable+32912,
    MagicTable+13674, MagicTable+6109, MagicTable+26494, MagicTable+17919,
    MagicTable+25757, MagicTable+17338, MagicTable+16983, MagicTable+16659,
    MagicTable+13610, MagicTable+2224, MagicTable+60405, MagicTable+7983,
    MagicTable+17, MagicTable+34321, MagicTable+33216, MagicTable+17127,
    MagicTable+6397, MagicTable+22169, MagicTable+42727, MagicTable+155,
    MagicTable+8601, MagicTable+21101, MagicTable+29885, MagicTable+29340,
    MagicTable+19785, MagicTable+12258, MagicTable+50451, MagicTable+1712,
    MagicTable+78475, MagicTable+7855, MagicTable+13642, MagicTable+8156,
    MagicTable+4348, MagicTable+28794, MagicTable+22578, MagicTable+50315,
    MagicTable+85452, MagicTable+32816, MagicTable+13930, MagicTable+17967,
    MagicTable+33200, MagicTable+32456, MagicTable+7762, MagicTable+7794,
    MagicTable+22761, MagicTable+14918, MagicTable+11620, MagicTable+15925,
    MagicTable+32528, MagicTable+12196, MagicTable+32720, MagicTable+26781,
    MagicTable+19817, MagicTable+24732, MagicTable+25468, MagicTable+10186
};

static const uint64_t RookMagic[64] = {
    0x280077ffebfffeULL, 0x2004010201097fffULL, 0x10020010053fffULL, 0x30002ff71ffffaULL,
    0x7fd00441ffffd003ULL, 0x4001d9e03ffff7ULL, 0x4000888847ffffULL, 0x6800fbff75fffdULL,
    0x28010113ffffULL, 0x20040201fcffffULL, 0x7fe80042ffffe8ULL, 0x1800217fffe8ULL,
    0x1800073fffe8ULL, 0x7fe8009effffe8ULL, 0x1800602fffe8ULL, 0x30002fffffa0ULL,
    0x300018010bffffULL, 0x3000c0085fffbULL, 0x4000802010008ULL, 0x2002004002002ULL,
    0x2002020010002ULL, 0x1002020008001ULL, 0x4040008001ULL, 0x802000200040ULL,
    0x40200010080010ULL, 0x80010040010ULL, 0x4010008020008ULL, 0x40020200200ULL,
    0x10020020020ULL, 0x10020200080ULL, 0x8020200040ULL, 0x200020004081ULL,
    0xfffd1800300030ULL, 0x7fff7fbfd40020ULL, 0x3fffbd00180018ULL, 0x1fffde80180018ULL,
    0xfffe0bfe80018ULL, 0x1000080202001ULL, 0x3fffbff980180ULL, 0x1fffdff9000e0ULL,
    0xfffeebfeffd800ULL, 0x7ffff7ffc01400ULL, 0x408104200204ULL, 0x1ffff01fc03000ULL,
    0xfffe7f8bfe800ULL, 0x8001002020ULL, 0x3fff85fffa804ULL, 0x1fffd75ffa802ULL,
    0xffffec00280028ULL, 0x7fff75ff7fbfd8ULL, 0x3fff863fbf7fd8ULL, 0x1fffbfdfd7ffd8ULL,
    0xffff810280028ULL, 0x7ffd7f7feffd8ULL, 0x3fffc0c480048ULL, 0x1ffffafd7ffd8ULL,
    0xffffe4ffdfa3baULL, 0x7fffef7ff3d3daULL, 0x3fffbfdfeff7faULL, 0x1fffeff7fbfc22ULL,
    0x20408001001ULL, 0x7fffeffff77fdULL, 0x3ffffbf7dfeecULL, 0x1ffff9dffa333ULL,
};

static const uint64_t * RookOffset[64] = {
    MagicTable+41305, MagicTable+14326, MagicTable+24477, MagicTable+8223,
    MagicTable+49795, MagicTable+60546, MagicTable+28543, MagicTable+79282,
    MagicTable+6457, MagicTable+4125, MagicTable+81021, MagicTable+42341,
    MagicTable+14139, MagicTable+19465, MagicTable+9514, MagicTable+71090,
    MagicTable+75419, MagicTable+33476, MagicTable+27117, MagicTable+85964,
    MagicTable+54915, MagicTable+36544, MagicTable+71854, MagicTable+37996,
    MagicTable+30398, MagicTable+55939, MagicTable+53891, MagicTable+56963,
    MagicTable+77451, MagicTable+12319, MagicTable+88500, MagicTable+51405,
    MagicTable+72878, MagicTable+676, MagicTable+83122, MagicTable+22206,
    MagicTable+75186, MagicTable+681, MagicTable+36453, MagicTable+20369,
    MagicTable+1981, MagicTable+13343, MagicTable+10650, MagicTable+57987,
    MagicTable+26302, MagicTable+58357, MagicTable+40546, MagicTable+0,
    MagicTable+14967, MagicTable+80361, MagicTable+40905, MagicTable+58347,
    MagicTable+20381, MagicTable+81868, MagicTable+59381, MagicTable+84404,
    MagicTable+45811, MagicTable+62898, MagicTable+45796, MagicTable+66994,
    MagicTable+67204, MagicTable+32448, MagicTable+62946, MagicTable+17005
};


// Steffan Westcott's innovation.
static uint64_t SNOOB(const uint64_t set, const uint64_t subset)
{
    return (subset - set) & set;
}

static uint64_t CalcRookMask(int sq)
{
    uint64_t result = 0;

    result |= GenMask<North, true>(sq);
    result |= GenMask<South, true>(sq);
    result |= GenMask<East,  true>(sq);
    result |= GenMask<West,  true>(sq);
   
    return result;
}

static uint64_t CalcBishopMask(int sq)
{
    uint64_t result = 0;
    
    result |= GenMask<Northeast, true>(sq);
    result |= GenMask<Southeast, true>(sq);
    result |= GenMask<Northwest, true>(sq);
    result |= GenMask<Southwest, true>(sq);

    return result;
}

// Likewise.
static uint64_t CalcRookAttacks(int sq, uint64_t block)
{
    uint64_t result = 0ULL;
    int rk = sq/8, fl = sq%8, r, f;
    for (r = rk+1; r <= 7; r++) {
        result |= (1ULL << (fl + r*8));
        if(block & (1ULL << (fl + r*8))) {
            break;
        }
    }
    for (r = rk-1; r >= 0; r--) {
        result |= (1ULL << (fl + r*8));
        if(block & (1ULL << (fl + r*8))) {
            break;
        }
    }
    for (f = fl+1; f <= 7; f++) {
        result |= (1ULL << (f + rk*8));
        if(block & (1ULL << (f + rk*8))) {
            break;
        }
    }
    for (f = fl-1; f >= 0; f--) {
        result |= (1ULL << (f + rk*8));
        if(block & (1ULL << (f + rk*8))) {
            break;
        }
    }
    return result;
}

// Likewise. At this point I'll be banned from all ICGA tournaments.
// Such is the price of laziness.
static uint64_t CalcBishopAttacks(int sq, uint64_t block)
{
    uint64_t result = 0ULL;
    int rk = sq/8, fl = sq%8, r, f;
    for(r = rk+1, f = fl+1; r <= 7 && f <= 7; r++, f++) {
        result |= (1ULL << (f + r*8));
        if(block & (1ULL << (f + r * 8))) {
            break;
        }
    }
    for(r = rk+1, f = fl-1; r <= 7 && f >= 0; r++, f--) {
        result |= (1ULL << (f + r*8));
        if(block & (1ULL << (f + r * 8))) {
            break;
        }
    }
    for(r = rk-1, f = fl+1; r >= 0 && f <= 7; r--, f++) {
        result |= (1ULL << (f + r*8));
        if(block & (1ULL << (f + r * 8))) {
            break;
        }
    }
    for(r = rk-1, f = fl-1; r >= 0 && f >= 0; r--, f--) {
        result |= (1ULL << (f + r*8));
        if(block & (1ULL << (f + r * 8))) {
            break;
        }
    }
    return result;
}

extern "C" {
uint64_t BBAttackBishop(const uint64_t occ, const unsigned int sq)
{
    return *(BishopOffset[sq] + (((occ & BishopMask[sq]) * BishopMagic[sq]) >> 55));
}

uint64_t BBAttackRook(const uint64_t occ, const unsigned int sq)
{
    return *(RookOffset[sq] + (((occ & RookMask[sq]) * RookMagic[sq]) >> 52));
}

void BBAttackInit()
{
    uint64_t b, *index;
    int sq;

    // Bishops
    for (sq = 0; sq < 64; sq++) {
        b = 0;
        BishopMask[sq] = CalcBishopMask(sq);

        do {
            index = (uint64_t*)(BishopOffset[sq] + (((b & BishopMask[sq]) * BishopMagic[sq]) >> 55));
            *index = CalcBishopAttacks(sq, b);
        } while ((b = SNOOB(BishopMask[sq], b)));
    }

    // Rooks
    for (sq = 0; sq < 64; sq++) {
        b = 0;
        RookMask[sq] = CalcRookMask(sq);

        do {
            index = (uint64_t*)(RookOffset[sq] + (((b & RookMask[sq]) * RookMagic[sq]) >> 52));
            *index = CalcRookAttacks(sq, b);
        } while ((b = SNOOB(RookMask[sq], b)));
    }
}
}
#endif // #ifdef USE_MAGIC
