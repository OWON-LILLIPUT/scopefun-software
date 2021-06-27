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
#include "OsciloskopHardwareGenerator.h"

OsciloskopHardwareGenerator::OsciloskopHardwareGenerator(wxWindow* parent)
    :
    HardwareGenerator(parent)
{
}

void OsciloskopHardwareGenerator::HardwareGeneratorOnInitDialog(wxInitDialogEvent& event)
{
    // TODO: Implement HardwareGeneratorOnInitDialog
    FORMAT_BUFFER();
    // generator 0
    FORMAT("%f", pOsciloscope->window.hardwareGenerator.frequency0);
    m_comboBoxType->SetSelection(pOsciloscope->window.hardwareGenerator.type0);
    m_textCtrlFrequency->SetValue(wxString::FromAscii(formatBuffer));
    FORMAT("%f", double(pOsciloscope->window.hardwareGenerator.voltage0)*pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep);
    m_textCtrlVoltage0->SetValue(wxString::FromAscii(formatBuffer));
    m_sliderVoltage->SetValue(pOsciloscope->window.hardwareGenerator.voltage0);
    FORMAT("%f", double(pOsciloscope->window.hardwareGenerator.offset0)*pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep);
    m_textCtrlOffset0->SetValue(wxString::FromAscii(formatBuffer));
    m_sliderOffset->SetValue(pOsciloscope->window.hardwareGenerator.offset0);
    FORMAT("%d", pOsciloscope->window.hardwareGenerator.squareDuty0);
    m_textCtrlSquareDuty0->SetValue(wxString::FromAscii(formatBuffer));
    m_sliderSquareDuty->SetValue(pOsciloscope->window.hardwareGenerator.squareDuty0);
    m_checkBoxSawSlope->SetValue(pOsciloscope->window.hardwareGenerator.sawSlopePositive0);
    // generator 1
    FORMAT("%f", pOsciloscope->window.hardwareGenerator.frequency1);
    m_comboBoxType1->SetSelection(pOsciloscope->window.hardwareGenerator.type1);
    m_textCtrlFrequency1->SetValue(wxString::FromAscii(formatBuffer));
    FORMAT("%f", double(pOsciloscope->window.hardwareGenerator.voltage1)*pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep);
    m_textCtrlVoltage1->SetValue(wxString::FromAscii(formatBuffer));
    m_sliderVoltage1->SetValue(pOsciloscope->window.hardwareGenerator.voltage1);
    FORMAT("%f", double(pOsciloscope->window.hardwareGenerator.offset1)*pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep);
    m_textCtrlOffset1->SetValue(wxString::FromAscii(formatBuffer));
    m_sliderOffset1->SetValue(pOsciloscope->window.hardwareGenerator.offset1);
    FORMAT("%d", pOsciloscope->window.hardwareGenerator.squareDuty1);
    m_textCtrlSquareDuty1->SetValue(wxString::FromAscii(formatBuffer));
    m_sliderSquareDuty1->SetValue(pOsciloscope->window.hardwareGenerator.squareDuty1);
    m_checkBoxSawSlope1->SetValue(pOsciloscope->window.hardwareGenerator.sawSlopePositive1);
    if(!pOsciloscope->settings.getColors()->windowDefault)
    {
        if(pOsciloscope->window.hardwareGenerator.onOff0)
        {
            m_buttonOn0->SetBackgroundColour(pOsciloscope->settings.getColors()->windowFront);
            m_buttonOn0->SetForegroundColour(pOsciloscope->settings.getColors()->windowBack);
            m_buttonOff0->SetBackgroundColour(pOsciloscope->settings.getColors()->windowBack);
            m_buttonOff0->SetForegroundColour(pOsciloscope->settings.getColors()->windowFront);
        }
        else
        {
            m_buttonOn0->SetBackgroundColour(pOsciloscope->settings.getColors()->windowBack);
            m_buttonOn0->SetForegroundColour(pOsciloscope->settings.getColors()->windowFront);
            m_buttonOff0->SetBackgroundColour(pOsciloscope->settings.getColors()->windowFront);
            m_buttonOff0->SetForegroundColour(pOsciloscope->settings.getColors()->windowBack);
        }
        if(pOsciloscope->window.hardwareGenerator.onOff1)
        {
            m_buttonOn1->SetBackgroundColour(pOsciloscope->settings.getColors()->windowFront);
            m_buttonOn1->SetForegroundColour(pOsciloscope->settings.getColors()->windowBack);
            m_buttonOff1->SetBackgroundColour(pOsciloscope->settings.getColors()->windowBack);
            m_buttonOff1->SetForegroundColour(pOsciloscope->settings.getColors()->windowFront);
        }
        else
        {
            m_buttonOn1->SetBackgroundColour(pOsciloscope->settings.getColors()->windowBack);
            m_buttonOn1->SetForegroundColour(pOsciloscope->settings.getColors()->windowFront);
            m_buttonOff1->SetBackgroundColour(pOsciloscope->settings.getColors()->windowFront);
            m_buttonOff1->SetForegroundColour(pOsciloscope->settings.getColors()->windowBack);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////
//
// generator 0
//
///////////////////////////////////////////////////////////////////////////////////////

void OsciloskopHardwareGenerator::m_comboBoxTypeOnCombobox(wxCommandEvent& event)
{
    // TODO: Implement m_comboBoxTypeOnCombobox
    int type =  m_comboBoxType->GetSelection();
    pOsciloscope->window.hardwareGenerator.type0 = type;
    pOsciloscope->control.setGeneratorType0((GeneratorType)(type));
    pOsciloscope->control.transferData();
    if(type == GENERATOR_CUSTOM)
    {
        pOsciloscope->window.hardwareGenerator.upload();
    }
}

void OsciloskopHardwareGenerator::m_textCtrlFrequencyOnTextEnter(wxCommandEvent& event)
{
    // TODO: Implement m_textCtrlFrequencyOnTextEnter
    float freq = atof(m_textCtrlFrequency->GetValue().ToAscii().data());
    pOsciloscope->window.hardwareGenerator.frequency0 = freq;
    pOsciloscope->control.setGeneratorFrequency0(freq, pOsciloscope->settings.getHardware()->generatorFs);
    pOsciloscope->control.transferData();
}

void OsciloskopHardwareGenerator::m_textCtrlVoltage0OnTextEnter(wxCommandEvent& event)
{
    // TODO: Implement m_textCtrlVoltage0OnTextEnter
    pOsciloscope->window.hardwareGenerator.voltage0 = pFormat->stringToFloat(m_textCtrlVoltage0->GetValue().ToAscii().data()) / pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep;
    pOsciloscope->control.setGeneratorVoltage0(pOsciloscope->window.hardwareGenerator.voltage0);
    pOsciloscope->control.transferData();
    m_sliderVoltage->SetValue(pOsciloscope->window.hardwareGenerator.voltage0);
}

void OsciloskopHardwareGenerator::m_sliderVoltageOnScroll(wxScrollEvent& event)
{
    // TODO: Implement m_sliderVoltageOnScroll
    float voltage = double(m_sliderVoltage->GetValue()) * pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep;
    pOsciloscope->window.hardwareGenerator.voltage0 = m_sliderVoltage->GetValue();
    pOsciloscope->control.setGeneratorVoltage0(m_sliderVoltage->GetValue());
    pOsciloscope->control.transferData();
    m_textCtrlVoltage0->SetValue(pFormat->floatToString(voltage));
}

void OsciloskopHardwareGenerator::m_textCtrlOffset0OnTextEnter(wxCommandEvent& event)
{
    // TODO: Implement m_textCtrlOffset0OnTextEnter
    pOsciloscope->window.hardwareGenerator.offset0 = pFormat->stringToFloat(m_textCtrlOffset0->GetValue().ToAscii().data()) / pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep;
    pOsciloscope->control.setGeneratorOffset0(pOsciloscope->window.hardwareGenerator.offset0 + pOsciloscope->settings.getHardware()->getGeneratorOffset(pOsciloscope->window.horizontal.Capture, 0));
    pOsciloscope->control.transferData();
    m_sliderOffset->SetValue(pOsciloscope->window.hardwareGenerator.offset0);
}

void OsciloskopHardwareGenerator::m_sliderOffsetOnScroll(wxScrollEvent& event)
{
    // TODO: Implement m_sliderOffsetOnScroll
    float voltage = double(m_sliderOffset->GetValue()) * pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep;
    pOsciloscope->window.hardwareGenerator.offset0 = m_sliderOffset->GetValue();
    pOsciloscope->control.setGeneratorOffset0(m_sliderOffset->GetValue() + pOsciloscope->settings.getHardware()->getGeneratorOffset(pOsciloscope->window.horizontal.Capture,0));
    pOsciloscope->control.transferData();
    m_textCtrlOffset0->SetValue(pFormat->floatToString(voltage));
}

void OsciloskopHardwareGenerator::m_textCtrlSquareDuty0OnTextEnter(wxCommandEvent& event)
{
    // TODO: Implement m_textCtrlSquareDuty0OnTextEnter
    pOsciloscope->window.hardwareGenerator.squareDuty0 = pFormat->stringToFloat(m_textCtrlSquareDuty0->GetValue().ToAscii().data());
    pOsciloscope->control.setGeneratorSquareDuty0(pOsciloscope->window.hardwareGenerator.squareDuty0);
    pOsciloscope->control.transferData();
    m_sliderSquareDuty->SetValue(pOsciloscope->window.hardwareGenerator.squareDuty0);
}

void OsciloskopHardwareGenerator::m_sliderSquareDutyOnScroll(wxScrollEvent& event)
{
    // TODO: Implement m_sliderSquareDutyOnScroll
    pOsciloscope->window.hardwareGenerator.squareDuty0 = m_sliderSquareDuty->GetValue();
    pOsciloscope->control.setGeneratorSquareDuty0(m_sliderSquareDuty->GetValue());
    pOsciloscope->control.transferData();
    m_textCtrlSquareDuty0->SetValue(pFormat->floatToString(pOsciloscope->window.hardwareGenerator.squareDuty0));
}

void OsciloskopHardwareGenerator::m_checkBoxSawSlopeOnCheckBox(wxCommandEvent& event)
{
    // TODO: Implement m_checkBoxSawSlopeOnCheckBox
    pOsciloscope->window.hardwareGenerator.sawSlopePositive0 = m_checkBoxSawSlope->GetValue();
    pOsciloscope->control.setGeneratorSlope0(m_checkBoxSawSlope->GetValue());
    pOsciloscope->control.transferData();
}

void OsciloskopHardwareGenerator::m_filePicker1OnFileChanged(wxFileDirPickerEvent& event)
{
    // TODO: Implement m_filePicker1OnFileChanged
    pOsciloscope->window.hardwareGenerator.loadCustomData(0, m_filePicker1->GetFileName().GetFullPath().ToAscii().data());
}

void OsciloskopHardwareGenerator::m_buttonCustomFileOnButtonClick(wxCommandEvent& event)
{
    pOsciloscope->window.hardwareGenerator.loadCustomData(0, m_filePicker1->GetFileName().GetFullPath().ToAscii().data());
    pOsciloscope->window.hardwareGenerator.upload();
}

void OsciloskopHardwareGenerator::m_buttonOn0OnButtonClick(wxCommandEvent& event)
{
    pOsciloscope->window.hardwareGenerator.onOff0 = 1;
    pOsciloscope->control.setGeneratorOn0(1);
    pOsciloscope->control.transferData();
    if(!pOsciloscope->settings.getColors()->windowDefault)
    {
        m_buttonOn0->SetBackgroundColour(pOsciloscope->settings.getColors()->windowFront);
        m_buttonOn0->SetForegroundColour(pOsciloscope->settings.getColors()->windowBack);
        m_buttonOff0->SetBackgroundColour(pOsciloscope->settings.getColors()->windowBack);
        m_buttonOff0->SetForegroundColour(pOsciloscope->settings.getColors()->windowFront);
    }
}

void OsciloskopHardwareGenerator::m_buttonOff0OnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_radioBtnOffOnRadioButton
    pOsciloscope->window.hardwareGenerator.onOff0 = 0;
    pOsciloscope->control.setGeneratorOn0(0);
    pOsciloscope->control.transferData();
    if(!pOsciloscope->settings.getColors()->windowDefault)
    {
        m_buttonOn0->SetBackgroundColour(pOsciloscope->settings.getColors()->windowBack);
        m_buttonOn0->SetForegroundColour(pOsciloscope->settings.getColors()->windowFront);
        m_buttonOff0->SetBackgroundColour(pOsciloscope->settings.getColors()->windowFront);
        m_buttonOff0->SetForegroundColour(pOsciloscope->settings.getColors()->windowBack);
    }
}

///////////////////////////////////////////////////////////////
// ch0 - spin
///////////////////////////////////////////////////////////////

void OsciloskopHardwareGenerator::m_spinBtnCh0VoltOnSpinUp(wxSpinEvent& event)
{
    pOsciloscope->window.hardwareGenerator.voltage0++;
    pOsciloscope->window.hardwareGenerator.voltage0 = min(pOsciloscope->window.hardwareGenerator.voltage0, 2048);
    m_textCtrlVoltage0->SetValue(pFormat->floatToString(double(pOsciloscope->window.hardwareGenerator.voltage0)*pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep));
    m_sliderVoltage->SetValue(pOsciloscope->window.hardwareGenerator.voltage0);
    pOsciloscope->control.setGeneratorVoltage0(pOsciloscope->window.hardwareGenerator.voltage0);
    pOsciloscope->control.transferData();
}

void OsciloskopHardwareGenerator::m_spinBtnCh0VoltOnSpinDown(wxSpinEvent& event)
{
    pOsciloscope->window.hardwareGenerator.voltage0--;
    pOsciloscope->window.hardwareGenerator.voltage0 = max(pOsciloscope->window.hardwareGenerator.voltage0, 0);
    m_textCtrlVoltage0->SetValue(pFormat->floatToString(double(pOsciloscope->window.hardwareGenerator.voltage0)*pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep));
    m_sliderVoltage->SetValue(pOsciloscope->window.hardwareGenerator.voltage0);
    pOsciloscope->control.setGeneratorVoltage0(pOsciloscope->window.hardwareGenerator.voltage0);
    pOsciloscope->control.transferData();
}

void OsciloskopHardwareGenerator::m_spinBtnCh0OffsetOnSpinUp(wxSpinEvent& event)
{
    pOsciloscope->window.hardwareGenerator.offset0++;
    pOsciloscope->window.hardwareGenerator.offset0 = min(pOsciloscope->window.hardwareGenerator.offset0, 2048);
    m_textCtrlOffset0->SetValue(pFormat->floatToString(double(pOsciloscope->window.hardwareGenerator.offset0)*pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep));
    m_sliderOffset->SetValue(pOsciloscope->window.hardwareGenerator.offset0);
    pOsciloscope->control.setGeneratorOffset0(pOsciloscope->window.hardwareGenerator.offset0 + pOsciloscope->settings.getHardware()->getGeneratorOffset(pOsciloscope->window.horizontal.Capture,0));
    pOsciloscope->control.transferData();
}

void OsciloskopHardwareGenerator::m_spinBtnCh0OffsetOnSpinDown(wxSpinEvent& event)
{
    pOsciloscope->window.hardwareGenerator.offset0--;
    pOsciloscope->window.hardwareGenerator.offset0 = max(pOsciloscope->window.hardwareGenerator.offset0, -2048);
    m_textCtrlOffset0->SetValue(pFormat->floatToString(double(pOsciloscope->window.hardwareGenerator.offset0)*pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep));
    m_sliderOffset->SetValue(pOsciloscope->window.hardwareGenerator.offset0);
    pOsciloscope->control.setGeneratorOffset0(pOsciloscope->window.hardwareGenerator.offset0 + pOsciloscope->settings.getHardware()->getGeneratorOffset(pOsciloscope->window.horizontal.Capture,0));
    pOsciloscope->control.transferData();
}

void OsciloskopHardwareGenerator::m_spinBtnGen0SqrDutyOnSpinUp(wxSpinEvent& event)
{
    pOsciloscope->window.hardwareGenerator.squareDuty0++;
    pOsciloscope->control.setGeneratorSquareDuty0(pOsciloscope->window.hardwareGenerator.squareDuty0);
    pOsciloscope->control.transferData();
    m_sliderSquareDuty->SetValue(pOsciloscope->window.hardwareGenerator.squareDuty0);
    m_textCtrlSquareDuty0->SetValue(pFormat->floatToString(pOsciloscope->window.hardwareGenerator.squareDuty0));
}

void OsciloskopHardwareGenerator::m_spinBtnGen0SqrDutyOnSpinDown(wxSpinEvent& event)
{
    pOsciloscope->window.hardwareGenerator.squareDuty0--;
    pOsciloscope->control.setGeneratorSquareDuty0(pOsciloscope->window.hardwareGenerator.squareDuty0);
    pOsciloscope->control.transferData();
    m_sliderSquareDuty->SetValue(pOsciloscope->window.hardwareGenerator.squareDuty0);
    m_textCtrlSquareDuty0->SetValue(pFormat->floatToString(pOsciloscope->window.hardwareGenerator.squareDuty0));
}

///////////////////////////////////////////////////////////////////////////////////////
//
// generator 1
//
///////////////////////////////////////////////////////////////////////////////////////
void OsciloskopHardwareGenerator::m_comboBoxType1OnChoice(wxCommandEvent& event)
{
    // TODO: Implement m_comboBoxType1OnChoice
    int type =  m_comboBoxType1->GetSelection();
    pOsciloscope->window.hardwareGenerator.type1 = type;
    pOsciloscope->control.setGeneratorType1((GeneratorType)type);
    pOsciloscope->control.transferData();
    if(type == GENERATOR_CUSTOM)
    {
        pOsciloscope->window.hardwareGenerator.upload();
    }
}

void OsciloskopHardwareGenerator::m_textCtrlFrequency1OnTextEnter(wxCommandEvent& event)
{
    // TODO: Implement m_textCtrlFrequency1OnTextEnter
    float freq = atof(m_textCtrlFrequency1->GetValue().ToAscii().data());
    pOsciloscope->window.hardwareGenerator.frequency1 = freq;
    pOsciloscope->control.setGeneratorFrequency1(freq, pOsciloscope->settings.getHardware()->generatorFs);
    pOsciloscope->control.transferData();
}

void OsciloskopHardwareGenerator::m_textCtrlVoltage1OnTextEnter(wxCommandEvent& event)
{
    // TODO: Implement m_textCtrlVoltage1OnTextEnter
    pOsciloscope->window.hardwareGenerator.voltage1 = pFormat->stringToFloat(m_textCtrlVoltage1->GetValue().ToAscii().data()) / pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep;
    pOsciloscope->control.setGeneratorVoltage1(pOsciloscope->window.hardwareGenerator.voltage1);
    pOsciloscope->control.transferData();
    m_sliderVoltage1->SetValue(pOsciloscope->window.hardwareGenerator.voltage1);
}

void OsciloskopHardwareGenerator::m_sliderVoltage1OnScroll(wxScrollEvent& event)
{
    // TODO: Implement m_sliderVoltage1OnScroll
    float voltage = double(m_sliderVoltage1->GetValue()) * pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep;
    pOsciloscope->window.hardwareGenerator.voltage1 = m_sliderVoltage1->GetValue();
    pOsciloscope->control.setGeneratorVoltage1(m_sliderVoltage1->GetValue());
    pOsciloscope->control.transferData();
    m_textCtrlVoltage1->SetValue(pFormat->floatToString(voltage));
}

void OsciloskopHardwareGenerator::m_textCtrlOffset1OnTextEnter(wxCommandEvent& event)
{
    // TODO: Implement m_textCtrlOffset1OnTextEnter
    pOsciloscope->window.hardwareGenerator.offset1 = pFormat->stringToFloat(m_textCtrlOffset1->GetValue().ToAscii().data()) / pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep;
    pOsciloscope->control.setGeneratorOffset1(pOsciloscope->window.hardwareGenerator.offset1 + pOsciloscope->settings.getHardware()->getGeneratorOffset(pOsciloscope->window.horizontal.Capture,1));
    pOsciloscope->control.transferData();
    m_sliderOffset1->SetValue(pOsciloscope->window.hardwareGenerator.offset1);
}

void OsciloskopHardwareGenerator::m_sliderOffset1OnScroll(wxScrollEvent& event)
{
    // TODO: Implement m_sliderOffset1OnScroll
    float voltage = double(m_sliderOffset1->GetValue()) * pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep;
    pOsciloscope->window.hardwareGenerator.offset1 = m_sliderOffset1->GetValue();
    pOsciloscope->control.setGeneratorOffset1(m_sliderOffset1->GetValue() + pOsciloscope->settings.getHardware()->getGeneratorOffset(pOsciloscope->window.horizontal.Capture,1));
    pOsciloscope->control.transferData();
    m_textCtrlOffset1->SetValue(pFormat->floatToString(voltage));
}

void OsciloskopHardwareGenerator::m_textCtrlSquareDuty1OnTextEnter(wxCommandEvent& event)
{
    // TODO: Implement m_textCtrlSquareDuty1OnTextEnter
    pOsciloscope->window.hardwareGenerator.squareDuty1 = pFormat->stringToFloat(m_textCtrlSquareDuty1->GetValue().ToAscii().data());
    pOsciloscope->control.setGeneratorSquareDuty1(pOsciloscope->window.hardwareGenerator.squareDuty1);
    pOsciloscope->control.transferData();
    m_sliderSquareDuty1->SetValue(pOsciloscope->window.hardwareGenerator.squareDuty1);
}

void OsciloskopHardwareGenerator::m_sliderSquareDuty1OnScroll(wxScrollEvent& event)
{
    // TODO: Implement m_sliderSquareDuty1OnScroll
    pOsciloscope->window.hardwareGenerator.squareDuty1 = m_sliderSquareDuty1->GetValue();
    pOsciloscope->control.setGeneratorSquareDuty1(m_sliderSquareDuty1->GetValue());
    pOsciloscope->control.transferData();
    m_textCtrlSquareDuty1->SetValue(pFormat->floatToString(pOsciloscope->window.hardwareGenerator.squareDuty1));
}

void OsciloskopHardwareGenerator::m_checkBoxSawSlope1OnCheckBox(wxCommandEvent& event)
{
    // TODO: Implement m_checkBoxSawSlope1OnCheckBox
    pOsciloscope->window.hardwareGenerator.sawSlopePositive1 = m_checkBoxSawSlope1->GetValue();
    pOsciloscope->control.setGeneratorSlope1(m_checkBoxSawSlope1->GetValue());
    pOsciloscope->control.transferData();
}

void OsciloskopHardwareGenerator::m_filePicker11OnFileChanged(wxFileDirPickerEvent& event)
{
    // TODO: Implement m_filePicker11OnFileChanged
    pOsciloscope->window.hardwareGenerator.loadCustomData(1, m_filePicker11->GetFileName().GetFullPath().ToAscii().data());
}

void OsciloskopHardwareGenerator::m_buttonCustomFile1OnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_buttonCustomFile1OnButtonClick
    pOsciloscope->window.hardwareGenerator.loadCustomData(1, m_filePicker11->GetFileName().GetFullPath().ToAscii().data());
    pOsciloscope->window.hardwareGenerator.upload();
}


void OsciloskopHardwareGenerator::m_buttonOn1OnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_radioBtnOn1OnRadioButton
    pOsciloscope->window.hardwareGenerator.onOff1 = 1;
    pOsciloscope->control.setGeneratorOn1(1);
    pOsciloscope->control.transferData();
    if(!pOsciloscope->settings.getColors()->windowDefault)
    {
        m_buttonOn1->SetBackgroundColour(pOsciloscope->settings.getColors()->windowFront);
        m_buttonOn1->SetForegroundColour(pOsciloscope->settings.getColors()->windowBack);
        m_buttonOff1->SetBackgroundColour(pOsciloscope->settings.getColors()->windowBack);
        m_buttonOff1->SetForegroundColour(pOsciloscope->settings.getColors()->windowFront);
    }
}

