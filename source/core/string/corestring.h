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
#ifndef __CORE__STRING__
#define __CORE__STRING__

#define MAX_STRING       1024
#define MAX_BUFFER       (1024*1024)

////////////////////////////////////////////////////////////////////////////////
//
// String
//
////////////////////////////////////////////////////////////////////////////////
class String
{
private:
    char  data[MAX_STRING];
    int   length;
public:
    String();
    String(const char* str);
    String(const char  c);
public:
    ~String();
public:
    void setLength(int newlength);
    int  getLength() const;
public:
    char&          index(int idx);
    const char&    index(int idx) const;
    int            pos(const String& substr, int start = 0) const;
    int            posReverse(const String& substr, int end = 0) const;
    void           insert(int pos, const String& str);
    void           remove(int pos, int count);
public:
    void toUpperCase();
    void toLowerCase();
public:
    char*    asChar() const;
    char*    asWide() const;
    int      asInteger() const;
    float    asFloat() const;
public:
    void copyTo(String& out, int start, int count) const;
    void replace(const String thisstr, const String withthis, int start = 0);
    void tokenize(const String delimitor, Array<String, 16>& aToken) const;
public:
    operator int() const;
    const char& operator[](int idx) const;
    char&       operator[](int idx);
public:
    String& operator = (const String& str);
    int     operator == (const String& str) const;
    int     operator != (const String& str) const;
    int     operator < (const String& str) const;
    int     operator > (const String& str) const;
};

class StringBuffer
{
    char data[MAX_BUFFER];
    int  pos;
public:
    StringBuffer()
    {
        clear();
    }
    void clear()
    {
        SDL_memset(data, 0, MAX_BUFFER);
        pos = 0;
    }
public:
    char* get()
    {
        return data;
    }
    void add(const char* str);
    int     operator << (const char* str);
    int     operator << (int integer);
    int     operator << (float floating);
    int     operator << (double floating);
};

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
