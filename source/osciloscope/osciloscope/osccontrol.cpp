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
#include<osciloscope/osciloscope.h>

ECallibrationType getCallibrationType(int enumerated);
ECallibrationType getCallibrationType(float value);

////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeControl1
//
////////////////////////////////////////////////////////////////////////////////
OsciloscopeControl1::OsciloscopeControl1()
{
    control = 0;
    vgaina = 0;
    vgainb = 0;
    offseta = 0;
    offsetb = 0;
    siggen = 0;
    trigger = 0;
    analogswitch = 0;
    triggerMode = 0;
    triggerSource = 0;
    triggerSlope = 0;
    triggerLevel = 0;
    triggerHis = 0;
    triggerPercent = 0;
    xRange = 0;
    sampleSize = 0;
    holdoffH = 0;
    holdoffL = 0;
    generatorType0 = 0;
    generatorVoltage0 = 0;
    generatorOffset0 = 0;
    generatorDeltaH0 = 0;
    generatorDeltaL0 = 0;
    generatorSqueareDuty0 = 0;
    generatorType1 = 0;
    generatorVoltage1 = 0;
    generatorOffset1 = 0;
    generatorDeltaH1 = 0;
    generatorDeltaL1 = 0;
    generatorSqueareDuty1 = 0;
    digitalPattern1a = 0;
    digitalPattern1b = 0;
    digitalMask1 = 0;
    digitalPattern2a = 0;
    digitalPattern2b = 0;
    digitalMask2 = 0;
    digitalPattern3a = 0;
    digitalPattern3b = 0;
    digitalMask3 = 0;
    digitalPattern4a = 0;
    digitalPattern4b = 0;
    digitalMask4 = 0;
    dt_delayMaxcnt1 = 0;
    dt_delayMaxcnt2 = 0;
    dt_delayMaxcnt3 = 0;
    dt_delayMaxcnt4 = 0;
    dt_control = 0;
    digitalVoltage = 0;
    digitalInputOutput = 0;
    digitalOutputBit = 0;
    digitalOutputMask = 0;
    digitalClkDivideH = 0;
    digitalClkDivideL = 0;
}

void OsciloscopeControl1::Default()
{
    control = CONTROLL1_NORMAL;
    vgaina = 0xff25;
    vgainb = 0xff25;
    sampleSize = NUM_SAMPLES;
    setGeneratorType0(GENERATOR_SIN);
    setGeneratorFrequency0(50.f);
    setGeneratorType1(GENERATOR_SIN);
    setGeneratorFrequency1(50.f);
}

ushort OsciloscopeControl1::getAttr(uint volt)
{
    return pOsciloscope->settings.getHardware()->referenceGainAttr[volt];
}

ushort OsciloscopeControl1::getGain(int channel, uint volt)
{
    float capture = pOsciloscope->window.horizontal.Capture;
    channel = clamp(channel, 0, 1);
    return pOsciloscope->settings.getHardware()->callibratedGainValue[getCallibrationType(capture)][channel][volt];
}

uint getVolt(int channel, ushort gain)
{
    float capture = pOsciloscope->window.horizontal.Capture;
    for(int i = 0; i < vcLast; i++)
    {
        if(pOsciloscope->settings.getHardware()->callibratedGainValue[getCallibrationType(capture)][channel][i] == gain)
        {
            return i;
        }
    }
    return 0;
}

void OsciloscopeControl1::setAnalogSwitchBit(int bit, int value)
{
    Flag16 flags;
    flags.set(analogswitch);
    flags.bit(bit, value);
    analogswitch = flags.get();
}

void OsciloscopeControl1::setEts(int enable)
{
    if(enable)
    {
        analogswitch = raiseFlag16(analogswitch, CHANNEL_ETS);
    }
    else
    {
        analogswitch = lowerFlag16(analogswitch, CHANNEL_ETS);
    }
}

void OsciloscopeControl1::setYRangeScaleA(uint voltage, float scale)
{
    ushort   attr = getAttr(voltage);
    ushort   gain = getGain(0, voltage);
    if(attr)
    {
        analogswitch = raiseFlag16(analogswitch, CHANNEL_ATTR_A);
    }
    else
    {
        analogswitch = lowerFlag16(analogswitch, CHANNEL_ATTR_A);
    }
    vgaina = gain;
}

void OsciloscopeControl1::setYRangeScaleA(uint gain, uint attr)
{
    if(attr)
    {
        analogswitch = raiseFlag16(analogswitch, CHANNEL_ATTR_A);
    }
    else
    {
        analogswitch = lowerFlag16(analogswitch, CHANNEL_ATTR_A);
    }
    vgaina = gain;
}

void OsciloscopeControl1::setYPositionA(int pos)
{
    pos = min(pos, 1500);
    pos = max(pos, -1500);
    offseta = pos;
}

void OsciloscopeControl1::setYRangeScaleB(uint voltage, float scale)
{
    ushort   attr = getAttr(voltage);
    ushort   gain = getGain(1, voltage);
    if(attr)
    {
        analogswitch = raiseFlag16(analogswitch, CHANNEL_ATTR_B);
    }
    else
    {
        analogswitch = lowerFlag16(analogswitch, CHANNEL_ATTR_B);
    }
    vgainb = gain;
}

void OsciloscopeControl1::setYRangeScaleB(uint gain, uint attr)
{
    if(attr)
    {
        analogswitch = raiseFlag16(analogswitch, CHANNEL_ATTR_B);
    }
    else
    {
        analogswitch = lowerFlag16(analogswitch, CHANNEL_ATTR_B);
    }
    vgainb = gain;
}

void OsciloscopeControl1::setYPositionB(int pos)
{
    pos = min(pos,  1500);
    pos = max(pos, -1500);
    offsetb = pos;
}

void OsciloscopeControl1::setXRange(ishort range)
{
    xRange = range;
}

void OsciloscopeControl1::setControl(uint selected)
{
    switch(selected)
    {
        case 0:
            control = CONTROLL1_NORMAL;
            break;
        case 1:
            control = CONTROLL1_TEST;
            break;
        case 2:
            control = CONTROLL1_SHUTDOWN;
            break;
    };
}

void OsciloscopeControl1::setSampleSize(uint value)
{
    uint size = 50 * (value / 50);
    size = max(50u, size);
    size = min((uint)NUM_SAMPLES, size);
    sampleSize = size;
}


void OsciloscopeControl1::setTriggerSource(int value)
{
    triggerSource = value;
}

void OsciloscopeControl1::setTriggerReArm(bool on)
{
    ushort mask = (1 << 2);
    if(on)
    {
        triggerMode = triggerMode | mask;
    }
    else
    {
        triggerMode = triggerMode & ~mask;
    }
}

void OsciloscopeControl1::setTriggerMode(int value)
{
    triggerMode = value;
}

void OsciloscopeControl1::setTriggerSlope(int value)
{
    triggerSlope = value;
}

void OsciloscopeControl1::setTriggerPre(float perc)
{
    uint sampleSize = getSampleSize();
    triggerPercent  = (perc / 100.f) * float(sampleSize - 1);
    triggerPercent  = (triggerPercent / 4);
}

void OsciloscopeControl1::setTriggerHis(int perc)
{
    triggerHis = perc;
}

void OsciloscopeControl1::setTriggerLevel(int perc)
{
    triggerLevel = perc;
}

void OsciloscopeControl1::setHoldoff(uint holdoff)
{
    holdoffH = (holdoff & 0xFFFF0000) >> 16;
    holdoffL = holdoff & 0x0000FFFF;
}

///////////////////////////////////////////////////////////////////////
// Digital
///////////////////////////////////////////////////////////////////////
void OsciloscopeControl1::setDigitalStart(int start)
{
    Flag16 startBits;
    startBits.set(start);
    Flag16 controlBits;
    controlBits.set(dt_control);
    controlBits.bit(1 << 8, startBits.is(1));
    controlBits.bit(1 << 9, startBits.is(2));
    dt_control = controlBits.get();
}

void OsciloscopeControl1::setDigitalMode(int mode)
{
    Flag16 modeBits;
    modeBits.set(mode);
    Flag16 controlBits;
    controlBits.set(dt_control);
    controlBits.bit(1 << 4, modeBits.is(1));
    dt_control = controlBits.get();
}

void OsciloscopeControl1::setDigitalChannel(int channel)
{
    Flag16 channelBits;
    channelBits.set(channel);
    Flag16 controlBits;
    controlBits.set(dt_control);
    controlBits.bit(1 << 0, channelBits.is(1));
    controlBits.bit(1 << 1, channelBits.is(2));
    controlBits.bit(1 << 2, channelBits.is(4));
    controlBits.bit(1 << 3, channelBits.is(8));
    dt_control = controlBits.get();
}

///////////////////////////////////////////////////////////////////////
// setDigital - stage
///////////////////////////////////////////////////////////////////////
void OsciloscopeControl1::setDigitalDelay(DigitalStage stage, ushort delay)
{
    switch(stage)
    {
        case 0:
            dt_delayMaxcnt1 = delay;
            break;
        case 1:
            dt_delayMaxcnt2 = delay;
            break;
        case 2:
            dt_delayMaxcnt3 = delay;
            break;
        case 3:
            dt_delayMaxcnt4 = delay;
            break;
        default:
            CORE_ERROR("invalid digital stage", 0);
    };
}

void OsciloscopeControl1::setDigitalMask(DigitalStage stage, DigitalBit bit, int value)
{
    Flag16 mask;
    switch(stage)
    {
        case 0:
            mask.set(digitalMask1);
            break;
        case 1:
            mask.set(digitalMask2);
            break;
        case 2:
            mask.set(digitalMask3);
            break;
        case 3:
            mask.set(digitalMask4);
            break;
        default:
            CORE_ERROR("invalid digital stage", 0);
    };
    //
    if(value)
    {
        mask.raise(BIT(bit));
    }
    else
    {
        mask.lower(BIT(bit));
    }
    //
    switch(stage)
    {
        case 0:
            digitalMask1 = mask.get();
            break;
        case 1:
            digitalMask2 = mask.get();
            break;
        case 2:
            digitalMask3 = mask.get();
            break;
        case 3:
            digitalMask4 = mask.get();
            break;
        default:
            CORE_ERROR("invalid digital stage", 0);
    };
}

