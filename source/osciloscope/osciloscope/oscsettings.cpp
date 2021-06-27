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


int jsonToInt(cJSON* json)
{
    if(json->type == cJSON_Number)
    {
        return json->valueint;
    }
    CORE_ERROR("Wrong json type", 0);
    return 0;
}

double jsonToDouble(cJSON* json)
{
    if(json->type == cJSON_Number)
    {
        return json->valuedouble;
    }
    CORE_ERROR("Wrong json type", 0);
    return 0;
}

const char* jsonToString(cJSON* json)
{
    if(json->type == cJSON_String)
    {
        return json->valuestring;
    }
    CORE_ERROR("Wrong json type", 0);
    return 0;
}

void fillGuid(UsbGuid& usbGuid, const char* str)
{
    uint data[11] = { 0 };
    sscanf(str, "0x%8x, 0x%4x, 0x%4x, 0x%2x, 0x%2x, 0x%2x, 0x%2x, 0x%2x, 0x%2x, 0x%2x, 0x%2x", &data[0],
           &data[1],
           &data[2],
           &data[3],
           &data[4],
           &data[5],
           &data[6],
           &data[7],
           &data[8],
           &data[9],
           &data[10]);
    usbGuid.data1 = data[0];
    usbGuid.data2 = data[1];
    usbGuid.data3 = data[2];
    usbGuid.data4[0] = data[3];
    usbGuid.data4[1] = data[4];
    usbGuid.data4[2] = data[5];
    usbGuid.data4[3] = data[6];
    usbGuid.data4[4] = data[7];
    usbGuid.data4[5] = data[8];
    usbGuid.data4[6] = data[9];
    usbGuid.data4[7] = data[10];
}



void VoltageFromJson(cJSON* json, ushort* output)
{
    if(json && output)
    {
        cJSON* volt2 = cJSON_GetObjectItem(json, "volt2");
        cJSON* volt1 = cJSON_GetObjectItem(json, "volt1");
        cJSON* mili500 = cJSON_GetObjectItem(json, "mili500");
        cJSON* mili200 = cJSON_GetObjectItem(json, "mili200");
        cJSON* mili100 = cJSON_GetObjectItem(json, "mili100");
        cJSON* mili50 = cJSON_GetObjectItem(json, "mili50");
        cJSON* mili20 = cJSON_GetObjectItem(json, "mili20");
        cJSON* mili10 = cJSON_GetObjectItem(json, "mili10");
        if(volt2)
        {
            output[vc2Volt] = jsonToDouble(volt2);
        }
        if(volt1)
        {
            output[vc1Volt] = jsonToDouble(volt1);
        }
        if(mili500)
        {
            output[vc500Mili] = jsonToDouble(mili500);
        }
        if(mili200)
        {
            output[vc200Mili] = jsonToDouble(mili200);
        }
        if(mili100)
        {
            output[vc100Mili] = jsonToDouble(mili100);
        }
        if(mili50)
        {
            output[vc50Mili] = jsonToDouble(mili50);
        }
        if(mili20)
        {
            output[vc20Mili] = jsonToDouble(mili20);
        }
        if(mili10)
        {
            output[vc10Mili] = jsonToDouble(mili10);
        }
    }
}

void VoltageFromJson(cJSON* json, double* output)
{
    if(json && output)
    {
        cJSON* volt2 = cJSON_GetObjectItem(json, "volt2");
        cJSON* volt1 = cJSON_GetObjectItem(json, "volt1");
        cJSON* mili500 = cJSON_GetObjectItem(json, "mili500");
        cJSON* mili200 = cJSON_GetObjectItem(json, "mili200");
        cJSON* mili100 = cJSON_GetObjectItem(json, "mili100");
        cJSON* mili50 = cJSON_GetObjectItem(json, "mili50");
        cJSON* mili20 = cJSON_GetObjectItem(json, "mili20");
        cJSON* mili10 = cJSON_GetObjectItem(json, "mili10");
        if(volt2)
        {
            output[vc2Volt] = jsonToDouble(volt2)/100;
        }
        if(volt1)
        {
            output[vc1Volt] = jsonToDouble(volt1) / 100;
        }
        if(mili500)
        {
            output[vc500Mili] = jsonToDouble(mili500) / 100;
        }
        if(mili200)
        {
            output[vc200Mili] = jsonToDouble(mili200) / 100;
        }
        if(mili100)
        {
            output[vc100Mili] = jsonToDouble(mili100) / 100;
        }
        if(mili50)
        {
            output[vc50Mili] = jsonToDouble(mili50) / 100;
        }
        if(mili20)
        {
            output[vc20Mili] = jsonToDouble(mili20) / 100;
        }
        if(mili10)
        {
            output[vc10Mili] = jsonToDouble(mili10) / 100;
        }
    }
}

void VoltageFromJson(cJSON* json, int* output)
{
    if(json && output)
    {
        cJSON* volt2 = cJSON_GetObjectItem(json, "volt2");
        cJSON* volt1 = cJSON_GetObjectItem(json, "volt1");
        cJSON* mili500 = cJSON_GetObjectItem(json, "mili500");
        cJSON* mili200 = cJSON_GetObjectItem(json, "mili200");
        cJSON* mili100 = cJSON_GetObjectItem(json, "mili100");
        cJSON* mili50 = cJSON_GetObjectItem(json, "mili50");
        cJSON* mili20 = cJSON_GetObjectItem(json, "mili20");
        cJSON* mili10 = cJSON_GetObjectItem(json, "mili10");
        if(volt2)
        {
            output[vc2Volt] = jsonToDouble(volt2);
        }
        if(volt1)
        {
            output[vc1Volt] = jsonToDouble(volt1);
        }
        if(mili500)
        {
            output[vc500Mili] = jsonToDouble(mili500);
        }
        if(mili200)
        {
            output[vc200Mili] = jsonToDouble(mili200);
        }
        if(mili100)
        {
            output[vc100Mili] = jsonToDouble(mili100);
        }
        if(mili50)
        {
            output[vc50Mili] = jsonToDouble(mili50);
        }
        if(mili20)
        {
            output[vc20Mili] = jsonToDouble(mili20);
        }
        if(mili10)
        {
            output[vc10Mili] = jsonToDouble(mili10);
        }
    }
}

