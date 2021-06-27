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
#ifndef __OSC_CONTROL__
#define __OSC_CONTROL__

////////////////////////////////////////////////////////////////////////////////
//
// AnalogFunction
//
////////////////////////////////////////////////////////////////////////////////
enum AnalogFunction
{
    ANALOG_FUNCTION_MEDIUM = 0,
    ANALOG_FUNCTION_SUB_CH0_CH1,
    ANALOG_FUNCTION_SUB_CH1_CH0,
    ANALOG_FUNCTION_ADD,
    ANALOG_FUNCTION_MIN,
    ANALOG_FUNCTION_MAX,
    ANALOG_FUNCTION_CUSTOM,
    ANALOG_FUNCTION_UPLOADED,
};

////////////////////////////////////////////////////////////////////////////////
//
// AnalogFlag
//
////////////////////////////////////////////////////////////////////////////////
enum AnalogFlag
{
    CHANNEL_ATTR_B = BIT(0),
    CHANNEL_ATTR_A = BIT(1),
    CHANNEL_B_GROUND = BIT(2),
    CHANNEL_A_GROUND = BIT(3),
    CHANNEL_B_ACDC = BIT(4),
    CHANNEL_A_ACDC = BIT(5),
    CHANNEL_INTERLEAVE = BIT(6),
    CHANNEL_ETS = BIT(7),
};

////////////////////////////////////////////////////////////////////////////////
//
// GeneratorType
//
////////////////////////////////////////////////////////////////////////////////
enum GeneratorType
{
    GENERATOR_CUSTOM,
    GENERATOR_SIN,
    GENERATOR_COS,
    GENERATOR_TRIANGLE,
    GENERATOR_RAMP_UP,
    GENERATOR_RAMP_DOWN,
    GENERATOR_SQUARE,
    GENERATOR_DELTA,
    GENERATOR_DC,
    GENERATOR_NOISE,
};

enum ControllType1
{
    CONTROLL1_SHUTDOWN = 0x0000,
    CONTROLL1_RESET = 0x0A5A,
    CONTROLL1_TEST = 0x0640,
    CONTROLL1_WAKEUP = 0x0003,
    CONTROLL1_NORMAL = 0x0600,
};

enum ControllType2
{
    CONTROLL2_NORMAL = 0x0000,
    CONTROLL2_TEST   = 0x0002,
    CONTROLL2_RESET  = 0x0004,
};

enum CallibrateFreq
{
    CALLIBRATE_1K,
    CALLIBRATE_5K,
    CALLIBRATE_10K,
    CALLIBRATE_100K,
    CALLIBRATE_200K,
    CALLIBRATE_500K,
    CALLIBRATE_1M,
    CALLIBRATE_2M,
};

enum DigitalPattern
{
    DIGITAL_PATTERN_0,
    DIGITAL_PATTERN_1,
    DIGITAL_PATTERN_RISING,
    DIGITAL_PATTERN_FALLING,
};

enum DigitalStage
{
    DIGITAL_STAGE_0,
    DIGITAL_STAGE_1,
    DIGITAL_STAGE_2,
    DIGITAL_STAGE_3,
};

enum DigitalBit
{
    DIGITAL_BIT_0,
    DIGITAL_BIT_1,
    DIGITAL_BIT_2,
    DIGITAL_BIT_3,
    DIGITAL_BIT_4,
    DIGITAL_BIT_5,
    DIGITAL_BIT_6,
    DIGITAL_BIT_7,
    DIGITAL_BIT_8,
    DIGITAL_BIT_9,
    DIGITAL_BIT_10,
    DIGITAL_BIT_11,
    DIGITAL_BIT_12,
    DIGITAL_BIT_13,
    DIGITAL_BIT_14,
    DIGITAL_BIT_15,
};

