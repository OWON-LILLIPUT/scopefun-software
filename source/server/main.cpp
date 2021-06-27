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
#include<server/ui/ScopeFunServerUI.h>
#include<api/scopefunapi.h>

#include "wx/wxprec.h"
#include "wx/app.h"
#include "wx/dynlib.h"
#include "wx/frame.h"
#include "wx/panel.h"
#include "wx/sizer.h"
#include "wx/stattext.h"
#include "wx/button.h"
#include "wx/thread.h"
#include "wx/msgdlg.h"
#include "wx/filename.h"


#include <wx/stdpaths.h>
#include <wx/cmdline.h>

extern void create();
extern void createServer();
extern void setup();

static const wxCmdLineEntryDesc g_cmdLineDesc[] =
{
    { wxCMD_LINE_OPTION,     "cc", 0, 0, wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_USAGE_TEXT, "cc", "client count", "max number of client", wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION,     "mm", 0, 0, wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_USAGE_TEXT, "mm", "max memory", "max server memory in megabytes", wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION,     "ip", 0, 0, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_USAGE_TEXT, "ip", "ip number", "ip as string, default is 127.0.0.1", wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION,     "port", 0, 0, wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_USAGE_TEXT, "port", "port number", "port number, default is 42250", wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_NONE }
};

class ServerApp : public wxApp
{
public:
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser)
    {
        if(parser.Found(wxT("cc"), (long*)&pServer->maxClient))
        {
            pServer->maxClient = min<int>(pServer->maxClient, SCOPEFUN_MAX_CLIENT);
        }
        else
        {
            pServer->maxClient = SCOPEFUN_MAX_CLIENT;
        }
        if(parser.Found(wxT("mm"), (long*)&pServer->maxMemory))
        {
            pServer->maxMemory *= MEGABYTE;
        }
        else
        {
            pServer->maxMemory = 16 * MEGABYTE;
        }
        wxString ip;
        if(parser.Found(wxT("ip"), &ip))
        {
            pServer->ip = ip.data().AsChar();
        }
        else
        {
            pServer->ip = "127.0.0.1";
        }
        uint port = 0;
        if(parser.Found(wxT("port"), (long*)&port))
        {
            pServer->port = port;
        }
        else
        {
            pServer->port = 42250;
        }
        return true;
    }

    virtual void OnInitCmdLine(wxCmdLineParser& parser)
    {
        parser.SetDesc(g_cmdLineDesc);
        parser.SetSwitchChars(wxT("-"));
    }

    bool OnInit()
    {
        // create
        create();
        // init
        if(!wxApp::OnInit())
        {
            return false;
        }
        // working dir
        FORMAT_BUFFER();
        FORMAT("%s\\", (const char*)wxGetCwd().char_str().data());
        pFormat->setCurrentWorkingPath(formatBuffer);
        pFormat->setCurrentWorkingExe(wxStandardPaths::Get().GetExecutablePath().char_str().data());
        // used by wxConfig
        SetAppName("Server");
        // setup
        setup();
        // start
        pManager->start();
        // setup frame
        wxApp* pApp = (wxApp*)wxApp::GetInstance();
        ScopeFunServerUI* frame = new ScopeFunServerUI(0);
        pApp->SetTopWindow(frame);
        // show window
        frame->Show(true);
        frame->Layout();
        // return
        return true;
    }

    int OnExit()
    {
        // stop
        pManager->stop();
        return 0;
    }
};

wxIMPLEMENT_APP(ServerApp);