void VoltageToJson(cJSON* json, ushort* input)
{
    if(json && input)
    {
        cJSON_AddItemToObject(json, "volt2", cJSON_CreateNumber(input[vc2Volt]));
        cJSON_AddItemToObject(json, "volt1", cJSON_CreateNumber(input[vc1Volt]));
        cJSON_AddItemToObject(json, "mili500", cJSON_CreateNumber(input[vc500Mili]));
        cJSON_AddItemToObject(json, "mili200", cJSON_CreateNumber(input[vc200Mili]));
        cJSON_AddItemToObject(json, "mili100", cJSON_CreateNumber(input[vc100Mili]));
        cJSON_AddItemToObject(json, "mili50", cJSON_CreateNumber(input[vc50Mili]));
        cJSON_AddItemToObject(json, "mili20", cJSON_CreateNumber(input[vc20Mili]));
        cJSON_AddItemToObject(json, "mili10", cJSON_CreateNumber(input[vc10Mili]));
    }
}


void VoltageToJson(cJSON* json, double* input)
{
    if(json && input)
    {
        cJSON_AddItemToObject(json, "volt2",   cJSON_CreateNumber(100 * input[vc2Volt]));
        cJSON_AddItemToObject(json, "volt1",   cJSON_CreateNumber(100 * input[vc1Volt]));
        cJSON_AddItemToObject(json, "mili500", cJSON_CreateNumber(100 * input[vc500Mili]));
        cJSON_AddItemToObject(json, "mili200", cJSON_CreateNumber(100 * input[vc200Mili]));
        cJSON_AddItemToObject(json, "mili100", cJSON_CreateNumber(100 * input[vc100Mili]));
        cJSON_AddItemToObject(json, "mili50",  cJSON_CreateNumber(100 * input[vc50Mili]));
        cJSON_AddItemToObject(json, "mili20",  cJSON_CreateNumber(100 * input[vc20Mili]));
        cJSON_AddItemToObject(json, "mili10",  cJSON_CreateNumber(100 * input[vc10Mili]));
    }
}


void VoltageToJson(cJSON* json, int* input)
{
    if(json && input)
    {
        cJSON_AddItemToObject(json, "volt2", cJSON_CreateNumber(input[vc2Volt]));
        cJSON_AddItemToObject(json, "volt1", cJSON_CreateNumber(input[vc1Volt]));
        cJSON_AddItemToObject(json, "mili500", cJSON_CreateNumber(input[vc500Mili]));
        cJSON_AddItemToObject(json, "mili200", cJSON_CreateNumber(input[vc200Mili]));
        cJSON_AddItemToObject(json, "mili100", cJSON_CreateNumber(input[vc100Mili]));
        cJSON_AddItemToObject(json, "mili50", cJSON_CreateNumber(input[vc50Mili]));
        cJSON_AddItemToObject(json, "mili20", cJSON_CreateNumber(input[vc20Mili]));
        cJSON_AddItemToObject(json, "mili10", cJSON_CreateNumber(input[vc10Mili]));
    }
}


////////////////////////////////////////////////////////////////////////////////
//
// OscHardware
//
////////////////////////////////////////////////////////////////////////////////
OscHardware::OscHardware(int iversion)
{
    SDL_memset(this, 0, sizeof(OscHardware));
    version = iversion;
}

SUsb OscHardware::getUSB()
{
    SUsb usb = { 0 };
    usb.guid.data1 = usbGuid.data1;
    usb.guid.data2 = usbGuid.data2;
    usb.guid.data3 = usbGuid.data3;
    for(int i = 0; i < 8; i++)
    {
        usb.guid.data4[i] = usbGuid.data4[i];
    }
    usb.idProduct = usbProduct;
    usb.idVendor  = usbVendor;
    usb.idSerial  = usbSerial;
    return usb;
}

int OscHardware::getAnalogOffset(float time, int ch, float volt)
{
    ECallibrationType type = getCallibrationType(time);
    uint             index = captureVoltFromValue(volt);
    return callibratedOffsets[type][ch][index];
}

double OscHardware::getAnalogOffsetDouble(float time, int ch, float volt)
{
    ECallibrationType type = getCallibrationType(time);
    uint             index = captureVoltFromValue(volt);
    int offset = callibratedOffsets[type][ch][index];
    double koeficient = 10*double( captureVoltFromEnum(index) ) / 1024.0;
    return koeficient*offset;
}
double OscHardware::getAnalogStep(float time, int ch, float volt)
{
    ECallibrationType type = getCallibrationType(time);
    uint              index = captureVoltFromValue(volt);
    return callibratedVoltageStep[type][ch][index];
}

ushort OscHardware::getAnalogGain(float time, int ch, float volt)
{
    ECallibrationType type = getCallibrationType(time);
    uint              index = captureVoltFromValue(volt);
    return callibratedGainValue[type][ch][index];
}

int OscHardware::getGeneratorOffset(float time, int ch)
{
    ECallibrationType type = getCallibrationType(time);
    return callibratedOffsetsGenerator[type][ch];
}

