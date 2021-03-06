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

//
// Pure GPS proxy (input & output)
//

#include "osw.h"
#include <string.h>

#define GPS_SERIAL_ID  0
#define USB_SERIAL_ID  1

#define GPS_SERIAL_SPEED  9600
#define USB_SERIAL_SPEED  38400

#define GPS_PDN_PORT 2
#define GPS_PDN_PIN 6

#define GPS_INIT() \
    pinAsOutput(GPS_PDN_PORT, GPS_PDN_PIN); \
    pinAsData(GPS_PDN_PORT, GPS_PDN_PIN);

#define GPS_OFF() pinSet(GPS_PDN_PORT, GPS_PDN_PIN);
#define GPS_ON()  pinClear(GPS_PDN_PORT, GPS_PDN_PIN);

void gpsCharRecv(uint8_t b)
{
    serialSendByte(USB_SERIAL_ID, b); // send to serial
}

void usbCharRecv(uint8_t b)
{
    // redLedOn();
    serialSendByte(GPS_SERIAL_ID, b); // send to GPS
}

#define NMEA_CMD_SET_BAUD_RATE  ((uint8_t *) "$PSRF100,1,9600,8,1,0*0D\r\n")

//-------------------------------------------
//      Entry point for the application
//-------------------------------------------
void appMain(void)
{
    GPS_INIT();
    GPS_ON();

    // GPS
    if (serialInit(GPS_SERIAL_ID, GPS_SERIAL_SPEED, 0)) redLedOn();
    serialEnableRX(GPS_SERIAL_ID);
    serialEnableTX(GPS_SERIAL_ID);

    // USB serial port
    if (serialInit(USB_SERIAL_ID, USB_SERIAL_SPEED, 0)) redLedOn();
    serialEnableRX(USB_SERIAL_ID);
    serialEnableTX(USB_SERIAL_ID);

    serialSetReceiveHandle(GPS_SERIAL_ID, gpsCharRecv);
    serialSetReceiveHandle(USB_SERIAL_ID, usbCharRecv);

    // serialSendString(GPS_SERIAL_ID, NMEA_CMD_SET_BAUD_RATE);
}