class OsciloscopeControlInterface
{
public:
    // default
    virtual void Default() = 0;
public:
    // set
    virtual void setAnalogSwitchBit(int bit, int value) = 0;
    virtual void setEts(int enable) = 0;
    virtual void setControl(uint control) = 0;
    virtual void setYRangeScaleA(uint voltage, float scale) = 0;
    virtual void setYRangeScaleA(uint gain, uint attr) = 0;
    virtual void setYPositionA(int pos) = 0;
    virtual void setYRangeScaleB(uint voltage, float scale) = 0;
    virtual void setYRangeScaleB(uint gain, uint attr) = 0;
    virtual void setYPositionB(int pos) = 0;
public:
    virtual void setTriggerSource(int source) = 0;
    virtual void setTriggerMode(int mode) = 0;
    virtual void setTriggerSlope(int mode) = 0;
    virtual void setTriggerReArm(bool on) = 0;
    virtual void setTriggerPre(float perc) = 0;
    virtual void setTriggerHis(int perc) = 0;
    virtual void setTriggerLevel(int perc) = 0;
public:
    virtual void setXRange(ishort range) = 0;
    virtual void setSampleSize(uint frameSize) = 0;
    virtual void setHoldoff(uint holdoff) = 0;
public:
    virtual void setDigitalStart(int start) = 0;
    virtual void setDigitalMode(int mode) = 0;
    virtual void setDigitalChannel(int channel) = 0;
    virtual void setDigitalDelay(DigitalStage stage, ushort delay) = 0;
    virtual void setDigitalMask(DigitalStage stage, DigitalBit bit, int value) = 0;
    virtual void setDigitalPattern(DigitalStage stage, DigitalBit bit, DigitalPattern pattern) = 0;
public:
    virtual void setGeneratorType0(GeneratorType type) = 0;
    virtual void setGeneratorOn0(int onoff) = 0;
    virtual void setGeneratorSlope0(int onoff) = 0;
    virtual void setGeneratorVoltage0(int volt) = 0;
    virtual void setGeneratorOffset0(int perc) = 0;
    virtual void setGeneratorFrequency0(float freq, float fs = 457142.81f) = 0;
    virtual void setGeneratorSquareDuty0(float perc) = 0;
public:
    virtual void setGeneratorType1(GeneratorType type) = 0;
    virtual void setGeneratorOn1(int onoff) = 0;
    virtual void setGeneratorSlope1(int onoff) = 0;
    virtual void setGeneratorVoltage1(int volt) = 0;
    virtual void setGeneratorOffset1(int perc) = 0;
    virtual void setGeneratorFrequency1(float freq, float fs = 457142.81f) = 0;
    virtual void setGeneratorSquareDuty1(float perc) = 0;
public:
    virtual void setDigitalVoltage(double volt) = 0;
    virtual void setDigitalInputOutput(int inout15, int inout7) = 0;
    virtual void setDigitalOutputBit(int bit, int onoff) = 0;
    virtual void setDigitalClockDivide(uint divider) = 0;
public:
    virtual void setAverage(int enable) = 0;
public:
    // get
    virtual ushort getAnalogSwitch() = 0;
    virtual int    getEts() = 0;
    virtual uint   getControl() = 0;
    virtual uint   getYRangeA() = 0;
    virtual float  getYScaleA() = 0;
    virtual int    getYPositionA() = 0;
    virtual uint   getYRangeB() = 0;
    virtual float  getYScaleB() = 0;
    virtual int    getYPositionB() = 0;
public:
    virtual ushort getTriggerSource() = 0;
    virtual ushort getTriggerMode() = 0;
    virtual ushort getTriggerSlope() = 0;
    virtual float  getTriggerPre() = 0;
    virtual int    getTriggerHis() = 0;
    virtual int    getTriggerLevel() = 0;
public:
    virtual uint   getSampleSize() = 0;
    virtual ishort getXRange() = 0;
    virtual uint   getHoldoff() = 0;
public:
    virtual int   getDigitalStart() = 0;
    virtual int   getDigitalMode() = 0;
    virtual int   getDigitalChannel() = 0;
public:
    virtual ushort         getDigitalDelay(DigitalStage stage) = 0;
    virtual int            getDigitalMask(DigitalStage stage, DigitalBit bit) = 0;
    virtual DigitalPattern getDigitalPattern(DigitalStage stage, DigitalBit bit) = 0;
public:
    virtual GeneratorType getGeneratorType0() = 0;
    virtual int           getGeneratorOn0() = 0;
    virtual int           getGeneratorSlope0() = 0;
    virtual int           getGeneratorVoltage0() = 0;
    virtual int           getGeneratorOffset0() = 0;
    virtual float         getGeneratorFrequency0(float fs = 457142.81f) = 0;
    virtual float         getGeneratorSquareDuty0() = 0;
public:
    virtual GeneratorType getGeneratorType1() = 0;
    virtual int           getGeneratorOn1() = 0;
    virtual int           getGeneratorSlope1() = 0;
    virtual int           getGeneratorVoltage1() = 0;
    virtual int           getGeneratorOffset1() = 0;
    virtual float         getGeneratorFrequency1(float fs = 457142.81f) = 0;
    virtual float         getGeneratorSquareDuty1() = 0;
public:
    virtual double getDigitalVoltage() = 0;
    virtual int    getDigitalInputOutput15() = 0;
    virtual int    getDigitalInputOutput7() = 0;
    virtual int    getDigitalOutputBit(int bit) = 0;
    virtual uint   getDigitalClockDivide() = 0;
public:
    virtual ushort getAttr(uint volt) = 0;
    virtual ushort getGain(int channel, uint volt) = 0;
public:
    virtual int getAverage() = 0;
};