void OscHardware::loadCallibrated(cJSON* jsonCallibrated, ECallibrationType type)
{
    FORMAT_BUFFER();
    for(int i = 0; i < 2; i++)
    {
        FORMAT("offsetsCh%d", i);
        cJSON* jsonOffsetsChannel = cJSON_GetObjectItem(jsonCallibrated, formatBuffer);
        VoltageFromJson(jsonOffsetsChannel, (int*)callibratedOffsets[type][i]);
    }
    for(int i = 0; i < 2; i++)
    {
        FORMAT("voltageStepCh%d", i);
        cJSON* jsonStep = cJSON_GetObjectItem(jsonCallibrated, formatBuffer);
        VoltageFromJson(jsonStep, (double*)callibratedVoltageStep[type][i]);
    }
    for(int i = 0; i < 2; i++)
    {
        FORMAT("gainValueCh%d", i);
        cJSON* jsonGainValue = cJSON_GetObjectItem(jsonCallibrated, formatBuffer);
        VoltageFromJson(jsonGainValue, (ushort*)callibratedGainValue[type][i]);
    }
    cJSON* jsonGenerator = cJSON_GetObjectItem(jsonCallibrated, "offsetsGenerator");
    if(jsonGenerator)
    {
        cJSON* ch0 = cJSON_GetObjectItem(jsonGenerator, "channel0");
        cJSON* ch1 = cJSON_GetObjectItem(jsonGenerator, "channel1");
        if(ch0)
        {
            callibratedOffsetsGenerator[type][0] = jsonToInt(ch0);
        }
        if(ch1)
        {
            callibratedOffsetsGenerator[type][1] = jsonToInt(ch1);
        }
    }
}

void OscHardware::load()
{
    FORMAT_BUFFER();
    if(version == HARDWARE_VERSION_1)
    {
        FORMAT_PATH("data/startup/hardware1.json");
    }
    if(version == HARDWARE_VERSION_2)
    {
        FORMAT_PATH("data/startup/hardware2.json");
    }
    char*  memory = 0;
    ilarge memorySize = 0;
    fileLoadString(formatBuffer, &memory, &memorySize);
    json = cJSON_Parse((const char*)memory);
    if(!json)
    {
        CORE_MESSAGE("hardware%d.json error before: [%.256s]\n", version, cJSON_GetErrorPtr());
    }
    fileFree(memory);
    cJSON* jUsb = cJSON_GetObjectItem(json, "usb");
    if(jUsb)
    {
        cJSON* jguid    = cJSON_GetObjectItem(jUsb, "guid");
        cJSON* jvendor  = cJSON_GetObjectItem(jUsb, "vendor");
        cJSON* jproduct = cJSON_GetObjectItem(jUsb, "product");
        cJSON* jserial  = cJSON_GetObjectItem(jUsb, "serial");
        cJSON* jfw      = cJSON_GetObjectItem(jUsb, "firmware");
        if(jguid)
        {
            fillGuid(usbGuid, jsonToString(jguid));
        }
        if(jvendor)
        {
            usbVendor   = jsonToInt(jvendor);
        }
        if(jproduct)
        {
            usbProduct  = jsonToInt(jproduct);
        }
        if(jserial)
        {
            usbSerial   = jsonToInt(jserial);
        }
        if(jfw)
        {
            usbFirmware = jsonToString(jfw);
        }
    }
    cJSON* jfpga = cJSON_GetObjectItem(json, "fpga");
    if(jfpga)
    {
        cJSON* jfw       = cJSON_GetObjectItem(jfpga, "firmware");
        cJSON* jEtsCount = cJSON_GetObjectItem(jfpga, "etsCount");
        cJSON* jEtsIndex = cJSON_GetObjectItem(jfpga, "etsIndex");
        if(jfw)
        {
            fpgaFirmware = jsonToString(jfw);
        }
        if(jEtsCount)
        {
            fpgaEtsCount = jsonToInt(jEtsCount);
        }
        if(jEtsIndex)
        {
            fpgaEtsIndex = jsonToInt(jEtsIndex);
        }
    }
    cJSON* jsonDigital = cJSON_GetObjectItem(json, "digital");
    if(jsonDigital)
    {
        cJSON* jVoltageCoeficient = cJSON_GetObjectItem(jsonDigital, "voltageCoeficient");
        if(jVoltageCoeficient)
        {
            digitalVoltageCoeficient = jsonToInt(jVoltageCoeficient);
        }
    }
    cJSON* jsonGenerator = cJSON_GetObjectItem(json, "generator");
    if(jsonGenerator)
    {
        cJSON* jfs = cJSON_GetObjectItem(jsonGenerator, "fs");
        if(jfs)
        {
            generatorFs = jsonToDouble(jfs);
        }
    }
    cJSON* jsonRef = cJSON_GetObjectItem(json, "reference");
    if(jsonRef)
    {
        cJSON* invCh0 = cJSON_GetObjectItem(jsonRef, "invertCh0");
        cJSON* invCh1 = cJSON_GetObjectItem(jsonRef, "invertCh1");
        cJSON* frames = cJSON_GetObjectItem(jsonRef, "framesPerCapture");
        cJSON* maxIt = cJSON_GetObjectItem(jsonRef, "maxIterations");
        cJSON* exitPerc = cJSON_GetObjectItem(jsonRef, "exitPercentage");
        cJSON* offsetMax = cJSON_GetObjectItem(jsonRef, "offsetMax");
        cJSON* offsetMin = cJSON_GetObjectItem(jsonRef, "offsetMin");
        cJSON* generatorRef = cJSON_GetObjectItem(jsonRef, "generatorReference");
        cJSON* generatorMax = cJSON_GetObjectItem(jsonRef, "generatorMax");
        cJSON* generatorMin = cJSON_GetObjectItem(jsonRef, "generatorMin");
        cJSON* generatorStep = cJSON_GetObjectItem(jsonRef, "generatorStep");
        cJSON* stepMax = cJSON_GetObjectItem(jsonRef, "stepMax");
        cJSON* stepMin = cJSON_GetObjectItem(jsonRef, "stepMin");
        if(invCh0)
        {
            referenceInvert[0] = jsonToInt(invCh0);
        }
        if(invCh1)
        {
            referenceInvert[1] = jsonToInt(invCh1);
        }
        if(frames)
        {
            referenceFramesPerCapture = jsonToInt(frames);
        }
        if(maxIt)
        {
            referenceMaxIterations = jsonToInt(maxIt);
        }
        if(exitPerc)
        {
            referenceExitPercentage = jsonToDouble(exitPerc);
        }
        if(offsetMax)
        {
            referenceOffsetMaxValue = jsonToDouble(offsetMax);
        }
        if(offsetMin)
        {
            referenceOffsetMinValue = jsonToDouble(offsetMin);
        }
        if(generatorRef)
        {
            referenceGenerator = jsonToInt(generatorRef);
        }
        if(generatorMax)
        {
            referenceGeneratorMaxValue = jsonToInt(generatorMax);
        }
        if(generatorMin)
        {
            referenceGeneratorMinValue = jsonToInt(generatorMin);
        }
        if(generatorStep)
        {
            referenceGeneratorVoltagePerStep = jsonToDouble(generatorStep);
        }
        if(stepMax)
        {
            referenceStepMax = jsonToInt(stepMax);
        }
        if(stepMin)
        {
            referenceStepMin = jsonToInt(stepMin);
        }
        cJSON* jsonGainRefValue = cJSON_GetObjectItem(jsonRef, "gainValue");
        VoltageFromJson(jsonGainRefValue, (ushort*)referenceGainValue);
        cJSON* jsonGainRefVoltage = cJSON_GetObjectItem(jsonRef, "gainVoltage");
        VoltageFromJson(jsonGainRefVoltage, (double*)referenceGainVoltage);
        cJSON* gMin = cJSON_GetObjectItem(jsonRef, "gainMin");
        VoltageFromJson(gMin, (ushort*)referenceGainMin);
        cJSON* gMax = cJSON_GetObjectItem(jsonRef, "gainMax");
        VoltageFromJson(gMax, (ushort*)referenceGainMax);
        cJSON* gAttr = cJSON_GetObjectItem(jsonRef, "gainAttr");
        VoltageFromJson(gAttr, (ushort*)referenceGainAttr);
    }
    cJSON* jsonCallibratedNormal = cJSON_GetObjectItem(json, "callibratedNormal");
    if(jsonCallibratedNormal)
    {
        loadCallibrated(jsonCallibratedNormal, ctNormal);
    }
    cJSON* jsonCallibrated500Mhz = cJSON_GetObjectItem(json, "callibrated500Mhz");
    if(jsonCallibrated500Mhz)
    {
        loadCallibrated(jsonCallibrated500Mhz, ct500Mhz);
    }
}

