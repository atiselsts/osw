/*
 * Copyright (c) 2013 OSW. All rights reserved.
 * Copyright (c) 2012 the MansOS team. All rights reserved.
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

#ifndef ADC_DAC_PINS_H
#define ADC_DAC_PINS_H

#include "ads8638/ads8638.h"
#include "ads8328/ads8328.h"
#include "dac7718/dac7718.h"

#ifdef USE_SOFT_SPI
#define ADS8328_SPI_ID SPI_BUS_SW
#define ADS8638_SPI_ID SPI_BUS_SW
#define DAC7718_SPI_ID SPI_BUS_SW
#else
// id 2 is A1
#define ADS8328_SPI_ID  2
// id 3 is B1
#define ADS8638_SPI_ID  3
#define DAC7718_SPI_ID  3
#endif

// Testbed platform uses, first, ADS8328 analog/digital converter

#define ADS8328_CS_PORT 5
#define ADS8328_CS_PIN  3

#define ADS8328_CONVST_PORT 5
#define ADS8328_CONVST_PIN  5

#define ADS8328_EOC_PORT 5
#define ADS8328_EOC_PIN  4

// Second, ADS8638 analog/digital converter

#define ADS8638_CS_PORT 5
#define ADS8638_CS_PIN  0

// Testbed platform uses DAC7718 digital/analog converter

#define DAC7718_CS_PORT   5
#define DAC7718_CS_PIN    4

#define DAC7718_RST_PORT  5
#define DAC7718_RST_PIN   7

// selects output voltage after reset
#define DAC7718_RSTSEL_PORT 5
#define DAC7718_RSTSEL_PIN  5

// fix DAC latch (active low)
#define DAC7718_LDAC_PORT 5
#define DAC7718_LDAC_PIN  6

#endif
