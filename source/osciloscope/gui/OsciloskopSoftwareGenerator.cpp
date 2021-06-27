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
#include "OsciloskopSoftwareGenerator.h"

OsciloskopSoftwareGenerator::OsciloskopSoftwareGenerator(wxWindow* parent)
    :
    SoftwareGenerator(parent)
{
}

void setValueMultiplyer(float value, wxTextCtrl* text, wxChoice* combo)
{
    uint  multiEnum  = multiplyerFromValue(value);
    float multiFloat = multiplyerFromEnum(multiEnum);
    text->SetValue(wxString::FromAscii(pFormat->floatToString(value / multiFloat)));
    combo->SetSelection(multiEnum);
}

void OsciloskopSoftwareGenerator::SoftwareGeneratorOnInitDialog(wxInitDialogEvent& event)
{
    // TODO: Implement SoftwareGeneratorOnInitDialog
    m_comboBoxType0->SetSelection(pOsciloscope->window.softwareGenerator.channel[0].type);
    m_comboBoxType1->SetSelection(pOsciloscope->window.softwareGenerator.channel[1].type);
    //
    m_textCtrlSpeed0->SetValue(wxString::FromAscii(pFormat->floatToString(pOsciloscope->window.softwareGenerator.channel[0].speed)));
    m_textCtrlSpeed1->SetValue(wxString::FromAscii(pFormat->floatToString(pOsciloscope->window.softwareGenerator.channel[1].speed)));
    //
    m_textCtrlAvery0->SetValue(wxString::FromAscii(pFormat->floatToString(pOsciloscope->window.softwareGenerator.channel[0].every)));
    m_textCtrlAvery1->SetValue(wxString::FromAscii(pFormat->floatToString(pOsciloscope->window.softwareGenerator.channel[1].every)));
    //
    setValueMultiplyer(pOsciloscope->window.softwareGenerator.channel[0].period, m_textCtrlPeriod0, m_comboBoxPeriod0);
    setValueMultiplyer(pOsciloscope->window.softwareGenerator.channel[1].period, m_textCtrlPeriod1, m_comboBoxPeriod1);
    //
    setValueMultiplyer(pOsciloscope->window.softwareGenerator.channel[0].peakToPeak, m_textCtrlPeek0, m_comboBoxPeek0);
    setValueMultiplyer(pOsciloscope->window.softwareGenerator.channel[1].peakToPeak, m_textCtrlPeek1, m_comboBoxPeek1);
    //
    if(!pOsciloscope->settings.getColors()->windowDefault)
    {
        if(pOsciloscope->window.softwareGenerator.channel[0].onOff)
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
        if(pOsciloscope->window.softwareGenerator.channel[1].onOff)
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
    SSimulate sim = pOsciloscope->GetServerSim();
    pOsciloscope->transmitSim(sim);
}

void OsciloskopSoftwareGenerator::m_comboBoxType0OnCombobox(wxCommandEvent& event)
{
    // TODO: Implement m_comboBoxType0OnCombobox
    pOsciloscope->window.softwareGenerator.channel[0].type = (GenerateType)m_comboBoxType0->GetSelection();
    SSimulate sim = pOsciloscope->GetServerSim();
    pOsciloscope->transmitSim(sim);
}

void OsciloskopSoftwareGenerator::m_textCtrlPeriod0OnTextEnter(wxCommandEvent& event)
{
    // TODO: Implement m_textCtrlPeriod0OnTextEnter
    pOsciloscope->window.softwareGenerator.channel[0].period = pFormat->stringToFloat(m_textCtrlPeriod0->GetValue().ToAscii().data()) * multiplyerFromEnum(m_comboBoxPeriod0->GetSelection());
    SSimulate sim = pOsciloscope->GetServerSim();
    pOsciloscope->transmitSim(sim);
}

void OsciloskopSoftwareGenerator::m_comboBoxPeriod0OnCombobox(wxCommandEvent& event)
{
    // TODO: Implement m_comboBoxPeriod0OnCombobox
    pOsciloscope->window.softwareGenerator.channel[0].period = pFormat->stringToFloat(m_textCtrlPeriod0->GetValue().ToAscii().data()) * multiplyerFromEnum(m_comboBoxPeriod0->GetSelection());
    SSimulate sim = pOsciloscope->GetServerSim();
    pOsciloscope->transmitSim(sim);
}

void OsciloskopSoftwareGenerator::m_textCtrlPeek0OnTextEnter(wxCommandEvent& event)
{
    // TODO: Implement m_textCtrlPeek0OnTextEnter
    pOsciloscope->window.softwareGenerator.channel[0].peakToPeak = pFormat->stringToFloat(m_textCtrlPeek0->GetValue().ToAscii().data()) * multiplyerFromEnum(m_comboBoxPeek0->GetSelection());
    SSimulate sim = pOsciloscope->GetServerSim();
    pOsciloscope->transmitSim(sim);
}

void OsciloskopSoftwareGenerator::m_comboBoxPeek0OnCombobox(wxCommandEvent& event)
{
    // TODO: Implement m_comboBoxPeek0OnCombobox
    pOsciloscope->window.softwareGenerator.channel[0].peakToPeak = pFormat->stringToFloat(m_textCtrlPeek0->GetValue().ToAscii().data()) * multiplyerFromEnum(m_comboBoxPeek0->GetSelection());
    SSimulate sim = pOsciloscope->GetServerSim();
    pOsciloscope->transmitSim(sim);
}

void OsciloskopSoftwareGenerator::m_textCtrlSpeed0OnTextEnter(wxCommandEvent& event)
{
    // TODO: Implement m_textCtrlSpeed0OnTextEnter
    pOsciloscope->window.softwareGenerator.channel[0].speed = pFormat->stringToFloat(m_textCtrlSpeed0->GetValue().ToAscii().data());
    SSimulate sim = pOsciloscope->GetServerSim();
    pOsciloscope->transmitSim(sim);
}

void OsciloskopSoftwareGenerator::m_textCtrlAvery0OnTextEnter(wxCommandEvent& event)
{
    // TODO: Implement m_textCtrlAvery0OnTextEnter
    pOsciloscope->window.softwareGenerator.channel[0].every = pFormat->stringToFloat(m_textCtrlAvery0->GetValue().ToAscii().data());
    SSimulate sim = pOsciloscope->GetServerSim();
    pOsciloscope->transmitSim(sim);
}

void OsciloskopSoftwareGenerator::m_buttonOn0OnButtonClick(wxCommandEvent& event)
{
    pOsciloscope->window.softwareGenerator.channel[0].onOff = 1;
    SSimulate sim = pOsciloscope->GetServerSim();
    pOsciloscope->transmitSim(sim);
    if(!pOsciloscope->settings.getColors()->windowDefault)
    {
        m_buttonOn0->SetBackgroundColour(pOsciloscope->settings.getColors()->windowFront);
        m_buttonOn0->SetForegroundColour(pOsciloscope->settings.getColors()->windowBack);
        m_buttonOff0->SetBackgroundColour(pOsciloscope->settings.getColors()->windowBack);
        m_buttonOff0->SetForegroundColour(pOsciloscope->settings.getColors()->windowFront);
    }
}

void OsciloskopSoftwareGenerator::m_buttonOff0OnButtonClick(wxCommandEvent& event)
{
    pOsciloscope->window.softwareGenerator.channel[0].onOff = 0;
    SSimulate sim = pOsciloscope->GetServerSim();
    pOsciloscope->transmitSim(sim);
    if(!pOsciloscope->settings.getColors()->windowDefault)
    {
        m_buttonOn0->SetBackgroundColour(pOsciloscope->settings.getColors()->windowBack);
        m_buttonOn0->SetForegroundColour(pOsciloscope->settings.getColors()->windowFront);
        m_buttonOff0->SetBackgroundColour(pOsciloscope->settings.getColors()->windowFront);
        m_buttonOff0->SetForegroundColour(pOsciloscope->settings.getColors()->windowBack);
    }
}


void OsciloskopSoftwareGenerator::m_comboBoxType1OnCombobox(wxCommandEvent& event)
{
    // TODO: Implement m_comboBoxType1OnCombobox
    pOsciloscope->window.softwareGenerator.channel[1].type = (GenerateType)m_comboBoxType1->GetSelection();
    SSimulate sim = pOsciloscope->GetServerSim();
    pOsciloscope->transmitSim(sim);
}


void OsciloskopSoftwareGenerator::m_textCtrlPeriod1OnTextEnter(wxCommandEvent& event)
{
    // TODO: Implement m_textCtrlPeriod1OnTextEnter
    pOsciloscope->window.softwareGenerator.channel[1].period = pFormat->stringToFloat(m_textCtrlPeriod1->GetValue().ToAscii().data()) * multiplyerFromEnum(m_comboBoxPeriod1->GetSelection());
    SSimulate sim = pOsciloscope->GetServerSim();
    pOsciloscope->transmitSim(sim);
}

void OsciloskopSoftwareGenerator::m_comboBoxPeriod1OnCombobox(wxCommandEvent& event)
{
    // TODO: Implement m_comboBoxPeriod1OnCombobox
    pOsciloscope->window.softwareGenerator.channel[1].period = pFormat->stringToFloat(m_textCtrlPeriod1->GetValue().ToAscii().data()) * multiplyerFromEnum(m_comboBoxPeriod1->GetSelection());
    SSimulate sim = pOsciloscope->GetServerSim();
    pOsciloscope->transmitSim(sim);
}

void OsciloskopSoftwareGenerator::m_textCtrlPeek1OnTextEnter(wxCommandEvent& event)
{
    // TODO: Implement m_textCtrlPeek1OnTextEnter
    pOsciloscope->window.softwareGenerator.channel[1].peakToPeak = pFormat->stringToFloat(m_textCtrlPeek1->GetValue().ToAscii().data()) * multiplyerFromEnum(m_comboBoxPeek1->GetSelection());
    SSimulate sim = pOsciloscope->GetServerSim();
    pOsciloscope->transmitSim(sim);
}

void OsciloskopSoftwareGenerator::m_comboBoxPeek1OnCombobox(wxCommandEvent& event)
{
    // TODO: Implement m_comboBoxPeek1OnCombobox
    pOsciloscope->window.softwareGenerator.channel[1].peakToPeak = pFormat->stringToFloat(m_textCtrlPeek1->GetValue().ToAscii().data()) * multiplyerFromEnum(m_comboBoxPeek1->GetSelection());
    SSimulate sim = pOsciloscope->GetServerSim();
    pOsciloscope->transmitSim(sim);
}

void OsciloskopSoftwareGenerator::m_textCtrlSpeed1OnTextEnter(wxCommandEvent& event)
{
    // TODO: Implement m_textCtrlSpeed1OnTextEnter
    pOsciloscope->window.softwareGenerator.channel[1].speed = pFormat->stringToFloat(m_textCtrlSpeed1->GetValue().ToAscii().data());
    SSimulate sim = pOsciloscope->GetServerSim();
    pOsciloscope->transmitSim(sim);
}

void OsciloskopSoftwareGenerator::m_textCtrlAvery1OnTextEnter(wxCommandEvent& event)
{
    // TODO: Implement m_textCtrlAvery1OnTextEnter
    pOsciloscope->window.softwareGenerator.channel[1].every = pFormat->stringToFloat(m_textCtrlAvery1->GetValue().ToAscii().data());
    SSimulate sim = pOsciloscope->GetServerSim();
    pOsciloscope->transmitSim(sim);
}

void OsciloskopSoftwareGenerator::m_buttonOn1OnButtonClick(wxCommandEvent& event)
{
    pOsciloscope->window.softwareGenerator.channel[1].onOff = 1;
    SSimulate sim = pOsciloscope->GetServerSim();
    pOsciloscope->transmitSim(sim);
    if(!pOsciloscope->settings.getColors()->windowDefault)
    {
        m_buttonOn1->SetBackgroundColour(pOsciloscope->settings.getColors()->windowFront);
        m_buttonOn1->SetForegroundColour(pOsciloscope->settings.getColors()->windowBack);
        m_buttonOff1->SetBackgroundColour(pOsciloscope->settings.getColors()->windowBack);
        m_buttonOff1->SetForegroundColour(pOsciloscope->settings.getColors()->windowFront);
    }
}

void OsciloskopSoftwareGenerator::m_buttonOff1OnButtonClick(wxCommandEvent& event)
{
    pOsciloscope->window.softwareGenerator.channel[1].onOff = 0;
    SSimulate sim = pOsciloscope->GetServerSim();
    pOsciloscope->transmitSim(sim);
    if(!pOsciloscope->settings.getColors()->windowDefault)
    {
        m_buttonOn1->SetBackgroundColour(pOsciloscope->settings.getColors()->windowBack);
        m_buttonOn1->SetForegroundColour(pOsciloscope->settings.getColors()->windowFront);
        m_buttonOff1->SetBackgroundColour(pOsciloscope->settings.getColors()->windowFront);
        m_buttonOff1->SetForegroundColour(pOsciloscope->settings.getColors()->windowBack);
    }
}

void OsciloskopSoftwareGenerator::m_buttonOkOnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_buttonOkOnButtonClick
    Hide();
}

void OsciloskopSoftwareGenerator::m_buttonDefaultOnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_buttonDefaultOnButtonClick
    pOsciloscope->window.softwareGenerator.Default();
    wxInitDialogEvent e;
    SoftwareGeneratorOnInitDialog(e);
}

void OsciloskopSoftwareGenerator::m_buttonCancelOnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_buttonCancelOnButtonClick
    Hide();
}