void OscHardware::saveCallibrated(cJSON* jsonCallibrated, ECallibrationType type)
{
    cJSON* jsonOffsetChannel0 = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonCallibrated, "offsetsCh0", jsonOffsetChannel0);
    VoltageToJson(jsonOffsetChannel0, (int*)callibratedOffsets[type][0]);
    cJSON* jsonOffsetChannel1 = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonCallibrated, "offsetsCh1", jsonOffsetChannel1);
    VoltageToJson(jsonOffsetChannel1, (int*)callibratedOffsets[type][1]);
    cJSON* jsonStep0 = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonCallibrated, "voltageStepCh0", jsonStep0);
    VoltageToJson(jsonStep0, (double*)callibratedVoltageStep[type][0]);
    cJSON* jsonStep1 = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonCallibrated, "voltageStepCh1", jsonStep1);
    VoltageToJson(jsonStep1, (double*)callibratedVoltageStep[type][1]);
    cJSON* jsonGainVolt0 = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonCallibrated, "gainValueCh0", jsonGainVolt0);
    VoltageToJson(jsonGainVolt0, (ushort*)callibratedGainValue[type][0]);
    cJSON* jsonGainVolt1 = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonCallibrated, "gainValueCh1", jsonGainVolt1);
    VoltageToJson(jsonGainVolt1, (ushort*)callibratedGainValue[type][1]);
    cJSON* jsonGeneratorOffsets = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonCallibrated, "offsetsGenerator", jsonGeneratorOffsets);
    cJSON_AddItemToObject(jsonGeneratorOffsets, "channel0", cJSON_CreateNumber(this->callibratedOffsetsGenerator[type][0]));
    cJSON_AddItemToObject(jsonGeneratorOffsets, "channel1", cJSON_CreateNumber(this->callibratedOffsetsGenerator[type][1]));
}

