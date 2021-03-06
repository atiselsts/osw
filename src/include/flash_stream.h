/*
 * Copyright (c) 2013 OSW. All rights reserved.
 * Copyright (c) 2012-2013 the MansOS team. All rights reserved.
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

#ifndef OSW_FLASH_STREAM_H
#define OSW_FLASH_STREAM_H

/// \file
/// External flash stream module interface (i.e. raw access without file system)
///

#include <defines.h>

///
/// Reset the stream back to start
///
void flashStreamReset(void);

///
/// Write a record
/// @param crc  if true, then 16-bit CRC is automatically calculated and stored
/// @return     true on success, false on failure
///
bool flashStreamWriteRecord(void *data, uint16_t length, bool crc);

///
/// Read a record
/// @return     true on success, false on failure (including bad crc)
///
bool flashStreamReadRecord(void *data, uint16_t length, bool crc);

///
/// Number of reserved bytes (left unused by the stream module)
/// at the start of external flash memory
#ifndef EXT_FLASH_RESERVED
#define EXT_FLASH_RESERVED  (256 * 1024ul) // 256kb
#endif

#endif