void OsciloskopHardwareGenerator::m_buttonOff1OnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_radioBtnOff1OnRadioButton
    pOsciloscope->window.hardwareGenerator.onOff1 = 0;
    pOsciloscope->control.setGeneratorOn1(0);
    pOsciloscope->control.transferData();
    if(!pOsciloscope->settings.getColors()->windowDefault)
    {
        m_buttonOn1->SetBackgroundColour(pOsciloscope->settings.getColors()->windowBack);
        m_buttonOn1->SetForegroundColour(pOsciloscope->settings.getColors()->windowFront);
        m_buttonOff1->SetBackgroundColour(pOsciloscope->settings.getColors()->windowFront);
        m_buttonOff1->SetForegroundColour(pOsciloscope->settings.getColors()->windowBack);
    }
}

///////////////////////////////////////////////////////////////
// ch1 - spin
///////////////////////////////////////////////////////////////

void OsciloskopHardwareGenerator::m_spinBtnCh1VoltOnSpinUp(wxSpinEvent& event)
{
    pOsciloscope->window.hardwareGenerator.voltage1++;
    pOsciloscope->window.hardwareGenerator.voltage1 = min(pOsciloscope->window.hardwareGenerator.voltage1, 2048);
    m_textCtrlVoltage1->SetValue(pFormat->floatToString(double(pOsciloscope->window.hardwareGenerator.voltage1)*pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep));
    m_sliderVoltage1->SetValue(pOsciloscope->window.hardwareGenerator.voltage1);
    pOsciloscope->control.setGeneratorVoltage1(pOsciloscope->window.hardwareGenerator.voltage1);
    pOsciloscope->control.transferData();
}

