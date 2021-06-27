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

#if defined(PLATFORM_WIN) || defined(PLATFORM_MINGW)
    #include "wx/msw/wrapwin.h"
    #include <windows.h>
#endif

extern void create();
extern void setup();

class OscApp : public wxApp
{
public:
    bool OnInit()
    {
        try
        {
            // init
            if(!wxApp::OnInit())
            {
                return false;
            }
            #ifndef PLATFORM_LINUX
            void create();
            create();
            FORMAT_BUFFER();
            FORMAT("%s/", (const char*)wxGetCwd().char_str().data());
            pFormat->setCurrentWorkingPath(formatBuffer);
            pFormat->setCurrentWorkingExe(wxStandardPaths::Get().GetExecutablePath().char_str().data());
            void setup();
            setup();
            pManager->start();
            #endif

            // used by wxConfig
            SetAppName("Oscilloscope");
            // load localization
            int loadLanguageFromConfig();
            int language = loadLanguageFromConfig();
            // set localization
            void setLocalization(int id);
            setLocalization(language);
            // save localization
            void saveLanguageToConfig(int id);
            saveLanguageToConfig(language);
            // gui
            void recreateGUI(int initial = 0);
            recreateGUI(1);
        }
        catch(...)
        {
            int debug = 1;
        }
        return true;
    }

    void OnIdle(wxIdleEvent& event)
    {
    }

    int OnExit()
    {
        // stop
        pOsciloscope->exitThreads();
        pManager->stop();
        return 0;
    }
};

#ifdef PLATFORM_LINUX
    wxIMPLEMENT_APP_NO_MAIN(OscApp);
#else
    wxIMPLEMENT_APP(OscApp);
#endif

#ifdef PLATFORM_LINUX

#include <unistd.h>

int main(int argc, char** argv)
{
    if (geteuid() != 0)
    {
       coreMessage("Access denied. Run program as super user!");
       return 0;
    }

    // create
    create();

    // working dir
    pFormat->setCurrentWorkingPath("/usr/lib/oscilloscope/");

    // setup
    setup();
    // start
    pManager->start();
    // main loop
    wxEntry(argc, argv);
    return 0;
}
#endif

