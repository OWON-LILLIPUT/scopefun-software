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
#ifndef __OSC__SIGNAL__
#define __OSC__SIGNAL__

#define MAXOSCVALUE    511.f
#define NUM_SAMPLES    10000
#define NUM_FFT        1048576
#define CAPTURE_BUFFER                62464
#define CAPTURE_BUFFER_HEADER          2048
#define CAPTURE_BUFFER_PADDING          416
#define CAPTURE_BUFFER_DATA           60000

class OsciloscopeFrame;

////////////////////////////////////////////////////////////////////////////////
//
// SignalMode
//
////////////////////////////////////////////////////////////////////////////////
enum SignalMode
{
    SIGNAL_MODE_PLAY,
    SIGNAL_MODE_PAUSE,
    SIGNAL_MODE_CAPTURE,
    SIGNAL_MODE_SIMULATE,
    SIGNAL_MODE_CLEAR,
};

////////////////////////////////////////////////////////////////////////////////
//
// FrameAttribute
//
////////////////////////////////////////////////////////////////////////////////
enum FrameAttribute
{
    FRAME_ATTRIBUTE_HIDE_SIGNAL      = 1,
    FRAME_ATTRIBUTE_TRIGGERED_LED    = 2,
    FRAME_ATTRIBUTE_ROLL_DISPLAY     = 4,
};

////////////////////////////////////////////////////////////////////////////////
//
// EtsAttribute
//
////////////////////////////////////////////////////////////////////////////////
enum Etsttribute
{
    ETS_CLEAR = 1,
    ETS_PAUSE = 2,
};

////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeFrame
//
////////////////////////////////////////////////////////////////////////////////
class OsciloscopeFrame
{
public:
    time_t                     utc;
    ularge                     firstFrame;
    ularge                     thisFrame;
    ularge                     triggerTime;
    double                     captureTime;
    double                     generateTime[2];
    double                     deltaTime[2];
public:
    uint                      edgeSample;
    double                    edgeOffset;
    Array<byte, NUM_SAMPLES>   attr;
    Array<ishort, NUM_SAMPLES> analog[2];
    Array<ushort, NUM_SAMPLES> digital;
    Array<byte, 2464>          debug;
public:
    uint                       ets;
    uint                       etsAttr;
public:
    OsciloscopeFrame();
public:
    ishort getAnalogShort(uint channel, uint sample);
    float  getAnalog(uint channel, uint sample);
    double getAnalogDouble(uint channel, uint sample);
    ishort getDigital(uint channel, uint sample);
    ushort getDigitalChannels(uint sample);
public:
    int  isFull();
    void clear();
public:
    void getTime(char* buffer, int size);
    void generate(double dt, uint count, double captureStart, double captureFreq);
    bool captureHeader(byte* src, uint size, ularge captureStart, ularge captureFreq);
    bool captureData(byte* src, uint& pos, uint size);
};

////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeFFT
//
////////////////////////////////////////////////////////////////////////////////
class OsciloscopeFFT
{
public:
    double* aRe;
    double* aIm;
    double* aAmpl;
public:
    void init()
    {
        aRe   = (double*)pMemory->allocate(NUM_FFT * sizeof(double));
        aIm   = (double*)pMemory->allocate(NUM_FFT * sizeof(double));
        aAmpl = (double*)pMemory->allocate(NUM_FFT * sizeof(double));
    }

    void clear()
    {
    }
};


////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeFunction
//
////////////////////////////////////////////////////////////////////////////////

enum Token
{
    tAdd,
    tSub,
    tMul,
    tDiv,
    tMod,
    tMin,
    tMax,
    tSin,
    tCos,
    tCh0,
    tCh1,
    tNumber,
    tDouble,
    tLeft,
    tRight,
    tSeperator,
    tLast,
};

class OscToken
{
public:
    Token     type;
    double    value;
public:
    double evaluate(double par1, double par2);
    double evaluate(double par);
};

class OsciloscopeFunction
{
public:
    Array<OscToken, 128>  tokens;
    Array<OscToken, 128>  postfix;
    Array<double, 128>    stack;
public:
    void           tokenize(String input);
    void           parse();
    double         evaluate(double ch0, double ch1);
};

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
