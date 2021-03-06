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

#include "osw.h"
#include <assert.h>

#define FILE_NAME "newfile1.txt"

void appMain(void)
{
    static FILE fileBuffer;
    FILE *f = fopenEx(FILE_NAME, "w", &fileBuffer);
    ASSERT(f);
    ASSERT(f->isOpened);
    fclose(f);
    ASSERT(!f->isOpened);

    f = fopenEx(FILE_NAME, "r", &fileBuffer);
    ASSERT(f);
    fclose(f);

    f = fopenEx(FILE_NAME, "a", &fileBuffer);
    fwrite("hello " FILE_NAME, 1, 18, f);
    fclose(f);

    remove(FILE_NAME);
    f = fopenEx(FILE_NAME, "r", &fileBuffer);
    ASSERT(!f);

    f = fopenEx("hello.txt", "w", &fileBuffer);
    fwrite("hello world", 1, 11, f);
    fclose(f);

    PRINTF("done!\n");
}
