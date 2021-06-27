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
#include "OsciloskopDebug.h"

OsciloskopDebug::OsciloskopDebug(wxWindow* parent)
    :
    Debug(parent)
{
    m_Redirect = new wxStreamToTextRedirector(m_textCtrl41);

    wxFont font(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Courier New");

    m_textCtrl41->SetFont(font);
}

void OsciloskopDebug::ThermalOnActivate(wxActivateEvent& event)
{
    // TODO: Implement ThermalOnActivate
}
void OsciloskopDebug::Clear()
{
    #if defined(PLATFORM_MINGW)
      system("cls");
    #else
      system("clear");
    #endif
    m_textCtrl41->Clear();
}
void OsciloskopDebug::AppendText(const char* str)
{
    std::cout << str;
}
void OsciloskopDebug::SetText(std::string str)
{
    m_textCtrl41->SetValue(str.c_str());
}