void OsciloskopHardwareGenerator::m_spinBtnCh1VoltOnSpinDown(wxSpinEvent& event)
{
    pOsciloscope->window.hardwareGenerator.voltage1--;
    pOsciloscope->window.hardwareGenerator.voltage1 = max(pOsciloscope->window.hardwareGenerator.voltage1, 0);
    m_textCtrlVoltage1->SetValue(pFormat->floatToString(double(pOsciloscope->window.hardwareGenerator.voltage1)*pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep));
    m_sliderVoltage1->SetValue(pOsciloscope->window.hardwareGenerator.voltage1);
    pOsciloscope->control.setGeneratorVoltage1(pOsciloscope->window.hardwareGenerator.voltage1);
    pOsciloscope->control.transferData();
}

void OsciloskopHardwareGenerator::m_spinBtnCh1OffsetOnSpinUp(wxSpinEvent& event)
{
    pOsciloscope->window.hardwareGenerator.offset1++;
    pOsciloscope->window.hardwareGenerator.offset1 = min(pOsciloscope->window.hardwareGenerator.offset1, 2048);
    m_textCtrlOffset1->SetValue(pFormat->floatToString(double(pOsciloscope->window.hardwareGenerator.offset1)*pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep));
    m_sliderOffset1->SetValue(pOsciloscope->window.hardwareGenerator.offset1);
    pOsciloscope->control.setGeneratorOffset1(pOsciloscope->window.hardwareGenerator.offset1 + pOsciloscope->settings.getHardware()->getGeneratorOffset(pOsciloscope->window.horizontal.Capture,1));
    pOsciloscope->control.transferData();
}