void OsciloscopeControl1::setDigitalPattern(DigitalStage stage, DigitalBit bit, DigitalPattern pattern)
{
    Flag16 patternA;
    Flag16 patternB;
    switch(stage)
    {
        case 0:
            patternA.set(digitalPattern1a);
            patternB.set(digitalPattern1b);
            break;
        case 1:
            patternA.set(digitalPattern2a);
            patternB.set(digitalPattern2b);
            break;
        case 2:
            patternA.set(digitalPattern3a);
            patternB.set(digitalPattern3b);
            break;
        case 3:
            patternA.set(digitalPattern4a);
            patternB.set(digitalPattern4b);
            break;
        default:
            CORE_ERROR("invalid digital stage", 0);
    };
    //
    switch(pattern)
    {
        case DIGITAL_PATTERN_0: // 00
            patternA.lower(BIT(bit));
            patternB.lower(BIT(bit));
            break;
        case DIGITAL_PATTERN_1: // 11
            patternA.raise(BIT(bit));
            patternB.raise(BIT(bit));
            break;
        case DIGITAL_PATTERN_RISING: // 01
            patternA.lower(BIT(bit));
            patternB.raise(BIT(bit));
            break;
        case DIGITAL_PATTERN_FALLING: // 10
            patternA.raise(BIT(bit));
            patternB.lower(BIT(bit));
            break;
        default:
            CORE_ERROR("invalid digital stage", 0);
    };
    //
    //
    switch(stage)
    {
        case 0:
            digitalPattern1a = patternA.get();
            digitalPattern1b = patternB.get();
            break;
        case 1:
            digitalPattern2a = patternA.get();
            digitalPattern2b = patternB.get();
            break;
        case 2:
            digitalPattern3a = patternA.get();
            digitalPattern3b = patternB.get();
            break;
        case 3:
            digitalPattern4a = patternA.get();
            digitalPattern4b = patternB.get();
            break;
        default:
            CORE_ERROR("invalid digital stage", 0);
    };
}


///////////////////////////////////////////////////////////////////////
// generator 0
///////////////////////////////////////////////////////////////////////
void OsciloscopeControl1::setGeneratorType0(GeneratorType type)
{
    uint mask = 1 << 8;
    if(generatorType0 & mask)
    {
        generatorType0 = type | mask;
    }
    else
    {
        generatorType0 = type;
    }
}

void OsciloscopeControl1::setGeneratorOn0(int onoff)
{
    uint mask = 1 << 8;
    if(onoff)
    {
        generatorType0 = generatorType0 | mask;
    }
    else
    {
        generatorType0 = generatorType0 & ~mask;
    }
}

void OsciloscopeControl1::setGeneratorSlope0(int onoff)
{
}

void OsciloscopeControl1::setGeneratorVoltage0(int volt)
{
    uint voltage = volt;
    uint mask = 1 << 12;
    if(generatorVoltage0 & mask)
    {
        generatorVoltage0 = voltage | mask;
    }
    else
    {
        generatorVoltage0 = voltage;
    }
}

void OsciloscopeControl1::setGeneratorOffset0(int perc)
{
    generatorOffset0 = perc;
}

void OsciloscopeControl1::setGeneratorFrequency0(float freq, float fs)
{
    double delta = 0;
    uint mask = (1 << 8);
    mask = ~mask;
    uint type = generatorType0 & mask;
    switch(type)
    {
        case GENERATOR_CUSTOM:
        case GENERATOR_SIN:
        case GENERATOR_COS:
        case GENERATOR_TRIANGLE:
        case GENERATOR_RAMP_UP:
        case GENERATOR_RAMP_DOWN:
        case GENERATOR_SQUARE:
        case GENERATOR_DELTA:
        case GENERATOR_DC:
        case GENERATOR_NOISE:
            delta = 8192.0 * double(freq) / double(fs);
            break;
    };
    //
    uint gDelta = uint(delta * 1048575.0);
    generatorDeltaH0 = (gDelta & 0xFFFF0000) >> 16;
    generatorDeltaL0 = gDelta & 0x0000FFFF;
}

void OsciloscopeControl1::setGeneratorSquareDuty0(float perc)
{
    generatorSqueareDuty0 = perc*2047.0/100.0;
}

///////////////////////////////////////////////////////////////////////
// generator 1
///////////////////////////////////////////////////////////////////////

void OsciloscopeControl1::setGeneratorType1(GeneratorType type)
{
    uint mask = 1 << 8;
    if(generatorType1 & mask)
    {
        generatorType1 = type | mask;
    }
    else
    {
        generatorType1 = type;
    }
}

void OsciloscopeControl1::setGeneratorOn1(int onoff)
{
    uint mask = 1 << 8;
    if(onoff)
    {
        generatorType1 = generatorType1 | mask;
    }
    else
    {
        generatorType1 = generatorType1 & ~mask;
    }
}

void OsciloscopeControl1::setGeneratorSlope1(int onoff)
{
}

void OsciloscopeControl1::setGeneratorVoltage1(int volt)
{
    uint voltage = volt;
    uint mask = 1 << 12;
    if(generatorVoltage1 & mask)
    {
        generatorVoltage1 = voltage | mask;
    }
    else
    {
        generatorVoltage1 = voltage;
    }
}

void OsciloscopeControl1::setGeneratorOffset1(int perc)
{
    generatorOffset1 = perc;
}

void OsciloscopeControl1::setGeneratorFrequency1(float freq, float fs)
{
    double delta = 0;
    uint mask = (1 << 8);
    mask = ~mask;
    uint type = generatorType1 & mask;
    switch(type)
    {
        case GENERATOR_CUSTOM:
        case GENERATOR_SIN:
        case GENERATOR_COS:
        case GENERATOR_TRIANGLE:
        case GENERATOR_RAMP_UP:
        case GENERATOR_RAMP_DOWN:
        case GENERATOR_SQUARE:
        case GENERATOR_DELTA:
        case GENERATOR_DC:
        case GENERATOR_NOISE:
            delta = 8192.0 * double(freq) / double(fs);
            break;
    };
    //
    uint gDelta = uint(delta * 1048575.0);
    generatorDeltaH1 = (gDelta & 0xFFFF0000) >> 16;
    generatorDeltaL1 = gDelta & 0x0000FFFF;
}

void OsciloscopeControl1::setGeneratorSquareDuty1(float perc)
{
    generatorSqueareDuty1 = perc*2047.0/100.0;
}

void OsciloscopeControl1::setDigitalVoltage(double volt)
{
    double kDigital = pOsciloscope->settings.getHardware()->digitalVoltageCoeficient;
    double Vmin = 1.25 * ((0.0 / kDigital) + 1.0);
    double Vmax = 1.25 * ((255.0 / kDigital) + 1.0);
    double dVolt = clamp(double(volt), Vmin, Vmax);
    digitalVoltage = ushort(((dVolt / 1.25) - 1.0) * kDigital);
}

void OsciloscopeControl1::setDigitalInputOutput(int inout15, int inout7)
{
    // 00 = OUT, OUT
    // 01 = OUT, IN
    // 10 = IN, OUT
    // 11 = IN, IN
    digitalInputOutput = (inout15 << 1) | inout7;
}

void OsciloscopeControl1::setDigitalOutputBit(int bit, int onoff)
{
    Flag16 output, mask;
    output.set(digitalOutputBit);
    mask.set(digitalOutputMask);
    if(onoff == 0 || onoff == 1)
    {
        output.bit(BIT(bit), onoff);
        mask.bit(BIT(bit), 1);
    }
    else if(onoff == 2)
    {
        output.bit(BIT(bit), onoff);
        mask.bit(BIT(bit), 0);
    }
    digitalOutputBit = output.get();
    digitalOutputMask = mask.get();
}

void OsciloscopeControl1::setDigitalClockDivide(uint divider)
{
    digitalClkDivideH = (divider & 0xFFFF0000) >> 16;
    digitalClkDivideL = divider & 0x0000FFFF;
}


///////////////////////////////////////////////////////////////////////
// get
///////////////////////////////////////////////////////////////////////
ushort OsciloscopeControl1::getAnalogSwitch()
{
    return analogswitch;
}

int OsciloscopeControl1::getEts()
{
    Flag16 tmp;
    tmp.set(analogswitch);
    return tmp.is(CHANNEL_ETS);
}

uint OsciloscopeControl1::getControl()
{
    switch(control)
    {
        case CONTROLL1_NORMAL:
            return 0;
        case CONTROLL1_TEST:
            return 1;
        case CONTROLL1_SHUTDOWN:
            return 2;
    };
    return 0;
}

uint OsciloscopeControl1::getYRangeA()
{
    return getVolt(0, vgaina);
}

float OsciloscopeControl1::getYScaleA()
{
    return 0.f;
}
int OsciloscopeControl1::getYPositionA()
{
    return (int)offseta;
}

uint OsciloscopeControl1::getYRangeB()
{
    return getVolt(1, vgainb);
}
float OsciloscopeControl1::getYScaleB()
{
    return 0.f;
}
int OsciloscopeControl1::getYPositionB()
{
    return (int)offsetb;
}

ushort OsciloscopeControl1::getTriggerSource()
{
    return triggerSource;
}

ushort OsciloscopeControl1::getTriggerMode()
{
    return triggerMode;
}

ushort OsciloscopeControl1::getTriggerSlope()
{
    return triggerSlope;
}

float OsciloscopeControl1::getTriggerPre()
{
    return 100.f * (float(triggerPercent * 4) / float(getSampleSize() - 1));
}

int OsciloscopeControl1::getTriggerHis()
{
    return triggerHis;
}

int OsciloscopeControl1::getTriggerLevel()
{
    return triggerLevel;
}

ishort OsciloscopeControl1::getXRange()
{
    return xRange;
}

uint OsciloscopeControl1::getSampleSize()
{
    return sampleSize;
}

uint OsciloscopeControl1::getHoldoff()
{
    uint holdOffInt = uint(holdoffL) | (uint(holdoffH) << 16);
    return holdOffInt;
}

int OsciloscopeControl1::getDigitalStart()
{
    return (dt_control & 0x300) >> 8;
}

int OsciloscopeControl1::getDigitalMode()
{
    return (dt_control & 0x10) >> 4;
}

