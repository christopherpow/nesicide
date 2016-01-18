/*
  LICENSE
  -------
Copyright 2005 Nullsoft, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  * Neither the name of Nullsoft nor the names of its contributors may be used to
    endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <QString>
#include "mainwindow.h"
#include <QApplication>
#include <QThread>

#include "Source/FamiTracker.h"
#include "Source/FamiTrackerView.h"
#include "Source/MainFrm.h"

#include "wa_ipc.h"
#include "Vis.h"

//// embedding stuff
//#define WM_WA_IPC WM_USER
//#define IPC_GET_EMBEDIF 505 // pass an embedWindowState; returns an HWND embedWindow(embedWindowState *); if the data is NULL, otherwise returns the HWND directly
//#define EMBED_FLAGS_NORESIZE 1 // set this bit in embedWindowState.flags to keep window from being resizable
//#define EMBED_FLAGS_NOTRANSPARENCY 2 // set this bit in embedWindowState.flags to make gen_ff turn transparency off for this wnd
//#define IPC_SETVISWND 611
//#define ID_VIS_NEXT                     40382
//#define ID_VIS_PREV                     40383
//#define ID_VIS_RANDOM                   40384
//#define ID_VIS_FS                       40389
//#define ID_VIS_CFG                      40390
//#define ID_VIS_MENU                     40391

//typedef struct
//{
//  HWND me; //hwnd of the window
//  int flags;
//  RECT r;
//  void *user_ptr; // for application use
//  int extra_data[64]; // for internal winamp use
//}
//embedWindowState;

// returns a winampVisModule when requested. Used in hdr, below
winampVisModule *getModule(int which);

#define MY_EXT_WINDOW_STYLE 0
#define MY_WINDOW_STYLE (WS_VISIBLE|WS_CHILDWINDOW|WS_OVERLAPPED|WS_CLIPCHILDREN|WS_CLIPSIBLINGS)

HWND m_hwnd;
CWnd* pWnd;
LONG m_uWindowLong;
char m_szClassName[256];
char m_szWindowCaption[512];
HINSTANCE m_hInstance;
embedWindowState myWindowState;

// "member" functions
void config(struct winampVisModule *this_mod); // configuration dialog
int init(struct winampVisModule *this_mod);	   // initialization for module
int render1(struct winampVisModule *this_mod);  // rendering for module 1
void quit(struct winampVisModule *this_mod);   // deinitialization for module

// Module header, includes version, description, and address of the module retriever function
winampVisHeader hdr = { VIS_HDRVER, "Phrenetic Apps LLC FTM Visualizer Plugin v0.0", getModule };

// first module (milkdrop)
winampVisModule mod1 =
{
    "Phrenetic Apps LLC FTM Visualizer Plugin v0.0",
    NULL,	// hwndParent
    NULL,	// hDllInstance
    0,		// sRate
    0,		// nCh
    30,		// latencyMS - delay between audio & video
    10,		// delayMS - winamp will make sure that at least this much time passes per frame.
    0,		// spectrumNch
    2,		// waveformNch
    { 0, },	// spectrumData
    { 0, },	// waveformData
    config,
    init,
    render1,
    quit
};

// this is the only exported symbol. returns our main header.
// if you are compiling C++, the extern "C" { is necessary, so we just #ifdef it
#ifdef __cplusplus
extern "C" {
#endif
__declspec( dllexport ) winampVisHeader *winampVisGetHeader()
{
    return &hdr;
}
#ifdef __cplusplus
}
#endif

// getmodule routine from the main header. Returns NULL if an invalid module was requested,
// otherwise returns either mod1, mod2 or mod3 depending on 'which'.
winampVisModule *getModule(int which)
{
    switch (which)
    {
        case 0: return &mod1;
        //case 1: return &mod2;
        //case 2: return &mod3;
        default: return NULL;
    }
}

// configuration. Passed this_mod, as a "this" parameter. Allows you to make one configuration
// function that shares code for all your modules (you don't HAVE to use it though, you can make
// config1(), config2(), etc...)
void config(struct winampVisModule *this_mod)
{
    MessageBox(NULL,
        "No configuration for .FTM visualizer.",
        "Configuration",MB_OK);
    // if we had a configuration box we'd want to write it here (using DialogBox, etc)
//	if (pg != NULL)
//	{
//		MessageBox( NULL, "Sorry - you can't configure MilkDrop while it's running.\r\nPlease stop MilkDrop and try again.", "Can't configure while running", MB_OK|MB_SETFOREGROUND|MB_TOPMOST|MB_TASKMODAL );
//		return;
//	}

//	pg = new CMilkDropObj;

//	pg->Init(this_mod->hwndParent, this_mod->hDllInstance);

//	InitCommonControls(); // loads common controls DLL

//	// note: DialogBox is modal, but CreateDialog is modeless
//	//CreateDialog(this_mod->hDllInstance, MAKEINTRESOURCE(IDD_CONFIG), NULL, (DLGPROC)ConfigDialogProc);
//	DialogBox(this_mod->hDllInstance, MAKEINTRESOURCE(IDD_CONFIG), NULL,/*this_mod->hwndParent,*/ (DLGPROC)ConfigDialogProc);

