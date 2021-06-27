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
#ifndef __OSCILOSCOPE__TOOL__WINDOW__
#define __OSCILOSCOPE__TOOL__WINDOW__


////////////////////////////////////////////////////////////////////////////////
//
// ECallibrationType
//
////////////////////////////////////////////////////////////////////////////////
enum ECallibrationType
{
    ctNormal = 0,
    ct500Mhz = 1,
    ctLast = 2,
};

////////////////////////////////////////////////////////////////////////////////
//
// ToolTime
//
////////////////////////////////////////////////////////////////////////////////
enum Time
{
    tSecond,
    tMili,
    tMicro,
    tNano,
};

////////////////////////////////////////////////////////////////////////////////
//
// ToolVolt
//
////////////////////////////////////////////////////////////////////////////////
enum Volt
{
    vVolt,
    vMili,
    vMicro,
    vNano,
};

////////////////////////////////////////////////////////////////////////////////
//
// ToolFrequency
//
////////////////////////////////////////////////////////////////////////////////
enum Frequency
{
    fHertz,
    fKilo,
    fMega,
    fGiga,
};

////////////////////////////////////////////////////////////////////////////////
//
// ToolCaptureTime
//
////////////////////////////////////////////////////////////////////////////////
enum TimeCapture
{
    tc10ns,
    tc20ns,
    tc50ns,
    tc100ns,
    tc200ns,
    tc500ns,
    tc1us,
    tc2us,
    tc5us,
    tc10us,
    tc20us,
    tc50us,
    tc100us,
    tc200us,
    tc500us,
    tc1ms,
    tc2ms,
    tc5ms,
    tc10ms,
    tc20ms,
    tc50ms,
    tc100ms,
    tc200ms,
    tc500ms,
    tc1s,
    tcLast,
};

enum TimeCapture2
{
    t2c2ns,
    t2c4ns,
    t2c8ns,
    t2c20ns,
    t2c40ns,
    t2c80ns,
    t2c200ns,
    t2c400ns,
    t2c800ns,
    t2c2us,
    t2c4us,
    t2c8us,
    t2c20us,
    t2c40us,
    t2c80us,
    t2c200us,
    t2c400us,
    t2c800us,
    t2c2ms,
    t2c4ms,
    t2c8ms,
    t2c20ms,
    t2cLast,
};

////////////////////////////////////////////////////////////////////////////////
//
// ToolCaptureVolt
//
////////////////////////////////////////////////////////////////////////////////
enum VoltageCapture
{
    vc2Volt,
    vc1Volt,
    vc500Mili,
    vc200Mili,
    vc100Mili,
    vc50Mili,
    vc20Mili,
    vc10Mili,
    vcLast,
};

////////////////////////////////////////////////////////////////////////////////
//
// Functions
//
////////////////////////////////////////////////////////////////////////////////
class ToolText
{
public:
    static void Time(char* buffer, int size, double value);
    static void Hertz(char* buffer, int size, float value);
    static void Decibels(char* buffer, int size, float value);
    static void Volt(char* buffer, int size, float value);
};

////////////////////////////////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////////////////////
double multiplyerDoubleFromValue(double value);
int    multiplyerIndexFromValue(double value);

float multiplyerFromEnum(int enumerated);
uint  multiplyerFromValue(float value);

const char* captureTimeToStr(int enumerated);
double captureTimeFromEnum(int enumerated);
double captureTimeFromEnumV1(int enumerated);
double captureTimeFromEnumV2(int enumerated);
double captureTimeFromEnumVersion(int enumerated, int version);
uint   captureTimeMaxReceive(int enumerated, int version);
uint  captureTimeFromValue(float value);
ECallibrationType getCallibrationType(int enumerated);
ECallibrationType getCallibrationType(float value);

float captureVoltFromEnum(int enumerated);
uint  captureVoltFromValue(float value);

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
