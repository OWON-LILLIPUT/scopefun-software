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

////////////////////////////////////////////////////////////////////////////////
//
// SoftwareGeneratorChannel
//
////////////////////////////////////////////////////////////////////////////////
SoftwareGeneratorChannel::SoftwareGeneratorChannel()
{
    Default();
}

void SoftwareGeneratorChannel::Default()
{
    type = GENERATE_SINUS;
    period = 50.f * MICRO;
    peakToPeak = 17.f;
    speed = 0.01f;
    every = 0.0f;
    onOff = 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// WndSoftwareGenerator
//
////////////////////////////////////////////////////////////////////////////////
WndSoftwareGenerator::WndSoftwareGenerator()
{
}

void WndSoftwareGenerator::Default()
{
    channel[0].Default();
    channel[1].Default();
    channel[1].peakToPeak = 12.f;
    channel[1].speed      = 0.05f;
    channel[1].peakToPeak = 12.f;
    channel[1].period     = 42.f * MICRO;
}


////////////////////////////////////////////////////////////////////////////////
//
// WndHardwareGenerator
//
////////////////////////////////////////////////////////////////////////////////

WndHardwareGenerator::WndHardwareGenerator()
{
    Default();
}

void WndHardwareGenerator::loadCustomData(int idx, const char* path)
{
    char*  memory = 0;
    ilarge memorySize = 0;
    int ret = fileLoadString(path, &memory, &memorySize);
    if(ret)
    {
        return;
    }
    cJSON* root = cJSON_Parse(memory);
    cJSON* arrayValues = cJSON_GetObjectItem(root, "signal");
    cJSON* val = cJSON_GetArrayItem(arrayValues, 0);
    for(uint i = 0; i < GENERATOR_ANALOG_SAMPLES; i++)
    {
        if(!arrayValues)
        {
            continue;
        }
        if(!val)
        {
            continue;
        }
        if(idx == 0)
        {
            custom.analog0.bytes[i] = endianSwap16((ushort&)val->valueint);
        }
        if(idx == 1)
        {
            custom.analog1.bytes[i] = endianSwap16((ushort&)val->valueint);
        }
        if(val) val = val->next;
    }
    cJSON_Delete(root);
    fileFree(memory);
}

void WndHardwareGenerator::loadCustomDigital(const char* path)
{
    char*  memory = 0;
    ilarge memorySize = 0;
    int ret = fileLoadString(path, &memory, &memorySize);
    if(ret)
    {
        return;
    }
    cJSON* root = cJSON_Parse(memory);
    if(!root)
    {
        const char* error = cJSON_GetErrorPtr();
        return;
    }
    FORMAT_BUFFER();
    FORMAT("digitalChannel%d", 0);
    cJSON* arrayValues0 = cJSON_GetObjectItem(root, formatBuffer);
    FORMAT("digitalChannel%d", 1);
    cJSON* arrayValues1 = cJSON_GetObjectItem(root, formatBuffer);
    FORMAT("digitalChannel%d", 2);
    cJSON* arrayValues2 = cJSON_GetObjectItem(root, formatBuffer);
    FORMAT("digitalChannel%d", 3);
    cJSON* arrayValues3 = cJSON_GetObjectItem(root, formatBuffer);
    FORMAT("digitalChannel%d", 4);
    cJSON* arrayValues4 = cJSON_GetObjectItem(root, formatBuffer);
    FORMAT("digitalChannel%d", 5);
    cJSON* arrayValues5 = cJSON_GetObjectItem(root, formatBuffer);
    FORMAT("digitalChannel%d", 6);
    cJSON* arrayValues6 = cJSON_GetObjectItem(root, formatBuffer);
    FORMAT("digitalChannel%d", 7);
    cJSON* arrayValues7 = cJSON_GetObjectItem(root, formatBuffer);
    FORMAT("digitalChannel%d", 8);
    cJSON* arrayValues8 = cJSON_GetObjectItem(root, formatBuffer);
    FORMAT("digitalChannel%d", 9);
    cJSON* arrayValues9 = cJSON_GetObjectItem(root, formatBuffer);
    FORMAT("digitalChannel%d", 10);
    cJSON* arrayValues10 = cJSON_GetObjectItem(root, formatBuffer);
    FORMAT("digitalChannel%d", 11);
    cJSON* arrayValues11 = cJSON_GetObjectItem(root, formatBuffer);
    FORMAT("digitalChannel%d", 12);
    cJSON* arrayValues12 = cJSON_GetObjectItem(root, formatBuffer);
    FORMAT("digitalChannel%d", 13);
    cJSON* arrayValues13 = cJSON_GetObjectItem(root, formatBuffer);
    FORMAT("digitalChannel%d", 14);
    cJSON* arrayValues14 = cJSON_GetObjectItem(root, formatBuffer);
    FORMAT("digitalChannel%d", 15);
    cJSON* arrayValues15 = cJSON_GetObjectItem(root, formatBuffer);

    cJSON* val0 = 0;
    cJSON* val1 = 0;
    cJSON* val2 = 0;
    cJSON* val3 = 0;
    cJSON* val4 = 0;
    cJSON* val5 = 0;
    cJSON* val6 = 0;
    cJSON* val7 = 0;
    cJSON* val8 = 0;
    cJSON* val9 = 0;
    cJSON* val10 = 0;
    cJSON* val11 = 0;
    cJSON* val12 = 0;
    cJSON* val13 = 0;
    cJSON* val14 = 0;
    cJSON* val15 = 0;
    if(arrayValues0)
    {
        val0 = cJSON_GetArrayItem(arrayValues0, 0);
    }
    if(arrayValues1)
    {
        val1 = cJSON_GetArrayItem(arrayValues1, 0);
    }
    if(arrayValues2)
    {
        val2 = cJSON_GetArrayItem(arrayValues2, 0);
    }
    if(arrayValues3)
    {
        val3 = cJSON_GetArrayItem(arrayValues3, 0);
    }
    if(arrayValues4)
    {
        val4 = cJSON_GetArrayItem(arrayValues4, 0);
    }
    if(arrayValues5)
    {
        val5 = cJSON_GetArrayItem(arrayValues5, 0);
    }
    if(arrayValues6)
    {
        val6 = cJSON_GetArrayItem(arrayValues6, 0);
    }
    if(arrayValues7)
    {
        val7 = cJSON_GetArrayItem(arrayValues7, 0);
    }
    if(arrayValues8)
    {
        val8 = cJSON_GetArrayItem(arrayValues8, 0);
    }
    if(arrayValues9)
    {
        val9 = cJSON_GetArrayItem(arrayValues9, 0);
    }
    if(arrayValues10)
    {
        val10 = cJSON_GetArrayItem(arrayValues10, 0);
    }
    if(arrayValues11)
    {
        val11 = cJSON_GetArrayItem(arrayValues11, 0);
    }
    if(arrayValues12)
    {
        val12 = cJSON_GetArrayItem(arrayValues12, 0);
    }
    if(arrayValues13)
    {
        val13 = cJSON_GetArrayItem(arrayValues13, 0);
    }
    if(arrayValues14)
    {
        val14 = cJSON_GetArrayItem(arrayValues14, 0);
    }
    if(arrayValues15)
    {
        val15 = cJSON_GetArrayItem(arrayValues15, 0);
    }
    for(uint i = 0; i < GENERATOR_DIGITAL_SAMPLES; i++)
    {
        ushort bits = 0;
        if(val0)
        {
            bits = bits | (val0->valueint << 0);
        }
        if(val1)
        {
            bits = bits | (val1->valueint << 1);
        }
        if(val2)
        {
            bits = bits | (val2->valueint << 2);
        }
        if(val3)
        {
            bits = bits | (val3->valueint << 3);
        }
        if(val4)
        {
            bits = bits | (val4->valueint << 4);
        }
        if(val5)
        {
            bits = bits | (val5->valueint << 5);
        }
        if(val6)
        {
            bits = bits | (val6->valueint << 6);
        }
        if(val7)
        {
            bits = bits | (val7->valueint << 7);
        }
        if(val8)
        {
            bits = bits | (val8->valueint << 8);
        }
        if(val9)
        {
            bits = bits | (val9->valueint << 9);
        }
        if(val10)
        {
            bits = bits | (val10->valueint << 10);
        }
        if(val11)
        {
            bits = bits | (val11->valueint << 11);
        }
        if(val12)
        {
            bits = bits | (val12->valueint << 12);
        }
        if(val13)
        {
            bits = bits | (val13->valueint << 13);
        }
        if(val14)
        {
            bits = bits | (val14->valueint << 14);
        }
        if(val15)
        {
            bits = bits | (val15->valueint << 15);
        }
        custom.digital.bytes[i] = endianSwap16(bits);

        if(val0) val0  = val0->next;
        if(val1) val1  = val1->next;
        if(val2) val2  = val2->next;
        if(val3) val3  = val3->next;
        if(val4) val4  = val4->next;
        if(val5) val5  = val5->next;
        if(val6) val6  = val6->next;
        if(val7) val7  = val7->next;
        if(val8) val8  = val8->next;
        if(val9) val9  = val9->next;
        if(val10) val10 = val10->next;
        if(val11) val11 = val11->next;
        if(val12) val12 = val12->next;
        if(val13) val13 = val13->next;
        if(val14) val14 = val14->next;
        if(val15) val15 = val15->next;
    }
    cJSON_Delete(root);
    fileFree(memory);
}

void WndHardwareGenerator::upload()
{
    pOsciloscope->thread.setGeneratorData(&custom);
    pOsciloscope->thread.function(afUploadGenerator);
}

void WndHardwareGenerator::uploadDigital()
{
   pOsciloscope->thread.setGeneratorData(&custom);
   pOsciloscope->thread.function(afUploadGenerator);
}

void WndHardwareGenerator::Default()
{
    // generator 0
    type0       = GENERATOR_SIN;
    frequency0  = 1000.f;
    voltage0    = 1024;
    offset0     = 0;
    squareDuty0 = 50;
    sawSlopePositive0 = 1;
    onOff0 = 0;
    // generator 1
    type1       = GENERATOR_SIN;
    frequency1  = 1000.f;
    voltage1    = 1024;
    offset1     = 0;
    squareDuty1 = 50;
    sawSlopePositive1 = 1;
    onOff1 = 0;
    // customData
    memset(&custom, 0, sizeof(custom));
}


////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