void OsciloskopHardwareGenerator::m_spinBtnCh1OffsetOnSpinDown(wxSpinEvent& event)
{
    pOsciloscope->window.hardwareGenerator.offset1--;
    pOsciloscope->window.hardwareGenerator.offset1 = max(pOsciloscope->window.hardwareGenerator.offset1, -2048);
    m_textCtrlOffset1->SetValue(pFormat->floatToString(double(pOsciloscope->window.hardwareGenerator.offset1)*pOsciloscope->settings.getHardware()->referenceGeneratorVoltagePerStep));
    m_sliderOffset1->SetValue(pOsciloscope->window.hardwareGenerator.offset1);
    pOsciloscope->control.setGeneratorOffset1(pOsciloscope->window.hardwareGenerator.offset1 + pOsciloscope->settings.getHardware()->getGeneratorOffset(pOsciloscope->window.horizontal.Capture,1));
    pOsciloscope->control.transferData();
}

void OsciloskopHardwareGenerator::m_spinBtnGen1SqrDutyOnSpinUp(wxSpinEvent& event)
{
    pOsciloscope->window.hardwareGenerator.squareDuty1++;
    pOsciloscope->control.setGeneratorSquareDuty1(pOsciloscope->window.hardwareGenerator.squareDuty1);
    pOsciloscope->control.transferData();
    m_sliderSquareDuty1->SetValue(pOsciloscope->window.hardwareGenerator.squareDuty1);
    m_textCtrlSquareDuty1->SetValue(pFormat->floatToString(pOsciloscope->window.hardwareGenerator.squareDuty1));
}