////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeControl1
//
////////////////////////////////////////////////////////////////////////////////
#ifdef PLATFORM_WIN
    #pragma pack(push,1)
#endif

class OsciloscopeControl1 : public OsciloscopeControlInterface
{
public:
    ushort control;
    ushort vgaina;
    ushort vgainb;
    short  offseta;
    short  offsetb;
    ushort siggen;
    ushort trigger;
    ushort analogswitch;
    ushort triggerMode;
    ushort triggerSource;
    ushort triggerSlope;
    ishort triggerLevel;
    ushort triggerHis;
    ushort triggerPercent;
    ushort xRange;
    ushort sampleSize;
    ushort holdoffH;
    ushort holdoffL;
    ushort generatorType0;
    ushort generatorVoltage0;
    short  generatorOffset0;
    ushort generatorDeltaH0;
    ushort generatorDeltaL0;
    ushort generatorSqueareDuty0;
    ushort generatorType1;
    ushort generatorVoltage1;
    short  generatorOffset1;
    ushort generatorDeltaH1;
    ushort generatorDeltaL1;
    ushort generatorSqueareDuty1;
    ushort digitalPattern1a;
    ushort digitalPattern1b;
    ushort digitalMask1;
    ushort digitalPattern2a;
    ushort digitalPattern2b;
    ushort digitalMask2;
    ushort digitalPattern3a;
    ushort digitalPattern3b;
    ushort digitalMask3;
    ushort digitalPattern4a;
    ushort digitalPattern4b;
    ushort digitalMask4;
    ushort dt_delayMaxcnt1;
    ushort dt_delayMaxcnt2;
    ushort dt_delayMaxcnt3;
    ushort dt_delayMaxcnt4;
    ushort dt_control;
    ushort digitalVoltage;
    ushort digitalInputOutput;
    ushort digitalOutputBit;
    ushort digitalOutputMask;
    ushort digitalClkDivideH;
    ushort digitalClkDivideL;
public:
    OsciloscopeControl1();
public:
    virtual void Default();
public:
    virtual void setAnalogSwitchBit(int bit, int value);
    virtual void setEts(int enable);
    virtual void setControl(uint control);
    virtual void setYRangeScaleA(uint voltage, float scale);
    virtual void setYRangeScaleA(uint gain, uint attr);
    virtual void setYPositionA(int pos);
    virtual void setYRangeScaleB(uint voltage, float scale);
    virtual void setYRangeScaleB(uint gain, uint attr);
    virtual void setYPositionB(int pos);
public:
    virtual void setTriggerSource(int source);
    virtual void setTriggerMode(int mode);
    virtual void setTriggerSlope(int mode);
    virtual void setTriggerReArm(bool on);
public:
    virtual void setTriggerPre(float perc);
    virtual void setTriggerHis(int perc);
    virtual void setTriggerLevel(int perc);
public:
    void setXRange(ishort range);
    void setSampleSize(uint frameSize);
    void setHoldoff(uint holdoff);
public:
    virtual void setDigitalStart(int start);
    virtual void setDigitalMode(int mode);
    virtual void setDigitalChannel(int channel);
public:
    virtual void setDigitalDelay(DigitalStage stage, ushort delay);
    virtual void setDigitalMask(DigitalStage stage, DigitalBit bit, int value);
    virtual void setDigitalPattern(DigitalStage stage, DigitalBit bit, DigitalPattern pattern);
public:
    virtual void setGeneratorType0(GeneratorType type);
    virtual void setGeneratorOn0(int onoff);
    virtual void setGeneratorSlope0(int onoff);
    virtual void setGeneratorVoltage0(int volt);
    virtual void setGeneratorOffset0(int perc);
    virtual void setGeneratorFrequency0(float freq, float fs = 457142.81f);
    virtual void setGeneratorSquareDuty0(float perc);
public:
    virtual void setGeneratorType1(GeneratorType type);
    virtual void setGeneratorOn1(int onoff);
    virtual void setGeneratorSlope1(int onoff);
    virtual void setGeneratorVoltage1(int volt);
    virtual void setGeneratorOffset1(int perc);
    virtual void setGeneratorFrequency1(float freq, float fs = 457142.81f);
    virtual void setGeneratorSquareDuty1(float perc);
public:
    virtual void setDigitalVoltage(double volt);
    virtual void setDigitalInputOutput(int inout15, int inout7);
    virtual void setDigitalOutputBit(int bit, int onoff);
    virtual void setDigitalClockDivide(uint divider);
public:
    virtual void setAverage(int enable) {};
public:
    virtual ushort getAnalogSwitch();
    virtual int   getEts();
    virtual uint  getControl();
    virtual uint  getYRangeA();
    virtual float getYScaleA();
    virtual int   getYPositionA();
public:
    virtual uint  getYRangeB();
    virtual float getYScaleB();
    virtual int   getYPositionB();
public:
    virtual ushort getTriggerSource();
    virtual ushort getTriggerMode();
    virtual ushort getTriggerSlope();
    virtual float  getTriggerPre();
    virtual int    getTriggerHis();
    virtual int    getTriggerLevel();
public:
    virtual uint   getSampleSize();
public:
    virtual ishort getXRange();
    virtual uint   getHoldoff();
public:
    virtual int   getDigitalStart();
    virtual int   getDigitalMode();
    virtual int   getDigitalChannel();
public:
    virtual ushort         getDigitalDelay(DigitalStage stage);
    virtual int            getDigitalMask(DigitalStage stage, DigitalBit bit);
    virtual DigitalPattern getDigitalPattern(DigitalStage stage, DigitalBit bit);
public:
    virtual GeneratorType getGeneratorType0();
    virtual int   getGeneratorOn0();
    virtual int   getGeneratorSlope0();
    virtual int   getGeneratorVoltage0();
    virtual int   getGeneratorOffset0();
    virtual float getGeneratorFrequency0(float fs = 457142.81f);
    virtual float getGeneratorSquareDuty0();
public:
    virtual GeneratorType getGeneratorType1();
    virtual int   getGeneratorOn1();
    virtual int   getGeneratorSlope1();
    virtual int   getGeneratorVoltage1();
    virtual int   getGeneratorOffset1();
    virtual float getGeneratorFrequency1(float fs = 457142.81f);
    virtual float getGeneratorSquareDuty1();
public:
    virtual double getDigitalVoltage();
    virtual int    getDigitalInputOutput15();
    virtual int    getDigitalInputOutput7();
    virtual int    getDigitalOutputBit(int bit);
    virtual uint   getDigitalClockDivide();
public:
    virtual ushort getAttr(uint volt);
    virtual ushort getGain(int channel, uint volt);
public:
    virtual int getAverage() {return 0;};
public:
    void       client1Set(SHardware1& configure);
    SHardware1 client1Get();
}
#if defined(PLATFORM_LINUX) || defined(PLATFORM_MAC)
    __attribute__((packed));