void OscHardware::save()
{
    // create
    cJSON* jsonRoot = cJSON_CreateObject();
    // usb
    cJSON* jsonUSB = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonRoot, "usb", jsonUSB);
    cJSON_AddItemToObject(jsonUSB, "guid", cJSON_CreateString(pFormat->formatGuid(this->usbGuid)));
    cJSON_AddItemToObject(jsonUSB, "vendor", cJSON_CreateNumber(this->usbVendor));
    cJSON_AddItemToObject(jsonUSB, "product", cJSON_CreateNumber(this->usbProduct));
    cJSON_AddItemToObject(jsonUSB, "serial", cJSON_CreateNumber(this->usbSerial));
    cJSON_AddItemToObject(jsonUSB, "firmware", cJSON_CreateString(this->usbFirmware.asChar()));
    // fpga
    cJSON* jsonFPGA = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonRoot, "fpga", jsonFPGA);
    cJSON_AddItemToObject(jsonFPGA, "firmware", cJSON_CreateString(this->fpgaFirmware.asChar()));
    cJSON_AddItemToObject(jsonFPGA, "etsIndex", cJSON_CreateNumber(this->fpgaEtsIndex));
    cJSON_AddItemToObject(jsonFPGA, "etsCount", cJSON_CreateNumber(this->fpgaEtsCount));
    // digital
    cJSON* jsonDigital = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonRoot, "digital", jsonDigital);
    cJSON_AddItemToObject(jsonDigital, "voltageCoeficient", cJSON_CreateNumber(this->digitalVoltageCoeficient));
    // generator
    cJSON* jsonGenerator = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonRoot, "generator", jsonGenerator);
    cJSON_AddItemToObject(jsonGenerator, "fs", cJSON_CreateNumber(this->generatorFs));
    // reference
    cJSON* jsonRef = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonRoot, "reference", jsonRef);
    cJSON_AddItemToObject(jsonRef, "invertCh0", cJSON_CreateNumber(this->referenceInvert[0]));
    cJSON_AddItemToObject(jsonRef, "invertCh1", cJSON_CreateNumber(this->referenceInvert[1]));
    cJSON_AddItemToObject(jsonRef, "framesPerCapture", cJSON_CreateNumber(this->referenceFramesPerCapture));
    cJSON_AddItemToObject(jsonRef, "maxIterations", cJSON_CreateNumber(this->referenceMaxIterations));
    cJSON_AddItemToObject(jsonRef, "exitPercentage", cJSON_CreateNumber(this->referenceExitPercentage));
    cJSON_AddItemToObject(jsonRef, "offsetMax", cJSON_CreateNumber(this->referenceOffsetMaxValue));
    cJSON_AddItemToObject(jsonRef, "offsetMin", cJSON_CreateNumber(this->referenceOffsetMinValue));
    cJSON_AddItemToObject(jsonRef, "generatorReference", cJSON_CreateNumber(this->referenceGenerator));
    cJSON_AddItemToObject(jsonRef, "generatorMax", cJSON_CreateNumber(this->referenceGeneratorMaxValue));
    cJSON_AddItemToObject(jsonRef, "generatorMin", cJSON_CreateNumber(this->referenceGeneratorMinValue));
    cJSON_AddItemToObject(jsonRef, "generatorStep", cJSON_CreateNumber(this->referenceGeneratorVoltagePerStep));
    cJSON_AddItemToObject(jsonRef, "stepMin", cJSON_CreateNumber(this->referenceStepMin));
    cJSON_AddItemToObject(jsonRef, "stepMax", cJSON_CreateNumber(this->referenceStepMax));
    cJSON* jsonGainRefValue = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonRef, "gainValue", jsonGainRefValue);
    VoltageToJson(jsonGainRefValue, (ushort*)referenceGainValue);
    cJSON* jsonGainRefVoltage = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonRef, "gainVoltage", jsonGainRefVoltage);
    VoltageToJson(jsonGainRefVoltage, (double*)referenceGainVoltage);
    cJSON* jsonGainMin = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonRef, "gainMin", jsonGainMin);
    VoltageToJson(jsonGainMin, (ushort*)referenceGainMin);
    cJSON* jsonGainMax = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonRef, "gainMax", jsonGainMax);
    VoltageToJson(jsonGainMax, (ushort*)referenceGainMax);
    cJSON* jsonGainAttr = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonRef, "gainAttr", jsonGainAttr);
    VoltageToJson(jsonGainAttr, (ushort*)referenceGainAttr);
    // callibrated
    cJSON* jsonCallibratedNormal = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonRoot, "callibratedNormal", jsonCallibratedNormal);
    saveCallibrated(jsonCallibratedNormal, ctNormal);
    cJSON* jsonCallibrated500Mhz = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonRoot, "callibrated500Mhz", jsonCallibrated500Mhz);
    saveCallibrated(jsonCallibrated500Mhz, ct500Mhz);
    // save
    setlocale(LC_ALL, "C");
    char* jsonString = cJSON_Print(jsonRoot);
    if(!jsonString)
    {
        CORE_MESSAGE("settings.json error before: [%.256s]\n", cJSON_GetErrorPtr());
    }
    FORMAT_BUFFER();
    if(version == HARDWARE_VERSION_1)
    {
        FORMAT_PATH("data/startup/hardware1.json");
    }
    if(version == HARDWARE_VERSION_2)
    {
        FORMAT_PATH("data/startup/hardware2.json");
    }
    fileSaveString(formatBuffer, jsonString);
    cJSON_Delete(jsonRoot);
    pMemory->free(jsonString);
}

////////////////////////////////////////////////////////////////////////////////
//
// OscSettings
//
////////////////////////////////////////////////////////////////////////////////
OscSettings::OscSettings()
{
    SDL_memset(this, 0, sizeof(OscSettings));
}

