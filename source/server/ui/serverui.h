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
#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/listbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/frame.h>

#include "server/servermanager.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ServerUI
///////////////////////////////////////////////////////////////////////////////
class ServerUI : public wxFrame
{
private:

protected:
    wxButton* m_button1;
    wxStaticText* m_staticText1;
    wxTextCtrl* m_textCtrl1;
    wxStaticText* m_staticText2;
    wxTextCtrl* m_textCtrl2;
    wxCheckBox* m_checkBox2;

    // Virtual event handlers, overide them in your derived class
    virtual void ServerUIOnActivate(wxActivateEvent& event)
    {
        event.Skip();
    }
    virtual void ServerUIOnClose(wxCloseEvent& event)
    {
        event.Skip();
    }
    virtual void m_button1OnButtonClick(wxCommandEvent& event)
    {
        event.Skip();
    }
    virtual void m_checkBox1OnCheckBox(wxCommandEvent& event)
    {
        event.Skip();
    }
    virtual void m_checkBox2OnCheckBox(wxCommandEvent& event)
    {
        event.Skip();
    }


public:
    wxListBox* m_listBox2;
    wxListBox* m_listBox1;
    wxCheckBox* m_checkBox1;

    ServerUI(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Server"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(-1, -1), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);

    ~ServerUI();

};

