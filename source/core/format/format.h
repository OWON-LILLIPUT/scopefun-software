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
#ifndef __CORE__FORMAT__
#define __CORE__FORMAT__

////////////////////////////////////////////////////////////////////////////////
//
// FormatManager
//
////////////////////////////////////////////////////////////////////////////////
class FormatManager : public Manager
{
public:
    String null;
    String space;
public:
    char        buffer[MAX_STRING];
    char        tmp[MAX_STRING];
public:
    String      currentWorkingPath;
    String      currentWorkingExe;
    String      guidString;
public:
    FormatManager();
public:
    const char* fixPath(const char* path);
    int         formatPath(char* buffer, int size, const char* path);
    int         formatString(char* buffer, int size, const char* msg, ...);
    const char* formatGuid(UsbGuid guid);
    void        setCurrentWorkingPath(const char* path);
    void        setCurrentWorkingExe(const char* exe);
public:
    const char*  doubleToString(double value);
    double       stringToDouble(const char* str);
    const char*  floatToString(float value);
    float        stringToFloat(const char* str);
    const char*  integerToString(int value);
    int          stringToInteger(const char* str);
};

MANAGER_POINTER(Format);

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