void OscSettings::load()
{
    // load
    FORMAT_BUFFER();
    FORMAT_PATH("data/startup/settings.json");
    char*  memory = 0;
    ilarge memorySize = 0;
    fileLoadString(formatBuffer, &memory, &memorySize);
    json = cJSON_Parse((const char*)memory);
    if(!json)
    {
        CORE_MESSAGE("settings.json error before: [%.256s]\n", cJSON_GetErrorPtr());
    }
    fileFree(memory);
    // set
    cJSON* render = cJSON_GetObjectItem(json, "render");
    if(render)
    {
        cJSON* jRenderShaders21                = cJSON_GetObjectItem(render, "shaders21");
        cJSON* jRenderVertexBufferSizeMegaByte = cJSON_GetObjectItem(render, "vertexBufferSizeMegaByte");
        cJSON* jRenderDepthBuffer              = cJSON_GetObjectItem(render, "depthBuffer");
        cJSON* jRenderFps                      = cJSON_GetObjectItem(render, "fps");
        cJSON* jRenderEventTimer               = cJSON_GetObjectItem(render, "eventTime");
        cJSON* jRenderThreadCount              = cJSON_GetObjectItem(render, "threadCount");
        if(jRenderShaders21)
        {
            renderShaders21                = jsonToInt(jRenderShaders21);
        }
        if(jRenderVertexBufferSizeMegaByte)
        {
            renderVertexBufferSizeMegaByte = jsonToInt(jRenderVertexBufferSizeMegaByte);
        }
        if(jRenderDepthBuffer)
        {
            renderDepthBuffer              = jsonToInt(jRenderDepthBuffer);
        }
        if(jRenderFps)
        {
            renderFps                      = jsonToInt(jRenderFps);
        }
        if(jRenderEventTimer)
        {
            renderEventTimer               = jsonToInt(jRenderEventTimer);
        }
        if(jRenderThreadCount)
        {
            renderThreadCount              = jsonToInt(jRenderThreadCount);
        }
    }
    cJSON* priority = cJSON_GetObjectItem(json, "priority");
    if(priority)
    {
        cJSON* jCapture = cJSON_GetObjectItem(priority, "capture");
        cJSON* jUpdate  = cJSON_GetObjectItem(priority, "update");
        cJSON* jRender  = cJSON_GetObjectItem(priority, "render");
        cJSON* jMain    = cJSON_GetObjectItem(priority, "main");
        if(jCapture)
        {
            priorityCapture = jsonToInt(jCapture);
        }
        if(jUpdate)
        {
            priorityUpdate  = jsonToInt(jUpdate);
        }
        if(jRender)
        {
            priorityRender  = jsonToInt(jRender);
        }
        if(jMain)
        {
            priorityMain    = jsonToInt(jMain);
        }
    }
    cJSON* delay = cJSON_GetObjectItem(json, "delay");
    if(delay)
    {
        cJSON* jCapture = cJSON_GetObjectItem(delay, "capture");
        cJSON* jUpdate = cJSON_GetObjectItem(delay, "update");
        cJSON* jRender = cJSON_GetObjectItem(delay, "render");
        cJSON* jMain = cJSON_GetObjectItem(delay, "main");
        if(jCapture)
        {
            delayCapture = jsonToInt(jCapture);
        }
        if(jUpdate)
        {
            delayUpdate = jsonToInt(jUpdate);
        }
        if(jRender)
        {
            delayRender = jsonToInt(jRender);
        }
        if(jMain)
        {
            delayMain = jsonToInt(jMain);
        }
    }
    cJSON* speed = cJSON_GetObjectItem(json, "speed");
    if(speed)
    {
        cJSON* low    = cJSON_GetObjectItem(speed, "low");
        cJSON* medium = cJSON_GetObjectItem(speed, "medium");
        cJSON* high   = cJSON_GetObjectItem(speed, "high");
        if(low)
        {
            speedLow    = jsonToInt(low);
        }
        if(medium)
        {
            speedMedium = jsonToInt(medium);
        }
        if(high)
        {
            speedHigh   = jsonToInt(high);
        }
    }
    cJSON* jmemory = cJSON_GetObjectItem(json, "memory");
    if(jmemory)
    {
        cJSON* jHistory = cJSON_GetObjectItem(jmemory, "history");
        cJSON* jFrame   = cJSON_GetObjectItem(jmemory, "frame");
        cJSON* jRld     = cJSON_GetObjectItem(jmemory, "rld");
        if(jHistory)
        {
            memoryHistory = jsonToInt(jHistory);
        }
        if(jFrame)
        {
            memoryFrame   = jsonToInt(jFrame);
        }
        if(jRld)
        {
            memoryRld     = jsonToInt(jRld);
        }
    }
    cJSON* history = cJSON_GetObjectItem(json, "history");
    if(history)
    {
        cJSON* frameCount = cJSON_GetObjectItem(history, "frameCount");
        cJSON* frameDisplay = cJSON_GetObjectItem(history, "frameDisplay");
        cJSON* frameLoadSave = cJSON_GetObjectItem(history, "frameLoadSave");
        cJSON* frameClipboard = cJSON_GetObjectItem(history, "frameClipboard");
        if(frameCount)
        {
            historyFrameCount = jsonToInt(frameCount);
        }
        if(frameDisplay)
        {
            historyFrameDisplay = jsonToInt(frameDisplay);
        }
        if(frameLoadSave)
        {
            historyFrameLoadSave = jsonToInt(frameLoadSave);
        }
        if(frameClipboard)
        {
            historyFrameClipboard = jsonToInt(frameClipboard);
        }
    }
    cJSON* window = cJSON_GetObjectItem(json, "window");
    if(window)
    {
        cJSON* debugWindow = cJSON_GetObjectItem(window, "debug");
        cJSON* dw = cJSON_GetObjectItem(window, "displayWidth");
        cJSON* dh = cJSON_GetObjectItem(window, "displayHeight");
        cJSON* cw = cJSON_GetObjectItem(window, "controlWidth");
        cJSON* ch = cJSON_GetObjectItem(window, "controlHeight");
        if(debugWindow)
        {
            windowDebug = jsonToInt(debugWindow);
        }
        if(dw)
        {
            windowDisplayWidth  = jsonToInt(dw);
        }
        if(dh)
        {
            windowDisplayHeight = jsonToInt(dh);
        }
        if(cw)
        {
            windowControlWidth  = jsonToInt(cw);
        }
        if(ch)
        {
            windowControlHeight = jsonToInt(ch);
        }
    }
    // delete
    cJSON_Delete(json);
}

