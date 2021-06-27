////////////////////////////////////////////////////////////////////////////////
//    ScopeFun Oscilloscope ( http://www.scopefun.com )
//    Copyright (C) 2016 David Košenina
//
//    This file is part of ScopeFun Oscilloscope.
//
//    ScopeFun Oscilloscope is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    ScopeFun Oscilloscope is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this ScopeFun Oscilloscope.  If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////
#include<core/purec/purec.h>

#ifdef _WINDOWS
    #include <windows.h>
#else
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #define Sleep(x) usleep((x)*1000)
#endif

int cItoA(int value, char* buffer, int base)
{
    #ifdef _WINDOWS
    _itoa(value, buffer, base);
    #else
    if(base == 10)
    {
        sprintf(buffer, "%d", value);
    }
    #endif
    return 0;
}

int cMemSet(char* dst, char val, int size)
{
    memset(dst, val, size);
    return 0;
}

int cMemCpy(char* dst, char* src, int size)
{
    memcpy(dst, src, size);
    return 0;
}

void* cMalloc(int size)
{
    return malloc(size);
}

int cFree(char* ptr)
{
    free(ptr);
    return 0;
}

int cSleep(int ms)
{
    Sleep(ms);
    return 0;
}

unsigned short cSwap16(unsigned short* value)
{
    unsigned short low  = ((*value) & 0x00FF);
    unsigned short high = ((*value) & 0xFF00);
    return (low << 8) | (high >> 8);
}

unsigned int cSwap32(unsigned int* value)
{
    unsigned int v0 = ((*value) & 0x000000FF);
    unsigned int v1 = ((*value) & 0x0000FF00);
    unsigned int v2 = ((*value) & 0x00FF0000);
    unsigned int v3 = ((*value) & 0xFF000000);
    return (v0 >> 24) | (v1 >> 4) | (v2 << 4) | (v3 << 24);
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
