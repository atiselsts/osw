/*
 * Copyright (c) 2013 OSW. All rights reserved.
 * Copyright (c) 2008-2012 the MansOS team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of  conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// -- This file is part of the public OSW API --

/// \file
/// Generic algorithms
///

#ifndef OSW_ALGO_H
#define OSW_ALGO_H

#include <defines.h>


//! Swap two values
#define swap(p1, p2) \
    do {                                         \
        typeof(p1) t = p2;                       \
        p2 = p1;                                 \
        p1 = t;                                  \
    } while (0)


//! Get minimum of two values
#define min(a, b) ((a) < (b) ? (a) : (b))

//! Get maximum of two values
#define max(a, b) ((a) > (b) ? (a) : (b))


//! Calculate logarithm of x in base 2
#define log2(x) \
    ({  typeof(x) x1 = (x) >> 1; uint8_t log = 0; while (x1) { x1 >>= 1; log++; } log; })



//! Calculate square root without using floating point operations, rounded down.
uint16_t intSqrt(uint32_t);


//! Calculate approximate triangle wave value at given point of time
uint16_t signalTriangleWave(uint16_t period, uint16_t low, uint16_t high);

//! Calculate approximate sawtooth wave value at given point of time
uint16_t signalSawtoothWave(uint16_t period, uint16_t low, uint16_t high);

//! Calculate approximate sine wave value at given point of time
/// TODO: not finished!
uint16_t signalSineWave(uint16_t period, uint16_t low, uint16_t high);

///
/// Map a specific value from input range to output range.
/// Idea taken from Arduino.
///
static inline int32_t map(int32_t value,
                          int32_t inputLow, int32_t inputHigh,
                          int32_t outputLow, int32_t outputHigh)
{
    const int32_t amplitudeIn = inputHigh - inputLow;
    const int32_t amplitudeOut = outputHigh - outputLow;
    return (value - inputLow) * amplitudeOut / amplitudeIn + outputLow;
}

//! Convert half-a-byte (so called "nibble") to a hexadecimal character
static inline char toHex(uint8_t nibble) {
    switch (nibble) {
    case 0 ... 9:
        return '0' + nibble;
    case 10 ... 15:
    default:
        return 'a' + nibble - 10;
    }
}

#endif