void OsciloskopHardwareGenerator::m_spinBtnGen1SqrDutyOnSpinDown(wxSpinEvent& event)
{
    pOsciloscope->window.hardwareGenerator.squareDuty1--;
    pOsciloscope->control.setGeneratorSquareDuty1(pOsciloscope->window.hardwareGenerator.squareDuty1);
    pOsciloscope->control.transferData();
    m_sliderSquareDuty1->SetValue(pOsciloscope->window.hardwareGenerator.squareDuty1);
    m_textCtrlSquareDuty1->SetValue(pFormat->floatToString(pOsciloscope->window.hardwareGenerator.squareDuty1));
}

////////////////////////////////////////////////////////////////////////////////////////////
//
// Ok / Default  / Cancel
//
////////////////////////////////////////////////////////////////////////////////////////////
void OsciloskopHardwareGenerator::m_buttonOkOnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_buttonOkOnButtonClick
    Hide();
}

void OsciloskopHardwareGenerator::m_buttonDefaultOnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_buttonDefaultOnButtonClick
    pOsciloscope->window.hardwareGenerator.Default();
    wxInitDialogEvent e;
    HardwareGeneratorOnInitDialog(e);
}

void OsciloskopHardwareGenerator::m_buttonCancelOnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_buttonCancelOnButtonClick
    Hide();
}
