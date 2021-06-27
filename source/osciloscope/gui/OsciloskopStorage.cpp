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
#include "OsciloskopStorage.h"

OsciloskopStorage::OsciloskopStorage(wxWindow* parent)
    :
    Storage(parent)
{
    if(m_choiceStorage->GetSelection() == 0)
    {
        m_textCtrlStorage->Disable();
    }
    wxCommandEvent evnt;
    m_buttonDefaultOnButtonClick(evnt);
}

void OsciloskopStorage::m_choiceStorageOnChoice(wxCommandEvent& event)
{
    // TODO: Implement m_choiceStorageOnChoice
    if(m_choiceStorage->GetSelection() == 0)
    {
        m_textCtrlStorage->Disable();
        pOsciloscope->captureBuffer->setMemory();
    }
    else
    {
        wxString tempDir = wxStandardPaths::Get().GetTempDir();
        FORMAT_BUFFER();
        FORMAT("%s/memory.ssd", tempDir.data().AsChar());
        pOsciloscope->captureBuffer->historySSD.init(formatBuffer, 2048 * MEGABYTE, pOsciloscope->window.storage.getPacketSize());
        m_textCtrlStorage->Enable();
        pOsciloscope->captureBuffer->setSSD();
    }
}

void OsciloskopStorage::m_textCtrlStorageOnTextEnter(wxCommandEvent& event)
{
    // TODO: Implement m_textCtrlStorageOnTextEnter
    ularge newsize = atoi(m_textCtrlStorage->GetValue());
    newsize = max<ularge>(newsize, 64U);
    newsize *= MEGABYTE;
    pOsciloscope->captureBuffer->history->resize(newsize, 0);
    FORMAT_BUFFER();
    FORMAT("%d", newsize / MEGABYTE);
    m_textCtrlStorage->SetValue(wxString::FromAscii(formatBuffer));
}

void OsciloskopStorage::m_choicePacketSizeOnChoice(wxCommandEvent& event)
{
    // TODO: Implement m_choicePacketSizeOnChoice
    pOsciloscope->window.storage.packet = (PacketType)m_choicePacketSize->GetSelection();
    int version = 0;
    int header  = 0;
    int data    = 0;
    int packet  = 0;
    pOsciloscope->thread.getFrame(&version, &header, &data, &packet);
    packet = pOsciloscope->window.storage.getPacketSize(version);

    pOsciloscope->thread.setFrame(version,header,data,packet);
    pOsciloscope->thread.function(afSetFrame);
    pOsciloscope->thread.function(afServerUpload);
}

void OsciloskopStorage::m_buttonOkOnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_buttonOkOnButtonClick
    Hide();
    wxCommandEvent evnt;
    m_textCtrlStorageOnTextEnter(evnt);
}

void OsciloskopStorage::m_buttonDefaultOnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_buttonDefaultOnButtonClick
    pOsciloscope->window.storage.Default();
    FORMAT_BUFFER();
    FORMAT("%d", pOsciloscope->window.storage.size);
    m_textCtrlStorage->SetValue(wxString::FromAscii(formatBuffer));
    m_choicePacketSize->SetSelection((int)pOsciloscope->window.storage.packet);
}

void OsciloskopStorage::m_buttonCancelOnButtonClick(wxCommandEvent& event)
{
    // TODO: Implement m_buttonCancelOnButtonClick
    Hide();
}
