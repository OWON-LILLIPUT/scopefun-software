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
#include "serverui.h"

///////////////////////////////////////////////////////////////////////////

ServerUI::ServerUI(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);
    wxBoxSizer* bSizer1;
    bSizer1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* bSizer2;
    bSizer2 = new wxBoxSizer(wxVERTICAL);
    m_listBox2 = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
    bSizer2->Add(m_listBox2, 1, wxALL | wxEXPAND, 5);
    m_listBox1 = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
    bSizer2->Add(m_listBox1, 2, wxALL | wxEXPAND, 5);
    wxBoxSizer* bSizer4;
    bSizer4 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* bSizer5;
    bSizer5 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* bSizer6;
    bSizer6 = new wxBoxSizer(wxHORIZONTAL);
    m_button1 = new wxButton(this, wxID_ANY, wxT("Kill Client"), wxDefaultPosition, wxDefaultSize, 0);
    bSizer6->Add(m_button1, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    m_staticText1 = new wxStaticText(this, wxID_ANY, wxT("IP"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticText1->Wrap(-1);
    bSizer6->Add(m_staticText1, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    m_textCtrl1 = new wxTextCtrl(this, wxID_ANY, wxT("127.0.0.1"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_RIGHT);
    bSizer6->Add(m_textCtrl1, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    m_staticText2 = new wxStaticText(this, wxID_ANY, wxT("Port"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticText2->Wrap(-1);
    bSizer6->Add(m_staticText2, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    m_textCtrl2 = new wxTextCtrl(this, wxID_ANY, wxT("42250"), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | wxTE_READONLY | wxTE_RIGHT);
    bSizer6->Add(m_textCtrl2, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    bSizer5->Add(bSizer6, 1, wxEXPAND, 5);
    wxBoxSizer* bSizer7;
    bSizer7 = new wxBoxSizer(wxHORIZONTAL);
    m_checkBox1 = new wxCheckBox(this, wxID_ANY, wxT("Logging"), wxDefaultPosition, wxDefaultSize, 0);
    bSizer7->Add(m_checkBox1, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    m_checkBox2 = new wxCheckBox(this, wxID_ANY, wxT("update simulation"), wxDefaultPosition, wxDefaultSize, 0);
    bSizer7->Add(m_checkBox2, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    bSizer5->Add(bSizer7, 1, wxEXPAND, 5);
    bSizer4->Add(bSizer5, 1, wxEXPAND, 5);
    bSizer2->Add(bSizer4, 1, wxEXPAND, 5);
    bSizer1->Add(bSizer2, 1, wxEXPAND, 5);
    this->SetSizer(bSizer1);
    this->Layout();
    bSizer1->Fit(this);
    this->Centre(wxBOTH);
    // Connect Events
    this->Connect(wxEVT_ACTIVATE, wxActivateEventHandler(ServerUI::ServerUIOnActivate));
    this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(ServerUI::ServerUIOnClose));
    m_button1->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ServerUI::m_button1OnButtonClick), NULL, this);
    m_checkBox1->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(ServerUI::m_checkBox1OnCheckBox), NULL, this);
    m_checkBox2->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(ServerUI::m_checkBox2OnCheckBox), NULL, this);
}

ServerUI::~ServerUI()
{
    // Disconnect Events
    this->Disconnect(wxEVT_ACTIVATE, wxActivateEventHandler(ServerUI::ServerUIOnActivate));
    this->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(ServerUI::ServerUIOnClose));
    m_button1->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(ServerUI::m_button1OnButtonClick), NULL, this);
    m_checkBox1->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(ServerUI::m_checkBox1OnCheckBox), NULL, this);
    m_checkBox2->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(ServerUI::m_checkBox2OnCheckBox), NULL, this);
}
