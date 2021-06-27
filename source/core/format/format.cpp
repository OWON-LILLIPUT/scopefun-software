////////////////////////////////////////////////////////////////////////////////
//    ScopeFun Oscilloscope ( http://www.scopefun.com )
//    Copyright (C) 2016 - 2019 David Košenina
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
#include<core/core.h>

////////////////////////////////////////////////////////////////////////////////
// globals - manager
///////////////////////////////////////////////////////////////////////////////
MANAGER_REGISTER(Format);

////////////////////////////////////////////////////////////////////////////////
//
// Format
//
////////////////////////////////////////////////////////////////////////////////
int macroString(char* buffer, int size, const char* msg, ...)
{
    va_list args;
    va_start(args, msg);
    vsprintf(buffer, msg, args);
    va_end(args);
    return 0;
}

int macroPath(char* buffer, int size, const char* path)
{
    return pFormat->formatPath(buffer, size, path);
}

FormatManager::FormatManager()
{
    null = "";
    space = " ";
}

void FormatManager::setCurrentWorkingPath(const char* path)
{
    currentWorkingPath = path;
}

void FormatManager::setCurrentWorkingExe(const char* exe)
{
    currentWorkingExe = exe;
    #ifdef PLATFORM_MAC
    String sub("/");
    for(int i = 0; i < 3; i++)
    {
        int index = currentWorkingExe.posReverse("/", 0);
        currentWorkingExe.remove(index, currentWorkingExe.getLength() - index);
    }
    #endif
}

int FormatManager::formatPath(char* buffer, int size, const char* path)
{
    #ifdef _WINDOWS
    memset(buffer, 0, size);
    for(int i = 0; i < currentWorkingPath.getLength(); i++)
    {
        buffer[i] = currentWorkingPath[i];
    }
    int plen = (int)strlen(path);
    for(int i = 0; i < plen; i++)
    {
        buffer[i + currentWorkingPath.getLength()] = path[i];
    }
    int len = (int)strlen(buffer);
    for(int i = 0; i < len; i++)
    {
        if(buffer[i] == '/')
        {
            buffer[i] = '\\';
        }
    }
    #else
    #ifdef PLATFORM_MAC
    char tmp[1024] = {0};
    formatString(tmp, 1024, "%s%s%s", currentWorkingExe.asChar(), "/Contents/Resources/", path);
    strcpy(buffer, tmp);
    #else
    char tmp[1024] = {0};
    formatString(tmp, 1024, "%s%s", currentWorkingPath.asChar(), path);
    strcpy(buffer, tmp);
    #endif
    #endif
    return 0;
}

const char* FormatManager::fixPath(const char* path)
{
    SDL_zero(tmp);
    formatPath(tmp, 1024, path);
    return tmp;
}

int FormatManager::formatString(char* buffer, int size, const char* str, ...)
{
    va_list argptr;
    va_start(argptr, str);
    vsprintf(buffer, str, argptr);
    va_end(argptr);
    return 0;
}

const char* FormatManager::formatGuid(UsbGuid guid)
{
    formatString(buffer, 1024, "0x%08x, 0x%04x, 0x%04x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x", guid.data1, guid.data2, guid.data3, guid.data4[0], guid.data4[1], guid.data4[2], guid.data4[3], guid.data4[4], guid.data4[5], guid.data4[6], guid.data4[7]);
    guidString = (const char*)buffer;
    return guidString.asChar();
}

////////////////////////////////////////////////////////////////////////////////
//
// String ...
//
////////////////////////////////////////////////////////////////////////////////
const char*  FormatManager::doubleToString(double value)
{
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "%f", value);
    return buffer;
}

double FormatManager::stringToDouble(const char* str)
{
    double d = strtod(str, 0);
    return d;
}

const char* FormatManager::floatToString(float value)
{
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "%f", value);
    return buffer;
}

float FormatManager::stringToFloat(const char* str)
{
    float f = atof(str);
    return f;
}

const char* FormatManager::integerToString(int value)
{
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "%d", value);
    return buffer;
}

int FormatManager::stringToInteger(const char* str)
{
    int i = atoi(str);
    return i;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