int OsciloscopeControl1::getDigitalChannel()
{
    return dt_control & 0xF;
}

ushort OsciloscopeControl1::getDigitalDelay(DigitalStage stage)
{
    switch(stage)
    {
        case 0:
            return dt_delayMaxcnt1;
        case 1:
            return dt_delayMaxcnt2;
        case 2:
            return dt_delayMaxcnt3;
        case 3:
            return dt_delayMaxcnt4;
        default:
            CORE_ERROR("invalid digital stage", 0);
    };
    return 0;
}

int OsciloscopeControl1::getDigitalMask(DigitalStage stage, DigitalBit bit)
{
    Flag16 mask;
    switch(stage)
    {
        case 0:
            mask.set(digitalMask1);
            break;
        case 1:
            mask.set(digitalMask2);
            break;
        case 2:
            mask.set(digitalMask3);
            break;
        case 3:
            mask.set(digitalMask4);
            break;
        default:
            CORE_ERROR("invalid digital stage", 0);
    };
    return mask.is(BIT(bit));
}

DigitalPattern OsciloscopeControl1::getDigitalPattern(DigitalStage stage, DigitalBit bit)
{
    Flag16 patternA;
    Flag16 patternB;
    switch(stage)
    {
        case 0:
            patternA.set(digitalPattern1a);
            patternB.set(digitalPattern1b);
            break;
        case 1:
            patternA.set(digitalPattern2a);
            patternB.set(digitalPattern2b);
            break;
        case 2:
            patternA.set(digitalPattern3a);
            patternB.set(digitalPattern3b);
            break;
        case 3:
            patternA.set(digitalPattern4a);
            patternB.set(digitalPattern4b);
            break;
        default:
            CORE_ERROR("invalid digital stage", 0);
    };
    int bitA = patternA.is(BIT(bit));
    int bitB = patternB.is(BIT(bit));
    if(bitA &&  bitB)
    {
        return DIGITAL_PATTERN_1;
    }
    if(!bitA && !bitB)
    {
        return DIGITAL_PATTERN_0;
    }
    if(!bitA &&  bitB)
    {
        return DIGITAL_PATTERN_RISING;
    }
    if(bitA && !bitB)
    {
        return DIGITAL_PATTERN_FALLING;
    }
    return DIGITAL_PATTERN_1;
}

///////////////////////////////////////////////////////////////////////
// generator 0
///////////////////////////////////////////////////////////////////////

GeneratorType OsciloscopeControl1::getGeneratorType0()
{
    uint mask = 1 << 8;
    if(generatorType0 & mask)
    {
        return (GeneratorType)(generatorType0 & ~mask);
    }
    else
    {
        return (GeneratorType)(generatorType0);
    }
}

