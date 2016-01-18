/*
** Example Winamp .FTM input plug-in
** Copyright (c) 2015, Christopher Pow/Phrenetic Apps LLC.
**
*/

#include "in2.h"

#include "ftm_iface.h"

// avoid CRT. Evil. Big. Bloated. Only uncomment this code if you are using
// 'ignore default libraries' in VC++. Keeps DLL size way down.
BOOL WINAPI _DllMainCRTStartup(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}

// module definition.

In_Module mod =
{
    IN_VER,	// defined in IN2.H
    "Phrenetic Apps LLC FTM Player v0.0 "
    // winamp runs on both alpha systems and x86 ones. :)
#ifdef __alpha
    "(AXP)"
#else
    "(x86)"
#endif
    ,
    0,	// hMainWindow (filled in by winamp)
    0,  // hDllInstance (filled in by winamp)
    "FTM\0FTM Audio File (*.FTM)\0"
    // this is a double-null limited list. "EXT\0Description\0EXT\0Description\0" etc.
    ,
    0,	// is_seekable
    1,	// uses output plug-in system
    config,
    about,
    init,
    quit,
    getfileinfo,
    infoDlg,
    isourfile,
    play,
    pause,
    unpause,
    ispaused,
    stop,

    getlength,
    getoutputtime,
    setoutputtime,

    setvolume,
    setpan,

    0,0,0,0,0,0,0,0,0, // visualization calls filled in by winamp

    0,0, // dsp calls filled in by winamp

    eq_set,

    NULL,		// setinfo call filled in by winamp

    0 // out_mod filled in by winamp

};

// exported symbol. Returns output module.

__declspec( dllexport ) In_Module * winampGetInModule2()
{
    return &mod;
}