void OscSettings::save()
{
    // create
    cJSON* jsonRoot = cJSON_CreateObject();
    // set
    cJSON* jsonRender = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonRoot, "render", jsonRender);
    cJSON_AddItemToObject(jsonRender, "shaders21", cJSON_CreateNumber(this->renderShaders21));
    cJSON_AddItemToObject(jsonRender, "vertexBufferSizeMegaByte", cJSON_CreateNumber(this->renderVertexBufferSizeMegaByte));
    cJSON_AddItemToObject(jsonRender, "depthBuffer", cJSON_CreateNumber(this->renderDepthBuffer));
    cJSON_AddItemToObject(jsonRender, "fps", cJSON_CreateNumber(this->renderFps));
    cJSON_AddItemToObject(jsonRender, "eventTimer", cJSON_CreateNumber(this->renderEventTimer));
    cJSON_AddItemToObject(jsonRender, "threadCount", cJSON_CreateNumber(this->renderThreadCount));
    cJSON* jsonPriority = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonRoot, "priority", jsonPriority);
    cJSON_AddItemToObject(jsonPriority, "capture", cJSON_CreateNumber(this->priorityCapture));
    cJSON_AddItemToObject(jsonPriority, "update", cJSON_CreateNumber(this->priorityUpdate));
    cJSON_AddItemToObject(jsonPriority, "render", cJSON_CreateNumber(this->priorityRender));
    cJSON_AddItemToObject(jsonPriority, "main", cJSON_CreateNumber(this->priorityMain));
    cJSON* jsonDelay = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonRoot, "delay", jsonDelay);
    cJSON_AddItemToObject(jsonDelay, "capture", cJSON_CreateNumber(this->delayCapture));
    cJSON_AddItemToObject(jsonDelay, "update", cJSON_CreateNumber(this->delayUpdate));
    cJSON_AddItemToObject(jsonDelay, "render", cJSON_CreateNumber(this->delayRender));
    cJSON_AddItemToObject(jsonDelay, "main", cJSON_CreateNumber(this->delayMain));
    cJSON* jsonSpeed = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonRoot, "speed", jsonSpeed);
    cJSON_AddItemToObject(jsonSpeed, "low",    cJSON_CreateNumber(this->speedLow));
    cJSON_AddItemToObject(jsonSpeed, "medium", cJSON_CreateNumber(this->speedMedium));
    cJSON_AddItemToObject(jsonSpeed, "high",   cJSON_CreateNumber(this->speedHigh));
    cJSON* jmemory = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonRoot, "memory", jmemory);
    cJSON_AddItemToObject(jsonSpeed, "history", cJSON_CreateNumber(this->memoryHistory));
    cJSON_AddItemToObject(jsonSpeed, "frame", cJSON_CreateNumber(this->memoryFrame));
    cJSON_AddItemToObject(jsonSpeed, "rld", cJSON_CreateNumber(this->memoryRld));
    cJSON* jsonHistory = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonRoot, "history", jsonHistory);
    cJSON_AddItemToObject(jsonHistory, "frameCount",     cJSON_CreateNumber(this->historyFrameCount));
    cJSON_AddItemToObject(jsonHistory, "frameDisplay",   cJSON_CreateNumber(this->historyFrameDisplay));
    cJSON_AddItemToObject(jsonHistory, "frameLoadSave",  cJSON_CreateNumber(this->historyFrameLoadSave));
    cJSON_AddItemToObject(jsonHistory, "frameClipboard", cJSON_CreateNumber(this->historyFrameClipboard));
    cJSON* jsonWindow = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonRoot, "window", jsonWindow);
    cJSON_AddItemToObject(jsonWindow, "debug", cJSON_CreateNumber(this->windowDebug));
    cJSON_AddItemToObject(jsonWindow, "displayWidth", cJSON_CreateNumber(this->windowDisplayWidth));
    cJSON_AddItemToObject(jsonWindow, "displayHeight", cJSON_CreateNumber(this->windowDisplayHeight));
    cJSON_AddItemToObject(jsonWindow, "controlWidth", cJSON_CreateNumber(this->windowControlWidth));
    cJSON_AddItemToObject(jsonWindow, "controlHeight", cJSON_CreateNumber(this->windowControlHeight));
    // save
    char* jsonString = cJSON_Print(jsonRoot);
    if(!jsonString)
    {
        CORE_MESSAGE("settings.json error before: [%.256s]\n", cJSON_GetErrorPtr());
    }
    FORMAT_BUFFER();
    FORMAT_PATH("data/startup/settings.json");
    fileSaveString(formatBuffer, jsonString);
    cJSON_Delete(jsonRoot);
    pMemory->free(jsonString);
}



////////////////////////////////////////////////////////////////////////////////
//
// OscColors
//
////////////////////////////////////////////////////////////////////////////////
OscColors::OscColors()
{
    SDL_memset(this, 0, sizeof(OscSettings));
}

uint OscColors::JsonToColorABGR(cJSON* jArray)
{
    uint ret = 0xffffffff;
    if(jArray)
    {
        int size = cJSON_GetArraySize(jArray);
        if(size == 4)
        {
            int r = jsonToInt(cJSON_GetArrayItem(jArray, 0));
            int g = jsonToInt(cJSON_GetArrayItem(jArray, 1));
            int b = jsonToInt(cJSON_GetArrayItem(jArray, 2));
            int a = jsonToInt(cJSON_GetArrayItem(jArray, 3));
            ret = COLOR_ARGB(a, b, g, r);
        }
    }
    return ret;
}
uint OscColors::JsonToColorARGB(cJSON* jArray)
{
    uint ret = 0xffffffff;
    if(jArray)
    {
        int size = cJSON_GetArraySize(jArray);
        if(size == 4)
        {
            int r = jsonToInt(cJSON_GetArrayItem(jArray, 0));
            int g = jsonToInt(cJSON_GetArrayItem(jArray, 1));
            int b = jsonToInt(cJSON_GetArrayItem(jArray, 2));
            int a = jsonToInt(cJSON_GetArrayItem(jArray, 3));
            ret = COLOR_ARGB(a, r, g, b);
        }
    }
    return ret;
}


cJSON* OscColors::ColorABGRToJson(uint color)
{
    uint colors[4] = { 0 };
    colors[0] = COLOR_B(color);
    colors[1] = COLOR_G(color);
    colors[2] = COLOR_R(color);
    colors[3] = COLOR_A(color);
    cJSON* jArray = cJSON_CreateIntArray((const int*)colors, 4);
    return jArray;
}

