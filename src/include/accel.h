/*
 * Copyright (c) 2013 OSW. All rights reserved.
 * Copyright (c) 2008-2013 the MansOS team. All rights reserved.
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

#ifndef OSW_ACCEL_H
#define OSW_ACCEL_H

/// \file
/// Accelerometer sensor reading
///

// -- List all supported accel chips here --
//! ADXL345 chip (available on Zolertia Z1 mote)
#define ACCEL_CHIP_ADXL345  1
// --

// include platform-specific interface
#include <platform.h>

//! Turn on the accelerometer sensor
extern inline void accelOn(void);
//! Turn off the accelerometer sensor
extern inline void accelOff(void);
//! Read acceleration on X axis
static inline uint16_t accelReadX(void);
//! Read acceleration on Y axis
extern inline uint16_t accelReadY(void);
//! Read acceleration on Z axis
extern inline uint16_t accelReadZ(void);

// Init accelerometer sensor, do not turn it on
extern inline void accelInit(void);

// Note: accel sensor may not support on/off

// implementation
#include <arch/accel_hal.h>

#endif
