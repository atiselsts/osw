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

#include "ads8328.h"
#include <spi.h>
#include <delay.h>
#include <print.h>
#include <serial.h>

// SCLK frequency can be up to 50 MHz

#define ADS8328_SPI_ENABLE()   spiSlaveEnable(ADS8328_CS_PORT, ADS8328_CS_PIN)
#define ADS8328_SPI_DISABLE()  spiSlaveDisable(ADS8328_CS_PORT, ADS8328_CS_PIN)

// TODO: use this! (?)
static uint8_t adsRange;

uint16_t ads8328ReadData(void)
{
    uint16_t result;

    ADS8328_SPI_ENABLE();
    result = spiReadByte(ADS8328_SPI_ID);
    result <<= 8;
    result |= spiReadByte(ADS8328_SPI_ID);
    ADS8328_SPI_DISABLE();

    return result;
}

uint16_t ads8328ConfigRegRead(void)
{ 
    uint16_t result;

    ADS8328_SPI_ENABLE();
    sw_spiWriteNibble(ADS8328_REG_CONFIG_READ);
    ADS8328_SPI_DISABLE();

    udelay(1);

    ADS8328_SPI_ENABLE();
    result = spiReadByte(ADS8328_SPI_ID);
    result <<= 8;
    result |= spiReadByte(ADS8328_SPI_ID);
    ADS8328_SPI_DISABLE();

    // use 12 least-significant bits
    result &= 0xfff;
    return result;
}

void ads8328RegWrite4(uint8_t address)
{
    ADS8328_SPI_ENABLE();
    sw_spiWriteNibble(address);
    ADS8328_SPI_DISABLE();
}

void ads8328RegWrite16(uint8_t address, uint16_t data)
{
    ADS8328_SPI_ENABLE();
    sw_spiWriteNibble(address);
    sw_spiWriteNibble(data >> 8);
    spiWriteByte(ADS8328_SPI_ID, data & 0xff);
    ADS8328_SPI_DISABLE();
}

void ads8328Init(void)
{
    spiBusInit(ADS8328_SPI_ID, SPI_MODE_MASTER);

    pinAsOutput(ADS8328_CS_PORT, ADS8328_CS_PIN);

    // setup Conversion Start pin
    pinAsOutput(ADS8328_CONVST_PORT, ADS8328_CONVST_PIN);
    pinSet(ADS8328_CONVST_PORT, ADS8328_CONVST_PIN);

    // setup End of Conversion pin (low while conversion in progress)
    pinAsInput(ADS8328_EOC_PORT, ADS8328_EOC_PIN);

    // turn on internal ref
    // ads8328RegWrite(ADS8328_REG_AUX_CONFIG, ADS8328_INTERNAL_VREF_ON);

    // select channel 0 with configured range
    ads8638SelectChannel(ADS8638_CHANNEL_0, ADS8638_RANGE_CONFIG);
}

void ads8638SelectChannel(uint8_t channel, uint8_t range)
{
    adsRange = range;

    ads8328RegWrite4(channel == ADS8638_CHANNEL_0 ?
            ADS8328_REG_CHANNEL0 :
            ADS8328_REG_CHANNEL1);
}

bool ads8328Read(uint16_t *value)
{
    // start the conversion
    pinClear(ADS8328_CONVST_PORT, ADS8328_CONVST_PIN);
    udelay(1);
    pinSet(ADS8328_CONVST_PORT, ADS8328_CONVST_PIN);

    // wait for end of conversion
    while (!pinRead(ADS8328_EOC_PORT, ADS8328_EOC_PIN));

    // read the result
    *value = ads8328ReadData();

    return true;
}