cJSON* OscColors::ColorARGBToJson(uint color)
{
    uint colors[4] = { 0 };
    colors[0] = COLOR_R(color);
    colors[1] = COLOR_G(color);
    colors[2] = COLOR_B(color);
    colors[3] = COLOR_A(color);
    cJSON* jArray = cJSON_CreateIntArray((const int*)colors, 4);
    return jArray;
}


void OscColors::load()
{
    // load
    FORMAT_BUFFER();
    FORMAT_PATH("data/startup/colors.json");
    char*  memory = 0;
    ilarge memorySize = 0;
    fileLoadString(formatBuffer, &memory, &memorySize);
    json = cJSON_Parse((const char*)memory);
    if(!json)
    {
        CORE_MESSAGE("colors.json error before: [%.256s]\n", cJSON_GetErrorPtr());
    }
    fileFree(memory);
    // window
    cJSON* window = cJSON_GetObjectItem(json, "window");
    if(window)
    {
        cJSON* jDefault = cJSON_GetObjectItem(window, "default");
        cJSON* jFront   = cJSON_GetObjectItem(window, "front");
        cJSON* jBack    = cJSON_GetObjectItem(window, "back");
        windowDefault = jsonToInt(jDefault);
        windowFront   = JsonToColorABGR(jFront);
        windowBack    = JsonToColorABGR(jBack);
    }
    // render
    cJSON* render = cJSON_GetObjectItem(json, "render");
    if(render)
    {
        cJSON* jBackground = cJSON_GetObjectItem(render, "background");
        cJSON* jTime       = cJSON_GetObjectItem(render, "time");
        cJSON* jChannel0   = cJSON_GetObjectItem(render, "channel0");
        cJSON* jChannel1   = cJSON_GetObjectItem(render, "channel1");
        cJSON* jFunction   = cJSON_GetObjectItem(render, "function");
        cJSON* jxyGraph    = cJSON_GetObjectItem(render, "xyGraph");
        cJSON* jGrid       = cJSON_GetObjectItem(render, "grid");
        cJSON* jBorder     = cJSON_GetObjectItem(render, "border");
        cJSON* jTrigger    = cJSON_GetObjectItem(render, "trigger");
        cJSON* jDigital    = cJSON_GetObjectItem(render, "digital");
        renderBackground = JsonToColorARGB(jBackground);
        renderTime       = JsonToColorARGB(jTime);
        renderChannel0   = JsonToColorARGB(jChannel0);
        renderChannel1   = JsonToColorARGB(jChannel1);
        renderFunction   = JsonToColorARGB(jFunction);
        renderXyGraph    = JsonToColorARGB(jxyGraph);
        renderGrid       = JsonToColorARGB(jGrid);
        renderBorder     = JsonToColorARGB(jBorder);
        renderTrigger    = JsonToColorARGB(jTrigger);
        renderDigital    = JsonToColorARGB(jDigital);
    }
    // delete
    cJSON_Delete(json);
}

void OscColors::save()
{
    // create
    cJSON* jsonRoot = cJSON_CreateObject();
    cJSON* jsonWindow = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonRoot, "window", jsonWindow);
    cJSON_AddItemToObject(jsonWindow, "front",  ColorABGRToJson(windowFront));
    cJSON_AddItemToObject(jsonWindow, "back",   ColorABGRToJson(windowBack));
    cJSON* jsonRender = cJSON_CreateObject();
    cJSON_AddItemToObject(jsonRoot, "render", jsonRender);
    cJSON_AddItemToObject(jsonRender, "backgrond",  ColorARGBToJson(renderBackground));
    cJSON_AddItemToObject(jsonRender, "channel0",   ColorARGBToJson(renderChannel0));
    cJSON_AddItemToObject(jsonRender, "channel1",   ColorARGBToJson(renderChannel1));
    cJSON_AddItemToObject(jsonRender, "function",   ColorARGBToJson(renderFunction));
    cJSON_AddItemToObject(jsonRender, "xyGraph",    ColorARGBToJson(renderXyGraph));
    cJSON_AddItemToObject(jsonRender, "grid",       ColorARGBToJson(renderGrid));
    cJSON_AddItemToObject(jsonRender, "border",     ColorARGBToJson(renderBorder));
    cJSON_AddItemToObject(jsonRender, "trigger",    ColorARGBToJson(renderTrigger));
    cJSON_AddItemToObject(jsonRender, "digital",    ColorARGBToJson(renderDigital));
    // save
    char* jsonString = cJSON_Print(jsonRoot);
    if(!jsonString)
    {
        CORE_MESSAGE("colors.json error before: [%.256s]\n", cJSON_GetErrorPtr());
    }
    FORMAT_BUFFER();
    FORMAT_PATH("data/startup/colors.json");
    fileSaveString(formatBuffer, jsonString);
    cJSON_Delete(jsonRoot);
    pMemory->free(jsonString);
}

////////////////////////////////////////////////////////////////////////////////
//
// OscSettingsInterface
//
////////////////////////////////////////////////////////////////////////////////
OscSettingsInterface::OscSettingsInterface() : hardware1(1), hardware2(2)
{
    pHardware = &hardware2;
}

void OscSettingsInterface::load()
{
    settings.load();
    colors.load();
    hardware1.load();
    hardware2.load();
}
void OscSettingsInterface::save()
{
    settings.save();
    colors.save();
    hardware1.save();
    hardware2.save();
}

void OscSettingsInterface::setVersion(int version)
{
    if(version == HARDWARE_VERSION_1)
    {
        pHardware = &hardware1;
    }
    if(version == HARDWARE_VERSION_2)
    {
        pHardware = &hardware2;
    }
}

OscHardware* OscSettingsInterface::getHardware()
{
    return pHardware;
}

OscSettings* OscSettingsInterface::getSettings()
{
    return &settings;
}

OscColors* OscSettingsInterface::getColors()
{
    return &colors;
}


////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