//	assert(pg != NULL);
//	if (pg != NULL)
//	{
//		delete pg;
//		pg = NULL;
//	}
}

// initialization. Registers our window class, creates our window, etc. Again, this one works for
// both modules, but you could make init1() and init2()...
// returns 0 on success, 1 on failure.
int init(struct winampVisModule *this_mod)
{
    CFamiTrackerApp* pApp = (CFamiTrackerApp*)AfxGetApp();
    CMainFrame* pMainFrame = (CMainFrame*)pApp->m_pMainWnd;

    if ( GetModuleHandle("in_ftm.dll") == NULL )
    {
        MessageBox(NULL,"FTM Visualizer Plugin requires\nFTM Player Plugin!","FTM plugin needed...",MB_OK);
        return 1;
    }

    pWnd = (CWnd*)pMainFrame->GetVisualizerWindow();

    // only works on winamp 2.90+!
    memset(&myWindowState,0,sizeof(myWindowState));
    int success = 0;
    if (SendMessage(this_mod->hwndParent,WM_WA_IPC,0,0) >= 0x2900)
    {
        myWindowState.flags |= EMBED_FLAGS_NOTRANSPARENCY;
        myWindowState.me = (HWND)pWnd;
        myWindowState.r.left   = 0;
        myWindowState.r.top    = 0;
        myWindowState.r.right  = myWindowState.r.left + 256;
        myWindowState.r.bottom = myWindowState.r.top  + 256;
        HWND (*e)(embedWindowState *v);
        *(void**)&e = (void *)SendMessage(this_mod->hwndParent,WM_WA_IPC,(LPARAM)0,IPC_GET_EMBEDIF);
        if (e)
        {
            HWND p = e(&myWindowState);
            if (p)
            {
                SetWindowText(p,"m_szWindowCaption");
            }
            else
            {
                MessageBox(NULL,"p failed","...",0);
            }
        }
        else
        {
            MessageBox(NULL,"e failed","...",0);
        }
    }

    pWnd->ShowWindow(SW_SHOW);

    if (!pWnd)
    {
        MessageBox(m_hwnd, "CreateWindow failed", "ERROR", MB_OK|MB_SETFOREGROUND|MB_TOPMOST);
        return 1;
    }

//    SetWindowLong(m_hwnd, GWL_USERDATA, m_uWindowLong);

    SendMessage(this_mod->hwndParent, WM_WA_IPC, (int)(HWND)pWnd, IPC_SETVISWND);

    ShowWindow(this_mod->hwndParent,SW_SHOWNA); // showing the parent wnd will make it size the child, too
//	if (pg != NULL)
//	{
//		MessageBox( NULL, "Sorry - you can't run MilkDrop while it's already open\r\n(either running or being configured).\r\n\r\nPlease exit all instances of MilkDrop and try again.", "Multiple Instances Not Allowed", MB_OK|MB_SETFOREGROUND|MB_TOPMOST|MB_TASKMODAL );
//		return 1;
//	}

//	// query winamp for its playback state
//	int ret = SendMessage(this_mod->hwndParent, WM_USER, 0, 104);
//	// ret=1: playing, ret=3: paused, other: stopped

//	if (ret != 1)
//	{
//		MessageBox( NULL, "MilkDrop can't run without music.\r\nPlease play some music, through Winamp, and then try running MilkDrop again.", "No music playing", MB_OK|MB_SETFOREGROUND|MB_TOPMOST|MB_TASKMODAL );
//		return 1;  // failure
//	}

//	if (pg != NULL)
//	{
//		delete pg;
//		pg = NULL;
//	}

//	pg = new CMilkDropObj;

//	pg->Init(this_mod->hwndParent, this_mod->hDllInstance);

//	if (pg->GoFullScreen())
//	{
//		// success
//		return 0;
//	}
//	else
//	{
//		pg->Finish();
//		if (pg != NULL)
//		{
//			delete pg;
//			pg = NULL;
//		}

//		// failure
//		return 1;
//	}

//	// show the window
//	//ShowWindow(pg->m_hMainWnd,SW_SHOWNORMAL);

    return 0;
}

// render function for oscilliscope. Returns 0 if successful, 1 if visualization should end.
int render1(struct winampVisModule *this_mod)
{
//	pg->RenderFrame(this_mod->waveformData[0], this_mod->waveformData[1]);

    return 0;
}


// cleanup (opposite of init()). Destroys the window, unregisters the window class, nukes D3D
void quit(struct winampVisModule *this_mod)
{
//	assert(pg != NULL);
//	pg->Finish();
//	if (pg != NULL)
//	{
//		delete pg;
//		pg = NULL;
//	}
}