int OsciloscopeControl1::getGeneratorOn0()
{
    uint mask = 1 << 8;
    if(generatorType0 & mask)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int OsciloscopeControl1::getGeneratorSlope0()
{
    return 0;
}

int OsciloscopeControl1::getGeneratorVoltage0()
{
    uint mask = 1 << 12;
    if(generatorVoltage0 & mask)
    {
        return generatorVoltage0 & ~mask;
    }
    else
    {
        return generatorVoltage0;
    }
}

int OsciloscopeControl1::getGeneratorOffset0()
{
    return generatorOffset0;
}

float OsciloscopeControl1::getGeneratorFrequency0(float fs)
{
    uint gDelta = uint(generatorDeltaL0) | uint(generatorDeltaH0 << 16);
    double delta = double(gDelta / 1048575.0);
    return float(delta * double(fs) / 8188.0);
}

float OsciloscopeControl1::getGeneratorSquareDuty0()
{
    return 100.0*float(generatorSqueareDuty0)/2047.0;
}


///////////////////////////////////////////////////////////////////////
// generator 1
///////////////////////////////////////////////////////////////////////

GeneratorType OsciloscopeControl1::getGeneratorType1()
{
    uint mask = 1 << 8;
    if(generatorType1 & mask)
    {
        return (GeneratorType)(generatorType1 & ~mask);
    }
    else
    {
        return (GeneratorType)(generatorType1);
    }
}

int OsciloscopeControl1::getGeneratorOn1()
{
    uint mask = 1 << 8;
    if(generatorType1 & mask)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int OsciloscopeControl1::getGeneratorSlope1()
{
    return 0;
}

int OsciloscopeControl1::getGeneratorVoltage1()
{
    uint mask = 1 << 12;
    if(generatorVoltage1 & mask)
    {
        return generatorVoltage1 & ~mask;
    }
    else
    {
        return generatorVoltage1;
    }
}

int OsciloscopeControl1::getGeneratorOffset1()
{
    return generatorOffset1;
}

float OsciloscopeControl1::getGeneratorFrequency1(float fs)
{
    uint gDelta = uint(generatorDeltaL1) | uint(generatorDeltaH1 << 16);
    double delta = double(gDelta / 1048575.0);
    return float(delta * double(fs) / 8188.0);
}

float OsciloscopeControl1::getGeneratorSquareDuty1()
{
    return 100.0*float(generatorSqueareDuty1)/2047.0;
}

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////

double OsciloscopeControl1::getDigitalVoltage()
{
    double kDigital = pOsciloscope->settings.getHardware()->digitalVoltageCoeficient;
    double Vmin = 1.25 * ((0.0 / kDigital) + 1.0);
    double Vmax = 1.25 * ((255.0 / kDigital) + 1.0);
    double voltage = 1.25 * ((double(digitalVoltage) / kDigital) + 1.0);
    return clamp(voltage, Vmin, Vmax);
}

int OsciloscopeControl1::getDigitalInputOutput15()
{
    return (digitalInputOutput >> 1) & 0x1;
}

int OsciloscopeControl1::getDigitalInputOutput7()
{
    return digitalInputOutput & 0x1;
}

int OsciloscopeControl1::getDigitalOutputBit(int bit)
{
    Flag16 bits;
    bits.set(digitalOutputBit);
    return bits.is(BIT(bit));
}

uint OsciloscopeControl1::getDigitalClockDivide()
{
    return uint(digitalClkDivideL) | uint(digitalClkDivideH << 16);
}


////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeControl1
//
////////////////////////////////////////////////////////////////////////////////
OsciloscopeControl2::OsciloscopeControl2()
{
    controlAddr = 0;
    controlData = 0;
    vgaina = 0;
    vgainb = 0;
    offseta = 0;
    offsetb = 0;
    analogswitch = 0;
    triggerMode = 0;
    triggerSource = 0;
    triggerSlope = 0;
    triggerLevel = 0;
    triggerHis = 0;
    triggerPercent = 0;
    xRange = 0;
    holdoffH = 0;
    holdoffL = 0;
    sampleSizeH = 0;
    sampleSizeL = 0;
    generatorType0 = 0;
    generatorVoltage0 = 0;
    generatorOffset0 = 0;
    generatorDeltaH0 = 0;
    generatorDeltaL0 = 0;
    generatorSqueareDuty0 = 0;
    generatorType1 = 0;
    generatorVoltage1 = 0;
    generatorOffset1 = 0;
    generatorDeltaH1 = 0;
    generatorDeltaL1 = 0;
    generatorSqueareDuty1 = 0;
    digitalPattern1a = 0;
    digitalPattern1b = 0;
    digitalMask1 = 0;
    digitalPattern2a = 0;
    digitalPattern2b = 0;
    digitalMask2 = 0;
    digitalPattern3a = 0;
    digitalPattern3b = 0;
    digitalMask3 = 0;
    digitalPattern4a = 0;
    digitalPattern4b = 0;
    digitalMask4 = 0;
    dt_delayMaxcnt1 = 0;
    dt_delayMaxcnt2 = 0;
    dt_delayMaxcnt3 = 0;
    dt_delayMaxcnt4 = 0;
    dt_control = 0;
    digitalVoltage = 0;
    digitalInputOutput = 0;
    digitalOutputBit = 0;
    digitalOutputMask = 0;
    digitalClkDivideH = 0;
    digitalClkDivideL = 0;
    average   = 0;
    reserved2 = 0;
    reserved3 = 0;
    reserved4 = 0;
    reserved5 = 0;
    reserved6 = 0;
}

void OsciloscopeControl2::Default()
{
    controlAddr = 0x000D;
    controlData = CONTROLL2_NORMAL;
    vgaina = 0xff25;
    vgainb = 0xff25;
    sampleSizeH = 0;
    sampleSizeL = NUM_SAMPLES;
    setGeneratorType0(GENERATOR_SIN);
    setGeneratorFrequency0(50.f);
    setGeneratorType1(GENERATOR_SIN);
    setGeneratorFrequency1(50.f);
}

ushort OsciloscopeControl2::getAttr(uint volt)
{
    return pOsciloscope->settings.getHardware()->referenceGainAttr[volt];
}

ushort OsciloscopeControl2::getGain(int channel, uint volt)
{
    channel = clamp(channel, 0, 1);
    Flag16 flags;
    flags.set(analogswitch);
    if(flags.is(CHANNEL_INTERLEAVE))
    {
        return pOsciloscope->settings.getHardware()->callibratedGainValue[ct500Mhz][channel][volt];
    }
    else
    {
        return pOsciloscope->settings.getHardware()->callibratedGainValue[ctNormal][channel][volt];
    }
}

void OsciloscopeControl2::setAnalogSwitchBit(int bit, int value)
{
    Flag16 flags;
    flags.set(analogswitch);
    flags.bit(bit, value);
    analogswitch = flags.get();
}

void OsciloscopeControl2::setEts(int enable)
{
    if(enable)
    {
        analogswitch = raiseFlag16(analogswitch, CHANNEL_ETS);
    }
    else
    {
        analogswitch = lowerFlag16(analogswitch, CHANNEL_ETS);
    }
}

void OsciloscopeControl2::setYRangeScaleA(uint voltage, float scale)
{
    ushort   attr = getAttr(voltage);
    ushort   gain = getGain(0, voltage);
    if(attr)
    {
        analogswitch = raiseFlag16(analogswitch, CHANNEL_ATTR_A);
    }
    else
    {
        analogswitch = lowerFlag16(analogswitch, CHANNEL_ATTR_A);
    }
    vgaina = gain;
}

void OsciloscopeControl2::setYRangeScaleA(uint gain, uint attr)
{
    if(attr)
    {
        analogswitch = raiseFlag16(analogswitch, CHANNEL_ATTR_A);
    }
    else
    {
        analogswitch = lowerFlag16(analogswitch, CHANNEL_ATTR_A);
    }
    vgaina = gain;
}

void OsciloscopeControl2::setYPositionA(int pos)
{
    pos = min(pos, 1500);
    pos = max(pos, -1500);
    offseta = pos;
}

void OsciloscopeControl2::setYRangeScaleB(uint voltage, float scale)
{
    ushort   attr = getAttr(voltage);
    ushort   gain = getGain(1, voltage);
    if(attr)
    {
        analogswitch = raiseFlag16(analogswitch, CHANNEL_ATTR_B);
    }
    else
    {
        analogswitch = lowerFlag16(analogswitch, CHANNEL_ATTR_B);
    }
    vgainb = gain;
}

void OsciloscopeControl2::setYRangeScaleB(uint gain, uint attr)
{
    if(attr)
    {
        analogswitch = raiseFlag16(analogswitch, CHANNEL_ATTR_B);
    }
    else
    {
        analogswitch = lowerFlag16(analogswitch, CHANNEL_ATTR_B);
    }
    vgainb = gain;
}

void OsciloscopeControl2::setYPositionB(int pos)
{
    pos = min(pos, 1500);
    pos = max(pos, -1500);
    offsetb = pos;
}

void OsciloscopeControl2::setXRange(ishort range)
{
    // 4ns and 2ns is 0
    if(range == 0)
    {
        analogswitch = raiseFlag16(analogswitch, CHANNEL_INTERLEAVE);
        range = 1;
    }
    else
    {
        analogswitch = lowerFlag16(analogswitch, CHANNEL_INTERLEAVE);
    }
    range--;
    xRange = range;
}

void OsciloscopeControl2::setControl(uint selected)
{
    switch(selected)
    {
        case 0:
            controlData = CONTROLL2_NORMAL;
            break;
        case 1:
            controlData = CONTROLL2_TEST;
            break;
        case 3:
            controlData = CONTROLL2_RESET;
            break;
    };
}

void OsciloscopeControl2::setSampleSize(uint value)
{
    value = min<uint>(value, (SCOPEFUN_FRAME_DATA / 4));
    uint size = 4 * (value / 4);
    value = max(256U, size);
    sampleSizeH = (value >> 16) & 0xFFFF;
    sampleSizeL = (value & 0xFFFF);
}


void OsciloscopeControl2::setTriggerSource(int value)
{
    triggerSource = value;
}

void OsciloscopeControl2::setTriggerReArm(bool on)
{
    ushort mask = (1 << 2);
    if(on)
    {
        triggerMode = triggerMode | mask;
    }
    else
    {
        triggerMode = triggerMode & ~mask;
    }
}

void OsciloscopeControl2::setTriggerMode(int value)
{
    triggerMode = value;
}

void OsciloscopeControl2::setTriggerSlope(int value)
{
    triggerSlope = value;
}

void OsciloscopeControl2::setTriggerPre(float perc)
{
    uint sampleSize = getSampleSize();
    triggerPercent  = (perc / 100.f) * float(sampleSize - 1);
    triggerPercent  = ( triggerPercent / 4 );
 }

void OsciloscopeControl2::setTriggerHis(int perc)
{
    triggerHis = perc;
}

void OsciloscopeControl2::setTriggerLevel(int perc)
{
    triggerLevel = perc;
}

void OsciloscopeControl2::setHoldoff(uint holdoff)
{
    holdoffH = (holdoff & 0xFFFF0000) >> 16;
    holdoffL = holdoff & 0x0000FFFF;
}

///////////////////////////////////////////////////////////////////////
// Digital
///////////////////////////////////////////////////////////////////////
void OsciloscopeControl2::setDigitalStart(int start)
{
    Flag16 startBits;
    startBits.set(start);
    Flag16 controlBits;
    controlBits.set(dt_control);
    controlBits.bit(1 << 8, startBits.is(1));
    controlBits.bit(1 << 9, startBits.is(2));
    dt_control = controlBits.get();
}

void OsciloscopeControl2::setDigitalMode(int mode)
{
    Flag16 modeBits;
    modeBits.set(mode);
    Flag16 controlBits;
    controlBits.set(dt_control);
    controlBits.bit(1 << 4, modeBits.is(1));
    dt_control = controlBits.get();
}

void OsciloscopeControl2::setDigitalChannel(int channel)
{
    Flag16 channelBits;
    channelBits.set(channel);
    Flag16 controlBits;
    controlBits.set(dt_control);
    controlBits.bit(1 << 0, channelBits.is(1));
    controlBits.bit(1 << 1, channelBits.is(2));
    controlBits.bit(1 << 2, channelBits.is(4));
    controlBits.bit(1 << 3, channelBits.is(8));
    dt_control = controlBits.get();
}

///////////////////////////////////////////////////////////////////////
// setDigital - stage
///////////////////////////////////////////////////////////////////////
void OsciloscopeControl2::setDigitalDelay(DigitalStage stage, ushort delay)
{
    switch(stage)
    {
        case 0:
            dt_delayMaxcnt1 = delay;
            break;
        case 1:
            dt_delayMaxcnt2 = delay;
            break;
        case 2:
            dt_delayMaxcnt3 = delay;
            break;
        case 3:
            dt_delayMaxcnt4 = delay;
            break;
        default:
            CORE_ERROR("invalid digital stage", 0);
    };
}

void OsciloscopeControl2::setDigitalMask(DigitalStage stage, DigitalBit bit, int value)
{
    Flag16 mask;
    switch(stage)
    {
        case 0:
            mask.set(digitalMask1);
            break;
        case 1:
            mask.set(digitalMask2);
            break;
        case 2:
            mask.set(digitalMask3);
            break;
        case 3:
            mask.set(digitalMask4);
            break;
        default:
            CORE_ERROR("invalid digital stage", 0);
    };
    //
    if(value)
    {
        mask.raise(BIT(bit));
    }
    else
    {
        mask.lower(BIT(bit));
    }
    //
    switch(stage)
    {
        case 0:
            digitalMask1 = mask.get();
            break;
        case 1:
            digitalMask2 = mask.get();
            break;
        case 2:
            digitalMask3 = mask.get();
            break;
        case 3:
            digitalMask4 = mask.get();
            break;
        default:
            CORE_ERROR("invalid digital stage", 0);
    };
}

void OsciloscopeControl2::setDigitalPattern(DigitalStage stage, DigitalBit bit, DigitalPattern pattern)
{
    Flag16 patternA;
    Flag16 patternB;
    switch(stage)
    {
        case 0:
            patternA.set(digitalPattern1a);
            patternB.set(digitalPattern1b);
            break;
        case 1:
            patternA.set(digitalPattern2a);
            patternB.set(digitalPattern2b);
            break;
        case 2:
            patternA.set(digitalPattern3a);
            patternB.set(digitalPattern3b);
            break;
        case 3:
            patternA.set(digitalPattern4a);
            patternB.set(digitalPattern4b);
            break;
        default:
            CORE_ERROR("invalid digital stage", 0);
    };
    //
    switch(pattern)
    {
        case DIGITAL_PATTERN_0: // 00
            patternA.lower(BIT(bit));
            patternB.lower(BIT(bit));
            break;
        case DIGITAL_PATTERN_1: // 11
            patternA.raise(BIT(bit));
            patternB.raise(BIT(bit));
            break;
        case DIGITAL_PATTERN_RISING: // 01
            patternA.lower(BIT(bit));
            patternB.raise(BIT(bit));
            break;
        case DIGITAL_PATTERN_FALLING: // 10
            patternA.raise(BIT(bit));
            patternB.lower(BIT(bit));
            break;
        default:
            CORE_ERROR("invalid digital stage", 0);
    };
    //
    //
    switch(stage)
    {
        case 0:
            digitalPattern1a = patternA.get();
            digitalPattern1b = patternB.get();
            break;
        case 1:
            digitalPattern2a = patternA.get();
            digitalPattern2b = patternB.get();
            break;
        case 2:
            digitalPattern3a = patternA.get();
            digitalPattern3b = patternB.get();
            break;
        case 3:
            digitalPattern4a = patternA.get();
            digitalPattern4b = patternB.get();
            break;
        default:
            CORE_ERROR("invalid digital stage", 0);
    };
}


///////////////////////////////////////////////////////////////////////
// generator 0
///////////////////////////////////////////////////////////////////////
void OsciloscopeControl2::setGeneratorType0(GeneratorType type)
{
    uint mask = 1 << 8;
    if(generatorType0 & mask)
    {
        generatorType0 = type | mask;
    }
    else
    {
        generatorType0 = type;
    }
}

void OsciloscopeControl2::setGeneratorOn0(int onoff)
{
    uint mask = 1 << 8;
    if(onoff)
    {
        generatorType0 = generatorType0 | mask;
    }
    else
    {
        generatorType0 = generatorType0 & ~mask;
    }
}

void OsciloscopeControl2::setGeneratorSlope0(int onoff)
{
}

void OsciloscopeControl2::setGeneratorVoltage0(int volt)
{
    uint voltage = volt;
    uint mask = 1 << 12;
    if(generatorVoltage0 & mask)
    {
        generatorVoltage0 = voltage | mask;
    }
    else
    {
        generatorVoltage0 = voltage;
    }
}

void OsciloscopeControl2::setGeneratorOffset0(int perc)
{
    generatorOffset0 = perc;
}

void OsciloscopeControl2::setGeneratorFrequency0(float freq, float fs)
{
    double delta = 0;
    uint mask = (1 << 8);
    mask = ~mask;
    uint type = generatorType0 & mask;
    switch(type)
    {
        case GENERATOR_CUSTOM:
        case GENERATOR_SIN:
        case GENERATOR_COS:
        case GENERATOR_TRIANGLE:
        case GENERATOR_RAMP_UP:
        case GENERATOR_RAMP_DOWN:
        case GENERATOR_SQUARE:
        case GENERATOR_DELTA:
        case GENERATOR_DC:
        case GENERATOR_NOISE:
            delta = 65536.0 * double(freq) / double(fs);
            break;
    };
    //
    uint gDelta = uint(delta * 131071.0);
    generatorDeltaH0 = (gDelta & 0xFFFF0000) >> 16;
    generatorDeltaL0 = gDelta & 0x0000FFFF;
}

void OsciloscopeControl2::setGeneratorSquareDuty0(float perc)
{
    generatorSqueareDuty0 = perc*2047.0/100.0;
}

///////////////////////////////////////////////////////////////////////
// generator 1
///////////////////////////////////////////////////////////////////////

void OsciloscopeControl2::setGeneratorType1(GeneratorType type)
{
    uint mask = 1 << 8;
    if(generatorType1 & mask)
    {
        generatorType1 = type | mask;
    }
    else
    {
        generatorType1 = type;
    }
}

void OsciloscopeControl2::setGeneratorOn1(int onoff)
{
    uint mask = 1 << 8;
    if(onoff)
    {
        generatorType1 = generatorType1 | mask;
    }
    else
    {
        generatorType1 = generatorType1 & ~mask;
    }
}

void OsciloscopeControl2::setGeneratorSlope1(int onoff)
{
}

void OsciloscopeControl2::setGeneratorVoltage1(int volt)
{
    uint voltage = volt;
    uint mask = 1 << 12;
    if(generatorVoltage1 & mask)
    {
        generatorVoltage1 = voltage | mask;
    }
    else
    {
        generatorVoltage1 = voltage;
    }
}

void OsciloscopeControl2::setGeneratorOffset1(int perc)
{
    generatorOffset1 = perc;
}

void OsciloscopeControl2::setGeneratorFrequency1(float freq, float fs)
{
    double delta = 0;
    uint mask = (1 << 8);
    mask = ~mask;
    uint type = generatorType1 & mask;
    switch(type)
    {
        case GENERATOR_CUSTOM:
        case GENERATOR_SIN:
        case GENERATOR_COS:
        case GENERATOR_TRIANGLE:
        case GENERATOR_RAMP_UP:
        case GENERATOR_RAMP_DOWN:
        case GENERATOR_SQUARE:
        case GENERATOR_DELTA:
        case GENERATOR_DC:
        case GENERATOR_NOISE:
            delta = 65536.0 * double(freq) / double(fs);
            break;
    };
    //
    uint gDelta = uint(delta * 131071.0);
    generatorDeltaH1 = (gDelta & 0xFFFF0000) >> 16;
    generatorDeltaL1 = gDelta & 0x0000FFFF;
}

void OsciloscopeControl2::setGeneratorSquareDuty1(float perc)
{
    generatorSqueareDuty1 = perc*2047.0/100.0;
}

void OsciloscopeControl2::setDigitalVoltage(double volt)
{
    double kDigital = pOsciloscope->settings.getHardware()->digitalVoltageCoeficient;
    double Vmin = 1.25 * ((0.0 / kDigital) + 1.0);
    double Vmax = 1.25 * ((255.0 / kDigital) + 1.0);
    double dVolt = clamp(double(volt), Vmin, Vmax);
    digitalVoltage = ushort(((dVolt / 1.25) - 1.0) * kDigital);
}

void OsciloscopeControl2::setDigitalInputOutput(int inout15, int inout7)
{
    // 00 = OUT, OUT
    // 01 = OUT, IN
    // 10 = IN, OUT
    // 11 = IN, IN
    digitalInputOutput = (inout15 << 1) | inout7;
}

void OsciloscopeControl2::setDigitalOutputBit(int bit, int onoff)
{
    Flag16 output, mask;
    output.set(digitalOutputBit);
    mask.set(digitalOutputMask);
    if(onoff == 0 || onoff == 1)
    {
        output.bit(BIT(bit), onoff);
        mask.bit(BIT(bit), 1);
    }
    else if(onoff == 2)
    {
        output.bit(BIT(bit), onoff);
        mask.bit(BIT(bit), 0);
    }
    digitalOutputBit = output.get();
    digitalOutputMask = mask.get();
}

void OsciloscopeControl2::setDigitalClockDivide(uint divider)
{
    digitalClkDivideH = (divider & 0xFFFF0000) >> 16;
    digitalClkDivideL = divider & 0x0000FFFF;
}

void OsciloscopeControl2::setAverage(int enable)
{
    average = enable;
}

///////////////////////////////////////////////////////////////////////
// get
///////////////////////////////////////////////////////////////////////
ushort OsciloscopeControl2::getAnalogSwitch()
{
    return analogswitch;
}

int OsciloscopeControl2::getEts()
{
    Flag16 tmp;
    tmp.set(analogswitch);
    return tmp.is(CHANNEL_ETS);
}

uint OsciloscopeControl2::getControl()
{
    switch(controlData)
    {
        case CONTROLL2_NORMAL:
            return 0;
        case CONTROLL2_TEST:
            return 1;
        case CONTROLL2_RESET:
            return 3;
    };
    return 0;
}

uint OsciloscopeControl2::getYRangeA()
{
    return getVolt(0, vgaina);
}

float OsciloscopeControl2::getYScaleA()
{
    return 0.f;
}
int OsciloscopeControl2::getYPositionA()
{
    if( offseta > 1500 ) return -int(65536-offseta);
    return (int)offseta;
}

uint OsciloscopeControl2::getYRangeB()
{
    return getVolt(1, vgainb);
}
float OsciloscopeControl2::getYScaleB()
{
    return 0.f;
}
int OsciloscopeControl2::getYPositionB()
{
    if( offsetb > 1500 ) return -int(65536-offsetb);
    return (int)offsetb;
}

ushort OsciloscopeControl2::getTriggerSource()
{
    return triggerSource;
}

ushort OsciloscopeControl2::getTriggerMode()
{
    return triggerMode;
}

ushort OsciloscopeControl2::getTriggerSlope()
{
    return triggerSlope;
}

float OsciloscopeControl2::getTriggerPre()
{
    return 100.f * (float(triggerPercent * 4) / float(getSampleSize() - 1));
}

int OsciloscopeControl2::getTriggerHis()
{
    return triggerHis;
}

int OsciloscopeControl2::getTriggerLevel()
{
    return triggerLevel;
}

ishort OsciloscopeControl2::getXRange()
{
    return xRange;
}

uint OsciloscopeControl2::getSampleSize()
{
    uint lo = sampleSizeL;
    uint hi = sampleSizeH;
    uint sampleSize = lo | (hi << 16);
    return sampleSize;
}

uint OsciloscopeControl2::getHoldoff()
{
    uint holdOffInt = uint(holdoffL) | (uint(holdoffH) << 16);
    return holdOffInt;
}

int OsciloscopeControl2::getDigitalStart()
{
    return (dt_control & 0x300) >> 8;
}

int OsciloscopeControl2::getDigitalMode()
{
    return (dt_control & 0x10) >> 4;
}

int OsciloscopeControl2::getDigitalChannel()
{
    return dt_control & 0xF;
}

ushort OsciloscopeControl2::getDigitalDelay(DigitalStage stage)
{
    switch(stage)
    {
        case 0:
            return dt_delayMaxcnt1;
        case 1:
            return dt_delayMaxcnt2;
        case 2:
            return dt_delayMaxcnt3;
        case 3:
            return dt_delayMaxcnt4;
        default:
            CORE_ERROR("invalid digital stage", 0);
    };
    return 0;
}

int OsciloscopeControl2::getDigitalMask(DigitalStage stage, DigitalBit bit)
{
    Flag16 mask;
    switch(stage)
    {
        case 0:
            mask.set(digitalMask1);
            break;
        case 1:
            mask.set(digitalMask2);
            break;
        case 2:
            mask.set(digitalMask3);
            break;
        case 3:
            mask.set(digitalMask4);
            break;
        default:
            CORE_ERROR("invalid digital stage", 0);
    };
    return mask.is(BIT(bit));
}

DigitalPattern OsciloscopeControl2::getDigitalPattern(DigitalStage stage, DigitalBit bit)
{
    Flag16 patternA;
    Flag16 patternB;
    switch(stage)
    {
        case 0:
            patternA.set(digitalPattern1a);
            patternB.set(digitalPattern1b);
            break;
        case 1:
            patternA.set(digitalPattern2a);
            patternB.set(digitalPattern2b);
            break;
        case 2:
            patternA.set(digitalPattern3a);
            patternB.set(digitalPattern3b);
            break;
        case 3:
            patternA.set(digitalPattern4a);
            patternB.set(digitalPattern4b);
            break;
        default:
            CORE_ERROR("invalid digital stage", 0);
    };
    int bitA = patternA.is(BIT(bit));
    int bitB = patternB.is(BIT(bit));
    if(bitA &&  bitB)
    {
        return DIGITAL_PATTERN_1;
    }
    if(!bitA && !bitB)
    {
        return DIGITAL_PATTERN_0;
    }
    if(!bitA &&  bitB)
    {
        return DIGITAL_PATTERN_RISING;
    }
    if(bitA && !bitB)
    {
        return DIGITAL_PATTERN_FALLING;
    }
    return DIGITAL_PATTERN_1;
}

///////////////////////////////////////////////////////////////////////
// generator 0
///////////////////////////////////////////////////////////////////////

GeneratorType OsciloscopeControl2::getGeneratorType0()
{
    uint mask = 1 << 8;
    if(generatorType0 & mask)
    {
        return (GeneratorType)(generatorType0 & ~mask);
    }
    else
    {
        return (GeneratorType)(generatorType0);
    }
}

int OsciloscopeControl2::getGeneratorOn0()
{
    uint mask = 1 << 8;
    if(generatorType0 & mask)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int OsciloscopeControl2::getGeneratorSlope0()
{
    return 0;
}

int OsciloscopeControl2::getGeneratorVoltage0()
{
    uint mask = 1 << 12;
    if(generatorVoltage0 & mask)
    {
        return generatorVoltage0 & ~mask;
    }
    else
    {
        return generatorVoltage0;
    }
}

int OsciloscopeControl2::getGeneratorOffset0()
{
    return generatorOffset0;
}

float OsciloscopeControl2::getGeneratorFrequency0(float fs)
{
    uint gDelta = uint(generatorDeltaL0) | uint(generatorDeltaH0 << 16);
    double delta = double(gDelta / 1048575.0);
    return float(delta * double(fs) / 8188.0);
}

float OsciloscopeControl2::getGeneratorSquareDuty0()
{
    return 100.0*float(generatorSqueareDuty0)/2047;
}


///////////////////////////////////////////////////////////////////////
// generator 1
///////////////////////////////////////////////////////////////////////

GeneratorType OsciloscopeControl2::getGeneratorType1()
{
    uint mask = 1 << 8;
    if(generatorType1 & mask)
    {
        return (GeneratorType)(generatorType1 & ~mask);
    }
    else
    {
        return (GeneratorType)(generatorType1);
    }
}

int OsciloscopeControl2::getGeneratorOn1()
{
    uint mask = 1 << 8;
    if(generatorType1 & mask)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int OsciloscopeControl2::getGeneratorSlope1()
{
    return 0;
}

int OsciloscopeControl2::getGeneratorVoltage1()
{
    uint mask = 1 << 12;
    if(generatorVoltage1 & mask)
    {
        return generatorVoltage1 & ~mask;
    }
    else
    {
        return generatorVoltage1;
    }
}

int OsciloscopeControl2::getGeneratorOffset1()
{
    return generatorOffset1;
}

float OsciloscopeControl2::getGeneratorFrequency1(float fs)
{
    uint gDelta = uint(generatorDeltaL1) | uint(generatorDeltaH1 << 16);
    double delta = double(gDelta / 1048575.0);
    return float(delta * double(fs) / 8188.0);
}

float OsciloscopeControl2::getGeneratorSquareDuty1()
{
    return 100.0*float(generatorSqueareDuty1)/2047;
}

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////

double OsciloscopeControl2::getDigitalVoltage()
{
    double kDigital = pOsciloscope->settings.getHardware()->digitalVoltageCoeficient;
    double Vmin = 1.25 * ((0.0 / kDigital) + 1.0);
    double Vmax = 1.25 * ((255.0 / kDigital) + 1.0);
    double voltage = 1.25 * ((double(digitalVoltage) / kDigital) + 1.0);
    return clamp(voltage, Vmin, Vmax);
}

int OsciloscopeControl2::getDigitalInputOutput15()
{
    return (digitalInputOutput >> 1) & 0x1;
}

int OsciloscopeControl2::getDigitalInputOutput7()
{
    return digitalInputOutput & 0x1;
}

int OsciloscopeControl2::getDigitalOutputBit(int bit)
{
    Flag16 bits;
    bits.set(digitalOutputBit);
    return bits.is(BIT(bit));
}

uint OsciloscopeControl2::getDigitalClockDivide()
{
    return uint(digitalClkDivideL) | uint(digitalClkDivideH << 16);
}

int OsciloscopeControl2::getAverage()
{
    return average;
}

///////////////////////////////////////////////////////////////////////
//
// OsciloscopeMainControl
//
///////////////////////////////////////////////////////////////////////
OsciloscopeMainControl::OsciloscopeMainControl()
{
    version = 0;
    pControl = 0;
    lock = 0;
    SDL_AtomicUnlock(&lock);
}

void OsciloscopeMainControl::setVersion(int ver)
{
    version = ver;
    if(version == 1)
    {
        pControl = &control1;
    }
    if(version == 2)
    {
        pControl = &control2;
    }
}

void OsciloscopeMainControl::transferData()
{
   SDL_AtomicLock(&lock);
      SHardware1 hw1 = control1.client1Get();
      SHardware2 hw2 = control2.client2Get();
      pOsciloscope->thread.hardwareControlFunction(&hw1, &hw2);
   SDL_AtomicUnlock(&lock);
}

// server
void OsciloscopeControl2::client2Set(SHardware2& in)
{
    controlAddr    = in.controlAddr;
    controlData    = in.controlData;
    vgaina         = in.vgaina;
    vgainb         = in.vgainb;
    offseta        = in.offseta;
    offsetb        = in.offsetb;
    analogswitch   = in.analogswitch;
    triggerMode    = in.triggerMode;
    triggerSource  = in.triggerSource;
    triggerSlope   = in.triggerSlope;
    triggerLevel   = in.triggerLevel;
    triggerHis     = in.triggerHis;
    triggerPercent = in.triggerPercent;
    xRange         = in.xRange;
    sampleSizeL    = in.sampleSizeL;
    sampleSizeH    = in.sampleSizeH;
    holdoffH = in.holdoffH;
    holdoffL = in.holdoffL;
    generatorType0 = in.generatorType0;
    generatorVoltage0 = in.generatorVoltage0;
    generatorOffset0 = in.generatorOffset0;
    generatorDeltaH0 = in.generatorDeltaH0;
    generatorDeltaL0 = in.generatorDeltaL0;
    generatorSqueareDuty0 = in.generatorSqueareDuty0;
    generatorType1 = in.generatorType1;
    generatorVoltage1 = in.generatorVoltage1;
    generatorOffset1 = in.generatorOffset1;
    generatorDeltaH1 = in.generatorDeltaH1;
    generatorDeltaL1 = in.generatorDeltaL1;
    generatorSqueareDuty1 = in.generatorSqueareDuty1;
    digitalPattern1a = in.digitalPattern1a;
    digitalPattern1b = in.digitalPattern1b;
    digitalMask1 = in.digitalMask1;
    digitalPattern2a = in.digitalPattern2a;
    digitalPattern2b = in.digitalPattern2b;
    digitalMask2 = in.digitalMask2;
    digitalPattern3a = in.digitalPattern3a;
    digitalPattern3b = in.digitalPattern3b;
    digitalMask3 = in.digitalMask3;
    digitalPattern4a = in.digitalPattern4a;
    digitalPattern4b = in.digitalPattern4b;
    digitalMask4 = in.digitalMask4;
    dt_delayMaxcnt1 = in.dt_delayMaxcnt1;
    dt_delayMaxcnt2 = in.dt_delayMaxcnt2;
    dt_delayMaxcnt3 = in.dt_delayMaxcnt3;
    dt_delayMaxcnt4 = in.dt_delayMaxcnt4;
    dt_control = in.dt_control;
    digitalVoltage = in.digitalVoltage;
    digitalInputOutput = in.digitalInputOutput;
    digitalOutputBit = in.digitalOutputBit;
    digitalOutputMask = in.digitalOutputMask;
    digitalClkDivideH = in.digitalClkDivideH;
    digitalClkDivideL = in.digitalClkDivideL;
    ushort* pShort = &controlAddr;
    for(int i = 0; i < sizeof(SHardware2) / 2; i++)
    {
        *pShort = endianSwap16(*pShort);
        pShort++;
    }
}

SHardware2 OsciloscopeControl2::client2Get()
{
    SHardware2 hw = { 0 };
    hw.controlAddr = controlAddr;
    hw.controlData = controlData;
    hw.vgaina = vgaina;
    hw.vgainb = vgainb;
    hw.offseta = offseta;
    hw.offsetb = offsetb;
    hw.analogswitch = analogswitch;
    hw.triggerMode = triggerMode;
    hw.triggerSource = triggerSource;
    hw.triggerSlope = triggerSlope;
    hw.triggerLevel = triggerLevel;
    hw.triggerHis = triggerHis;
    hw.triggerPercent = triggerPercent;
    hw.xRange = xRange;
    hw.sampleSizeL = sampleSizeL;
    hw.sampleSizeH = sampleSizeH;
    hw.holdoffH = holdoffH;
    hw.holdoffL = holdoffL;
    hw.generatorType0 = generatorType0;
    hw.generatorVoltage0 = generatorVoltage0;
    hw.generatorOffset0 = generatorOffset0;
    hw.generatorDeltaH0 = generatorDeltaH0;
    hw.generatorDeltaL0 = generatorDeltaL0;
    hw.generatorSqueareDuty0 = generatorSqueareDuty0;
    hw.generatorType1 = generatorType1;
    hw.generatorVoltage1 = generatorVoltage1;
    hw.generatorOffset1 = generatorOffset1;
    hw.generatorDeltaH1 = generatorDeltaH1;
    hw.generatorDeltaL1 = generatorDeltaL1;
    hw.generatorSqueareDuty1 = generatorSqueareDuty1;
    hw.digitalPattern1a = digitalPattern1a;
    hw.digitalPattern1b = digitalPattern1b;
    hw.digitalMask1 = digitalMask1;
    hw.digitalPattern2a = digitalPattern2a;
    hw.digitalPattern2b = digitalPattern2b;
    hw.digitalMask2 = digitalMask2;
    hw.digitalPattern3a = digitalPattern3a;
    hw.digitalPattern3b = digitalPattern3b;
    hw.digitalMask3 = digitalMask3;
    hw.digitalPattern4a = digitalPattern4a;
    hw.digitalPattern4b = digitalPattern4b;
    hw.digitalMask4 = digitalMask4;
    hw.dt_delayMaxcnt1 = dt_delayMaxcnt1;
    hw.dt_delayMaxcnt2 = dt_delayMaxcnt2;
    hw.dt_delayMaxcnt3 = dt_delayMaxcnt3;
    hw.dt_delayMaxcnt4 = dt_delayMaxcnt4;
    hw.dt_control = dt_control;
    hw.digitalVoltage = digitalVoltage;
    hw.digitalInputOutput = digitalInputOutput;
    hw.digitalOutputBit = digitalOutputBit;
    hw.digitalOutputMask = digitalOutputMask;
    hw.digitalClkDivideH = digitalClkDivideH;
    hw.digitalClkDivideL = digitalClkDivideL;
    hw.average = average;
    return hw;
}

void OsciloscopeControl1::client1Set(SHardware1& in)
{
    control = in.control;
    vgaina = in.vgaina;
    vgainb = in.vgainb;
    offseta = in.offseta;
    offsetb = in.offsetb;
    siggen = in.siggen;
    trigger = in.trigger;
    analogswitch = in.analogswitch;
    triggerMode = in.triggerMode;
    triggerSource = in.triggerSource;
    triggerSlope = in.triggerSlope;
    triggerLevel = in.triggerLevel;
    triggerHis = in.triggerHis;
    triggerPercent = in.triggerPercent;
    xRange = in.xRange;
    sampleSize = in.sampleSize;
    holdoffH = in.holdoffH;
    holdoffL = in.holdoffL;
    generatorType0 = in.generatorType0;
    generatorVoltage0 = in.generatorVoltage0;
    generatorOffset0 = in.generatorOffset0;
    generatorDeltaH0 = in.generatorDeltaH0;
    generatorDeltaL0 = in.generatorDeltaL0;
    generatorSqueareDuty0 = in.generatorSqueareDuty0;
    generatorType1 = in.generatorType1;
    generatorVoltage1 = in.generatorVoltage1;
    generatorOffset1 = in.generatorOffset1;
    generatorDeltaH1 = in.generatorDeltaH1;
    generatorDeltaL1 = in.generatorDeltaL1;
    generatorSqueareDuty1 = in.generatorSqueareDuty1;
    digitalPattern1a = in.digitalPattern1a;
    digitalPattern1b = in.digitalPattern1b;
    digitalMask1 = in.digitalMask1;
    digitalPattern2a = in.digitalPattern2a;
    digitalPattern2b = in.digitalPattern2b;
    digitalMask2 = in.digitalMask2;
    digitalPattern3a = in.digitalPattern3a;
    digitalPattern3b = in.digitalPattern3b;
    digitalMask3 = in.digitalMask3;
    digitalPattern4a = in.digitalPattern4a;
    digitalPattern4b = in.digitalPattern4b;
    digitalMask4 = in.digitalMask4;
    dt_delayMaxcnt1 = in.dt_delayMaxcnt1;
    dt_delayMaxcnt2 = in.dt_delayMaxcnt2;
    dt_delayMaxcnt3 = in.dt_delayMaxcnt3;
    dt_delayMaxcnt4 = in.dt_delayMaxcnt4;
    dt_control = in.dt_control;
    digitalVoltage = in.digitalVoltage;
    digitalInputOutput = in.digitalInputOutput;
    digitalOutputBit = in.digitalOutputBit;
    digitalOutputMask = in.digitalOutputMask;
    digitalClkDivideH = in.digitalClkDivideH;
    digitalClkDivideL = in.digitalClkDivideL;
}

SHardware1 OsciloscopeControl1::client1Get()
{
    SHardware1 hw = { 0 };
    hw.control = control;
    hw.vgaina = vgaina;
    hw.vgainb = vgainb;
    hw.offseta = offseta;
    hw.offsetb = offsetb;
    hw.siggen = siggen;
    hw.trigger = trigger;
    hw.analogswitch = analogswitch;
    hw.triggerMode = triggerMode;
    hw.triggerSource = triggerSource;
    hw.triggerSlope = triggerSlope;
    hw.triggerLevel = triggerLevel;
    hw.triggerHis = triggerHis;
    hw.triggerPercent = triggerPercent;
    hw.xRange = xRange;
    hw.sampleSize = sampleSize;
    hw.holdoffH = holdoffH;
    hw.holdoffL = holdoffL;
    hw.generatorType0 = generatorType0;
    hw.generatorVoltage0 = generatorVoltage0;
    hw.generatorOffset0 = generatorOffset0;
    hw.generatorDeltaH0 = generatorDeltaH0;
    hw.generatorDeltaL0 = generatorDeltaL0;
    hw.generatorSqueareDuty0 = generatorSqueareDuty0;
    hw.generatorType1 = generatorType1;
    hw.generatorVoltage1 = generatorVoltage1;
    hw.generatorOffset1 = generatorOffset1;
    hw.generatorDeltaH1 = generatorDeltaH1;
    hw.generatorDeltaL1 = generatorDeltaL1;
    hw.generatorSqueareDuty1 = generatorSqueareDuty1;
    hw.digitalPattern1a = digitalPattern1a;
    hw.digitalPattern1b = digitalPattern1b;
    hw.digitalMask1 = digitalMask1;
    hw.digitalPattern2a = digitalPattern2a;
    hw.digitalPattern2b = digitalPattern2b;
    hw.digitalMask2 = digitalMask2;
    hw.digitalPattern3a = digitalPattern3a;
    hw.digitalPattern3b = digitalPattern3b;
    hw.digitalMask3 = digitalMask3;
    hw.digitalPattern4a = digitalPattern4a;
    hw.digitalPattern4b = digitalPattern4b;
    hw.digitalMask4 = digitalMask4;
    hw.dt_delayMaxcnt1 = dt_delayMaxcnt1;
    hw.dt_delayMaxcnt2 = dt_delayMaxcnt2;
    hw.dt_delayMaxcnt3 = dt_delayMaxcnt3;
    hw.dt_delayMaxcnt4 = dt_delayMaxcnt4;
    hw.dt_control = dt_control;
    hw.digitalVoltage = digitalVoltage;
    hw.digitalInputOutput = digitalInputOutput;
    hw.digitalOutputBit = digitalOutputBit;
    hw.digitalOutputMask = digitalOutputMask;
    hw.digitalClkDivideH = digitalClkDivideH;
    hw.digitalClkDivideL = digitalClkDivideL;
    return hw;
}

void OsciloscopeMainControl::setWindow(WndMain& window)
{
    Flag16 flags;
    flags.set(getAnalogSwitch());
    window.channel01.Ground = flags.is(CHANNEL_A_GROUND);
    window.channel02.Ground = flags.is(CHANNEL_B_GROUND);
    window.channel01.AcDc = flags.is(CHANNEL_A_ACDC);
    window.channel02.AcDc = flags.is(CHANNEL_B_ACDC);
    window.horizontal.ETS = flags.is(CHANNEL_ETS);
    // channel A, B
    window.horizontal.Control = getControl();
    window.channel01.Capture = captureVoltFromEnum(getYRangeA());
    window.channel01.Scale = getYScaleA();
    window.channel01.YPosition = getYPositionA() - pOsciloscope->settings.getHardware()->getAnalogOffset(window.horizontal.Capture, 0, window.channel01.Capture);
    window.channel02.Capture = captureVoltFromEnum(getYRangeB());
    window.channel02.Scale = getYScaleB();
    window.channel02.YPosition = getYPositionB() - pOsciloscope->settings.getHardware()->getAnalogOffset(window.horizontal.Capture, 1, window.channel02.Capture);
    // trigger
    window.trigger.Source = getTriggerSource();
    window.trigger.Mode = getTriggerMode();
    window.trigger.Slope = getTriggerSlope();
    window.trigger.Percent = getTriggerPre();
    window.trigger.His = getTriggerHis();
    window.trigger.Level = getTriggerLevel();
    window.trigger.Holdoff = getHoldoff();
    // time
    window.horizontal.Capture = captureTimeFromEnum(getXRange());
    window.horizontal.FrameSize = getSampleSize();
    // digital
    window.trigger.stageStart = getDigitalStart();
    window.trigger.stageMode = getDigitalMode();
    window.trigger.stageChannel = getDigitalChannel();
    // digital: delay, mask, pattern
    for(int stage = 0; stage < 4; stage++)
    {
        window.trigger.delay[stage] = getDigitalDelay((DigitalStage)stage);
    }
    for(int stage = 0; stage < 4; stage++)
    {
        for(int bit = 0; bit < 16; bit++)
        {
            window.trigger.mask[stage][bit] = getDigitalMask((DigitalStage)stage, (DigitalBit)bit);
        }
    }
    for(int stage = 0; stage < 4; stage++)
    {
        for(int bit = 0; bit < 16; bit++)
        {
            window.trigger.pattern[stage][bit] = getDigitalPattern((DigitalStage)stage, (DigitalBit)bit);
        }
    }
    // digital: voltage, inputoutput, clock
    window.digitalSetup.voltage = getDigitalVoltage();
    window.digitalSetup.inputOutput15 = getDigitalInputOutput15();
    window.digitalSetup.inputOutput7 = getDigitalInputOutput7();
    window.digitalSetup.divider = getDigitalClockDivide();
    // digital: output
    for(int i = 0; i < 16; i++)
    {
        window.digital.output[i] = getDigitalOutputBit(i);
    }
    // generator A
    window.hardwareGenerator.type0 = getGeneratorType0();
    window.hardwareGenerator.onOff0 = getGeneratorOn0();
    window.hardwareGenerator.sawSlopePositive0 = getGeneratorSlope0();
    window.hardwareGenerator.voltage0 = getGeneratorVoltage0();
    window.hardwareGenerator.offset0 = getGeneratorOffset0() - pOsciloscope->settings.getHardware()->getGeneratorOffset(window.horizontal.Capture,0);
    window.hardwareGenerator.frequency0 = getGeneratorFrequency0(pOsciloscope->settings.getHardware()->generatorFs);
    window.hardwareGenerator.squareDuty0 = getGeneratorSquareDuty0();
    // generator B
    window.hardwareGenerator.type1 = getGeneratorType1();
    window.hardwareGenerator.onOff1 = getGeneratorOn1();
    window.hardwareGenerator.sawSlopePositive1 = getGeneratorSlope1();
    window.hardwareGenerator.voltage1 = getGeneratorVoltage1();
    window.hardwareGenerator.offset1 = getGeneratorOffset1() - pOsciloscope->settings.getHardware()->getGeneratorOffset(window.horizontal.Capture,1);
    window.hardwareGenerator.frequency1 = getGeneratorFrequency1(pOsciloscope->settings.getHardware()->generatorFs);
    window.hardwareGenerator.squareDuty1 = getGeneratorSquareDuty1();
}


///////////////////////////////////////////////////////////////////////
//
// interface
//
///////////////////////////////////////////////////////////////////////

void OsciloscopeMainControl::Default()
{
    pControl->Default();
}

///////////////////////////////////////////////////////////////////////
//
// set
//
///////////////////////////////////////////////////////////////////////

void OsciloscopeMainControl::setAnalogSwitchBit(int bit, int value)
{
    pControl->setAnalogSwitchBit(bit, value);
}

void OsciloscopeMainControl::setEts(int enable)
{
    pControl->setEts(enable);
}

void OsciloscopeMainControl::setControl(uint control)
{
    pControl->setControl(control);
}

void OsciloscopeMainControl::setYRangeScaleA(uint voltage, float scale)
{
    pControl->setYRangeScaleA(voltage, scale);
}
void OsciloscopeMainControl::setYRangeScaleA(uint gain, uint attr)
{
    pControl->setYRangeScaleA(gain, attr);
}

void OsciloscopeMainControl::setYPositionA(int pos)
{
    pControl->setYPositionA(pos);
}

void OsciloscopeMainControl::setYRangeScaleB(uint voltage, float scale)
{
    pControl->setYRangeScaleB(voltage, scale);
}

void OsciloscopeMainControl::setYRangeScaleB(uint gain, uint attr)
{
    pControl->setYRangeScaleB(gain, attr);
}

void OsciloscopeMainControl::setYPositionB(int pos)
{
    pControl->setYPositionB(pos);
}

void OsciloscopeMainControl::setTriggerSource(int source)
{
    pControl->setTriggerSource(source);
}
void OsciloscopeMainControl::setTriggerMode(int mode)
{
    pControl->setTriggerMode(mode);
}
void OsciloscopeMainControl::setTriggerSlope(int mode)
{
    pControl->setTriggerSlope(mode);
}
void OsciloscopeMainControl::setTriggerReArm(bool on)
{
    pControl->setTriggerReArm(on);
}
void OsciloscopeMainControl::setTriggerPre(float perc)
{
    pControl->setTriggerPre(perc);
}
void OsciloscopeMainControl::setTriggerHis(int perc)
{
    pControl->setTriggerHis(perc);
}
void OsciloscopeMainControl::setTriggerLevel(int perc)
{
    pControl->setTriggerLevel(perc);
}

void OsciloscopeMainControl::setXRange(ishort range)
{
    pControl->setXRange(range);
}

void OsciloscopeMainControl::setSampleSize(uint frameSize)
{
    pControl->setSampleSize(frameSize);
}

void OsciloscopeMainControl::setHoldoff(uint holdoff)
{
    pControl->setHoldoff(holdoff);
}

void OsciloscopeMainControl::setDigitalStart(int start)
{
    pControl->setDigitalStart(start);
}

void OsciloscopeMainControl::setDigitalMode(int mode)
{
    pControl->setDigitalMode(mode);
}
void OsciloscopeMainControl::setDigitalChannel(int channel)
{
    pControl->setDigitalChannel(channel);
}
void OsciloscopeMainControl::setDigitalDelay(DigitalStage stage, ushort delay)
{
    pControl->setDigitalDelay(stage, delay);
}
void OsciloscopeMainControl::setDigitalMask(DigitalStage stage, DigitalBit bit, int value)
{
    pControl->setDigitalMask(stage, bit, value);
}
void OsciloscopeMainControl::setDigitalPattern(DigitalStage stage, DigitalBit bit, DigitalPattern pattern)
{
    pControl->setDigitalPattern(stage, bit, pattern);
}

void OsciloscopeMainControl::setGeneratorType0(GeneratorType type)
{
    pControl->setGeneratorType0(type);
}
void OsciloscopeMainControl::setGeneratorOn0(int onoff)
{
    pControl->setGeneratorOn0(onoff);
}
void OsciloscopeMainControl::setGeneratorSlope0(int onoff)
{
    pControl->setGeneratorSlope0(onoff);
}
void OsciloscopeMainControl::setGeneratorVoltage0(int volt)
{
    pControl->setGeneratorVoltage0(volt);
}
void OsciloscopeMainControl::setGeneratorOffset0(int perc)
{
    pControl->setGeneratorOffset0(perc);
}
void OsciloscopeMainControl::setGeneratorFrequency0(float freq, float fs)
{
    pControl->setGeneratorFrequency0(freq, fs);
}
void OsciloscopeMainControl::setGeneratorSquareDuty0(float perc)
{
    pControl->setGeneratorSquareDuty0(perc);
}

void OsciloscopeMainControl::setGeneratorType1(GeneratorType type)
{
    pControl->setGeneratorType1(type);
}
void OsciloscopeMainControl::setGeneratorOn1(int onoff)
{
    pControl->setGeneratorOn1(onoff);
}
void OsciloscopeMainControl::setGeneratorSlope1(int onoff)
{
    pControl->setGeneratorSlope1(onoff);
}
void OsciloscopeMainControl::setGeneratorVoltage1(int volt)
{
    pControl->setGeneratorVoltage1(volt);
}
void OsciloscopeMainControl::setGeneratorOffset1(int perc)
{
    pControl->setGeneratorOffset1(perc);
}
void OsciloscopeMainControl::setGeneratorFrequency1(float freq, float fs)
{
    pControl->setGeneratorFrequency1(freq, fs);
}
void OsciloscopeMainControl::setGeneratorSquareDuty1(float perc)
{
    pControl->setGeneratorSquareDuty1(perc);
}
void OsciloscopeMainControl::setDigitalVoltage(double volt)
{
    pControl->setDigitalVoltage(volt);
}
void OsciloscopeMainControl::setDigitalInputOutput(int inout15, int inout7)
{
    pControl->setDigitalInputOutput(inout15, inout7);
}
void OsciloscopeMainControl::setDigitalOutputBit(int bit, int onoff)
{
    pControl->setDigitalOutputBit(bit, onoff);
}
void OsciloscopeMainControl::setDigitalClockDivide(uint divider)
{
    pControl->setDigitalClockDivide(divider);
}
void OsciloscopeMainControl::setAverage(int enable)
{
    pControl->setAverage(enable);
}

///////////////////////////////////////////////////////////////////////
//
// get
//
///////////////////////////////////////////////////////////////////////
ushort OsciloscopeMainControl::getAnalogSwitch()
{
    return pControl->getAnalogSwitch();
}
int   OsciloscopeMainControl::getEts()
{
    return pControl->getEts();
}
uint  OsciloscopeMainControl::getControl()
{
    return pControl->getControl();
}
uint  OsciloscopeMainControl::getYRangeA()
{
    return pControl->getYRangeA();
}
float OsciloscopeMainControl::getYScaleA()
{
    return pControl->getYScaleA();
}
int   OsciloscopeMainControl::getYPositionA()
{
    return pControl->getYPositionA();
}
uint  OsciloscopeMainControl::getYRangeB()
{
    return pControl->getYRangeB();
}
float OsciloscopeMainControl::getYScaleB()
{
    return pControl->getYScaleB();
}
int   OsciloscopeMainControl::getYPositionB()
{
    return pControl->getYPositionB();
}
ushort OsciloscopeMainControl::getTriggerSource()
{
    return pControl->getTriggerSource();
}
ushort OsciloscopeMainControl::getTriggerMode()
{
    return pControl->getTriggerMode();
}
ushort OsciloscopeMainControl::getTriggerSlope()
{
    return pControl->getTriggerSlope();
}
float  OsciloscopeMainControl::getTriggerPre()
{
    return pControl->getTriggerPre();
}
int    OsciloscopeMainControl::getTriggerHis()
{
    return pControl->getTriggerHis();
}
int    OsciloscopeMainControl::getTriggerLevel()
{
    return pControl->getTriggerLevel();
}

uint   OsciloscopeMainControl::getSampleSize()
{
    return pControl->getSampleSize();
}
ishort OsciloscopeMainControl::getXRange()
{
    return pControl->getXRange();
}
uint OsciloscopeMainControl::getHoldoff()
{
    return pControl->getHoldoff();
}

int OsciloscopeMainControl::getDigitalStart()
{
    return pControl->getDigitalStart();
}
int OsciloscopeMainControl::getDigitalMode()
{
    return pControl->getDigitalMode();
}
int OsciloscopeMainControl::getDigitalChannel()
{
    return pControl->getDigitalChannel();
}

ushort OsciloscopeMainControl::getDigitalDelay(DigitalStage stage)
{
    return pControl->getDigitalDelay(stage);
}
int    OsciloscopeMainControl::getDigitalMask(DigitalStage stage, DigitalBit bit)
{
    return pControl->getDigitalMask(stage, bit);
}
DigitalPattern OsciloscopeMainControl::getDigitalPattern(DigitalStage stage, DigitalBit bit)
{
    return pControl->getDigitalPattern(stage, bit);
}

GeneratorType OsciloscopeMainControl::getGeneratorType0()
{
    return pControl->getGeneratorType0();
}
int OsciloscopeMainControl::getGeneratorOn0()
{
    return pControl->getGeneratorOn0();
}
int OsciloscopeMainControl::getGeneratorSlope0()
{
    return pControl->getGeneratorSlope0();
}
int OsciloscopeMainControl::getGeneratorVoltage0()
{
    return pControl->getGeneratorVoltage0();
}
int   OsciloscopeMainControl::getGeneratorOffset0()
{
    return pControl->getGeneratorOffset0();
}
float OsciloscopeMainControl::getGeneratorFrequency0(float fs)
{
    return pControl->getGeneratorFrequency0(fs);
}
float OsciloscopeMainControl::getGeneratorSquareDuty0()
{
    return pControl->getGeneratorSquareDuty0();
}

GeneratorType OsciloscopeMainControl::getGeneratorType1()
{
    return pControl->getGeneratorType1();
}
int OsciloscopeMainControl::getGeneratorOn1()
{
    return pControl->getGeneratorOn1();
}
int OsciloscopeMainControl::getGeneratorSlope1()
{
    return pControl->getGeneratorSlope1();
}
int OsciloscopeMainControl::getGeneratorVoltage1()
{
    return pControl->getGeneratorVoltage1();
}
int OsciloscopeMainControl::getGeneratorOffset1()
{
    return pControl->getGeneratorOffset1();
}
float OsciloscopeMainControl::getGeneratorFrequency1(float fs)
{
    return pControl->getGeneratorFrequency1(fs);
}
float OsciloscopeMainControl::getGeneratorSquareDuty1()
{
    return pControl->getGeneratorSquareDuty1();
}
double OsciloscopeMainControl::getDigitalVoltage()
{
    return pControl->getDigitalVoltage();
}
int    OsciloscopeMainControl::getDigitalInputOutput15()
{
    return pControl->getDigitalInputOutput15();
}
int    OsciloscopeMainControl::getDigitalInputOutput7()
{
    return pControl->getDigitalInputOutput7();
}
int    OsciloscopeMainControl::getDigitalOutputBit(int bit)
{
    return pControl->getDigitalOutputBit(bit);
}
uint   OsciloscopeMainControl::getDigitalClockDivide()
{
    return pControl->getDigitalClockDivide();
}
ushort OsciloscopeMainControl::getAttr(uint volt)
{
    return pControl->getAttr(volt);
}
ushort OsciloscopeMainControl::getGain(int channel, uint volt)
{
    return pControl->getGain(channel, volt);
}
int OsciloscopeMainControl::getAverage()
{
    return pControl->getAverage();
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
