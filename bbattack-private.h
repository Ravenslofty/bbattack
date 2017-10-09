
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

#include "bbattack.h"

namespace BBAttack {

    template<int type>
    uint64_t Bishop(uint64_t occupancy, int square)
    {
        // Defeat warnings about unused variables.
        (void)occupancy;
        (void)square;

        static_assert(type > BBAttackStart && type < BBAttackFinish, "Invalid generation type selected");
    
        // Also defeat warnings about no return value.
        return 0;
    }
    
    template<int type>
    uint64_t Rook(uint64_t occupancy, int square)
    {
        // Defeat warnings about unused variables.
        (void)occupancy;
        (void)square;

        static_assert(type > BBAttackStart && type < BBAttackFinish, "Invalid generation type selected");
    
        // Also defeat warnings about no return value.
        return 0;
    }
    
    template<int type>
    void Init()
    {
        static_assert(type > BBAttackStart && type < BBAttackFinish, "Invalid generation type selected");
    }

    #include "bbattack-magic.h"
}

#endif // #ifndef BBATTACK_PRIVATE_H