#else
    ;
#endif

#ifdef PLATFORM_WIN
    #pragma pack(pop)
#endif


////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeControl2
//
////////////////////////////////////////////////////////////////////////////////
#ifdef PLATFORM_WIN
    #pragma pack(push,1)
#endif

class OsciloscopeControl2 : public OsciloscopeControlInterface
{
public:
    ushort controlAddr;
    ushort controlData;
    ushort vgaina;
    ushort vgainb;
    ushort offseta;
    ushort offsetb;
    ushort analogswitch;
    ushort triggerMode;
    ushort triggerSource;
    ushort triggerSlope;
    short  triggerLevel;
    ushort triggerHis;
    ushort triggerPercent;
    ushort xRange;
    ushort holdoffH;
    ushort holdoffL;
    ushort sampleSizeH;
    ushort sampleSizeL;
    ushort generatorType0;
    ushort generatorVoltage0;
    short  generatorOffset0;
    ushort generatorDeltaH0;
    ushort generatorDeltaL0;
    ushort generatorSqueareDuty0;
    ushort generatorType1;
    ushort generatorVoltage1;
    short  generatorOffset1;
    ushort generatorDeltaH1;
    ushort generatorDeltaL1;
    ushort generatorSqueareDuty1;
    ushort digitalPattern1a;
    ushort digitalPattern1b;
    ushort digitalMask1;
    ushort digitalPattern2a;
    ushort digitalPattern2b;
    ushort digitalMask2;
    ushort digitalPattern3a;
    ushort digitalPattern3b;
    ushort digitalMask3;
    ushort digitalPattern4a;
    ushort digitalPattern4b;
    ushort digitalMask4;
    ushort dt_delayMaxcnt1;
    ushort dt_delayMaxcnt2;
    ushort dt_delayMaxcnt3;
    ushort dt_delayMaxcnt4;
    ushort dt_control;
    ushort digitalVoltage;
    ushort digitalInputOutput;
    ushort digitalOutputBit;
    ushort digitalOutputMask;
    ushort digitalClkDivideH;
    ushort digitalClkDivideL;
    ushort average;
    uint   reserved2;
    uint   reserved3;
    uint   reserved4;
    uint   reserved5;
    uint   reserved6;
public:
    OsciloscopeControl2();
public:
    virtual void Default();
public:
    virtual void setAnalogSwitchBit(int bit, int value);
    virtual void setEts(int enable);
    virtual void setControl(uint control);
    virtual void setYRangeScaleA(uint voltage, float scale);
    virtual void setYRangeScaleA(uint gain, uint attr);
    virtual void setYPositionA(int pos);
    virtual void setYRangeScaleB(uint voltage, float scale);
    virtual void setYRangeScaleB(uint gain, uint attr);
    virtual void setYPositionB(int pos);
public:
    virtual void setTriggerSource(int source);
    virtual void setTriggerMode(int mode);
    virtual void setTriggerSlope(int mode);
    virtual void setTriggerReArm(bool on);
public:
    virtual void setTriggerPre(float perc);
    virtual void setTriggerHis(int perc);
    virtual void setTriggerLevel(int perc);
public:
    virtual void setXRange(ishort range);
    virtual void setSampleSize(uint frameSize);
    virtual void setHoldoff(uint holdoff);
public:
    virtual void setDigitalStart(int start);
    virtual void setDigitalMode(int mode);
    virtual void setDigitalChannel(int channel);
public:
    virtual void setDigitalDelay(DigitalStage stage, ushort delay);
    virtual void setDigitalMask(DigitalStage stage, DigitalBit bit, int value);
    virtual void setDigitalPattern(DigitalStage stage, DigitalBit bit, DigitalPattern pattern);
public:
    virtual void setGeneratorType0(GeneratorType type);
    virtual void setGeneratorOn0(int onoff);
    virtual void setGeneratorSlope0(int onoff);
    virtual void setGeneratorVoltage0(int volt);
    virtual void setGeneratorOffset0(int perc);
    virtual void setGeneratorFrequency0(float freq, float fs = 457142.81f);
    virtual void setGeneratorSquareDuty0(float perc);
public:
    virtual void setGeneratorType1(GeneratorType type);
    virtual void setGeneratorOn1(int onoff);
    virtual void setGeneratorSlope1(int onoff);
    virtual void setGeneratorVoltage1(int volt);
    virtual void setGeneratorOffset1(int perc);
    virtual void setGeneratorFrequency1(float freq, float fs = 457142.81f);
    virtual void setGeneratorSquareDuty1(float perc);
public:
    virtual void setDigitalVoltage(double volt);
    virtual void setDigitalInputOutput(int inout15, int inout7);
    virtual void setDigitalOutputBit(int bit, int onoff);
    virtual void setDigitalClockDivide(uint divider);
public:
    virtual void setAverage(int enable);
public:
    virtual ushort getAnalogSwitch();
    virtual int   getEts();
    virtual uint  getControl();
    virtual uint  getYRangeA();
    virtual float getYScaleA();
    virtual int   getYPositionA();
public:
    virtual uint  getYRangeB();
    virtual float getYScaleB();
    virtual int   getYPositionB();
public:
    virtual ushort getTriggerSource();
    virtual ushort getTriggerMode();
    virtual ushort getTriggerSlope();
    virtual float  getTriggerPre();
    virtual int    getTriggerHis();
    virtual int    getTriggerLevel();
public:
    virtual uint   getSampleSize();
public:
    virtual ishort getXRange();
    virtual uint   getHoldoff();
public:
    virtual int   getDigitalStart();
    virtual int   getDigitalMode();
    virtual int   getDigitalChannel();
public:
    virtual ushort         getDigitalDelay(DigitalStage stage);
    virtual int            getDigitalMask(DigitalStage stage, DigitalBit bit);
    virtual DigitalPattern getDigitalPattern(DigitalStage stage, DigitalBit bit);
public:
    virtual GeneratorType getGeneratorType0();
    virtual int   getGeneratorOn0();
    virtual int   getGeneratorSlope0();
    virtual int   getGeneratorVoltage0();
    virtual int   getGeneratorOffset0();
    virtual float getGeneratorFrequency0(float fs = 457142.81f);
    virtual float getGeneratorSquareDuty0();
public:
    virtual GeneratorType getGeneratorType1();
    virtual int   getGeneratorOn1();
    virtual int   getGeneratorSlope1();
    virtual int   getGeneratorVoltage1();
    virtual int   getGeneratorOffset1();
    virtual float getGeneratorFrequency1(float fs = 457142.81f);
    virtual float getGeneratorSquareDuty1();
public:
    virtual double getDigitalVoltage();
    virtual int    getDigitalInputOutput15();
    virtual int    getDigitalInputOutput7();
    virtual int    getDigitalOutputBit(int bit);
    virtual uint   getDigitalClockDivide();
public:
    virtual ushort getAttr(uint volt);
    virtual ushort getGain(int channel, uint volt);
public:
    virtual int getAverage();
public:
    void       client2Set(SHardware2& configure);
    SHardware2 client2Get();
}
#if defined(PLATFORM_LINUX) || defined(PLATFORM_MAC)
    __attribute__((packed));
