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
#ifndef __RING__
#define __RING__

////////////////////////////////////////////////////////////////////////////////
//
// Ring
//
///////////////////////////////////////////////////////////////////////////////
template<class T> class Ring
{
private:
    ularge  maxSize;
    ularge  size;
    ularge  start;
    ularge  count;
    T*      data;
public:
    Ring() : size(0), start(0), count(0), data(0) {};
public:
    void init(T* buffer, ularge size)
    {
        this->size    = size;
        this->start   = 0;
        this->count   = 0;
        this->data    = buffer;
        this->maxSize = size;
    }
public:
    T* getData()
    {
        return data;
    }

    void clear()
    {
        start = 0;
        count = 0;
    }

    int isFull()
    {
        return count == size;
    }

    int isEmpty()
    {
        return count == 0;
    }

    void offset(ularge amount)
    {
        start = (start + amount) % size;
    }

    ularge getCount()
    {
        return count;
    }

    void setCount(ularge cnt)
    {
        count = min<uint>(cnt, size);
    }

    void setSize(ularge isize)
    {
        size = min<ularge>(isize, maxSize);
    }

    ularge getSize()
    {
        return size;
    }

    void setStart(ularge istart)
    {
        start = min(istart, size);
    }

    ularge getStart()
    {
        return start;
    }

    ularge getLast()
    {
        if(count == 0)
        {
            return start;
        }
        ularge ret = (start + count - 1) % size;
        return ret;
    }

    ularge getWrite()
    {
        return (start + count) % size;
    }

    ularge getRead()
    {
        return start;
    }

    void write(T& elem)
    {
        uint end = (start + count) % size;
        data[end] = elem;
        if(count < size)
        {
            count++;
        }
        else
        {
            start = (start + 1) % size;
        }
    }

    void read(T& elem)
    {
        elem  = data[start];
        start = (start + 1) % size;
        --count;
    }

    void free(ularge icount)
    {
        for(uint i = 0; i < icount; i++)
        {
            start = (start + 1) % size;
            --count;
        }
    }

    T* peek(ularge index)
    {
        return &data[index];
    }
};

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
