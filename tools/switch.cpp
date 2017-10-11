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

#include "../bbattack-private.h"

static uint64_t BishopMask[64];
static uint64_t RookMask[64];

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

int main()
{
    uint64_t b, *index;
    int sq;

    puts("#include <stdint.h>");
    puts("#include \"bbattack.h\"");
    puts("#ifdef USE_SWITCH");

    // Bishop individual squares
    for (sq = 0; sq < 64; sq++) {
        printf("uint64_t Bishop%d(const uint64_t occ) {\n", sq);

        b = 0;
        BishopMask[sq] = CalcBishopMask(sq);
        printf("switch (occ & %lluULL) {\n", BishopMask[sq]);

        do {
            printf("case %lluULL: return %lluULL;\n", b, CalcBishopAttacks(sq, b));
        } while ((b = SNOOB(BishopMask[sq], b)));
        
        puts("default: __builtin_unreachable();");

        puts("}}");
    }

    // Bishop entry point
    puts("uint64_t BBAttackBishop(const uint64_t occ, const unsigned int sq) {");
    puts("switch (sq) {");

    for (sq = 0; sq < 64; sq++) {
        printf("case %d: return Bishop%d(occ);\n", sq, sq);
    }
    
    puts("default: __builtin_unreachable();");
    
    puts("}}");

    // Rook individual squares
    for (sq = 0; sq < 64; sq++) {
        printf("uint64_t Rook%d(const uint64_t occ) {\n", sq);

        b = 0;
        RookMask[sq] = CalcRookMask(sq);
        printf("switch (occ & %lluULL) {\n", RookMask[sq]);

        do {
            printf("case %lluULL: return %lluULL;\n", b, CalcRookAttacks(sq, b));
        } while ((b = SNOOB(RookMask[sq], b)));

        puts("default: __builtin_unreachable();");

        puts("}}");
    }

    // Rook entry point
    puts("uint64_t BBAttackRook(const uint64_t occ, const unsigned int sq) {");
    puts("switch (sq) {");

    for (sq = 0; sq < 64; sq++) {
        printf("case %d: return Rook%d(occ);\n", sq, sq);
    }
    
    puts("default: __builtin_unreachable();");

    puts("}}");

    // No-op init
    puts("void BBAttackInit() {}");

    puts("#endif");
}