#else
    ;
#endif

#ifdef PLATFORM_WIN
    #pragma pack(pop)
#endif

////////////////////////////////////////////////////////////////////////////////
//
// OsciloscopeMainControl
//
////////////////////////////////////////////////////////////////////////////////
class OsciloscopeMainControl : public OsciloscopeControlInterface
{
private:
    SHardware1                   hw1;
    SHardware2                   hw2;
    SDL_SpinLock                 lock;
    int                          version;
    OsciloscopeControlInterface* pControl;
public:
    OsciloscopeControl1          control1;
    OsciloscopeControl2          control2;
public:
    OsciloscopeMainControl();
public:
    void setVersion(int version);
    int  getVersion()
    {
        return version;
    };
public:
    void transferData();
public:
    void setWindow(WndMain& main);
public:
    // default
    virtual void Default();
public:
    // set
    virtual void setAnalogSwitchBit(int bit, int value);
    virtual void setEts(int enable);
    virtual void setControl(uint control);
    virtual void setYRangeScaleA(uint voltage, float scale);
    virtual void setYRangeScaleA(uint gain, uint attr);
    virtual void setYPositionA(int pos);
    virtual void setYRangeScaleB(uint voltage, float scale);
    virtual void setYRangeScaleB(uint gain, uint attr);
    virtual void setYPositionB(int pos);
public:
    virtual void setTriggerSource(int source);
    virtual void setTriggerMode(int mode);
    virtual void setTriggerSlope(int mode);
    virtual void setTriggerReArm(bool on);
    virtual void setTriggerPre(float perc);
    virtual void setTriggerHis(int perc);
    virtual void setTriggerLevel(int perc);
public:
    virtual void setXRange(ishort range);
    virtual void setSampleSize(uint frameSize);
    virtual void setHoldoff(uint holdoff);
public:
    virtual void setDigitalStart(int start);
    virtual void setDigitalMode(int mode);
    virtual void setDigitalChannel(int channel);
    virtual void setDigitalDelay(DigitalStage stage, ushort delay);
    virtual void setDigitalMask(DigitalStage stage, DigitalBit bit, int value);
    virtual void setDigitalPattern(DigitalStage stage, DigitalBit bit, DigitalPattern pattern);
public:
    virtual void setGeneratorType0(GeneratorType type);
    virtual void setGeneratorOn0(int onoff);
    virtual void setGeneratorSlope0(int onoff);
    virtual void setGeneratorVoltage0(int volt);
    virtual void setGeneratorOffset0(int perc);
    virtual void setGeneratorFrequency0(float freq, float fs = 457142.81f);
    virtual void setGeneratorSquareDuty0(float perc);
public:
    virtual void setGeneratorType1(GeneratorType type);
    virtual void setGeneratorOn1(int onoff);
    virtual void setGeneratorSlope1(int onoff);
    virtual void setGeneratorVoltage1(int volt);
    virtual void setGeneratorOffset1(int perc);
    virtual void setGeneratorFrequency1(float freq, float fs = 457142.81f);
    virtual void setGeneratorSquareDuty1(float perc);
public:
    virtual void setDigitalVoltage(double volt);
    virtual void setDigitalInputOutput(int inout15, int inout7);
    virtual void setDigitalOutputBit(int bit, int onoff);
    virtual void setDigitalClockDivide(uint divider);
public:
    virtual void setAverage(int enable);
public:
    // get
    virtual ushort getAnalogSwitch();
    virtual int   getEts();
    virtual uint  getControl();
    virtual uint  getYRangeA();
    virtual float getYScaleA();
    virtual int   getYPositionA();
    virtual uint  getYRangeB();
    virtual float getYScaleB();
    virtual int   getYPositionB();
public:
    virtual ushort getTriggerSource();
    virtual ushort getTriggerMode();
    virtual ushort getTriggerSlope();
    virtual float  getTriggerPre();
    virtual int    getTriggerHis();
    virtual int    getTriggerLevel();
public:
    virtual uint   getSampleSize();
    virtual ishort getXRange();
    virtual uint   getHoldoff();
public:
    virtual int   getDigitalStart();
    virtual int   getDigitalMode();
    virtual int   getDigitalChannel();
public:
    virtual ushort         getDigitalDelay(DigitalStage stage);
    virtual int            getDigitalMask(DigitalStage stage, DigitalBit bit);
    virtual DigitalPattern getDigitalPattern(DigitalStage stage, DigitalBit bit);
public:
    virtual GeneratorType getGeneratorType0();
    virtual int           getGeneratorOn0();
    virtual int           getGeneratorSlope0();
    virtual int           getGeneratorVoltage0();
    virtual int           getGeneratorOffset0();
    virtual float         getGeneratorFrequency0(float fs = 457142.81f);
    virtual float         getGeneratorSquareDuty0();
public:
    virtual GeneratorType getGeneratorType1();
    virtual int           getGeneratorOn1();
    virtual int           getGeneratorSlope1();
    virtual int           getGeneratorVoltage1();
    virtual int           getGeneratorOffset1();
    virtual float         getGeneratorFrequency1(float fs = 457142.81f);
    virtual float         getGeneratorSquareDuty1();
public:
    virtual double getDigitalVoltage();
    virtual int    getDigitalInputOutput15();
    virtual int    getDigitalInputOutput7();
    virtual int    getDigitalOutputBit(int bit);
    virtual uint   getDigitalClockDivide();
public:
    virtual int getAverage();
public:
    virtual ushort getAttr(uint volt);
    virtual ushort getGain(int channel, uint volt);
};


#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
