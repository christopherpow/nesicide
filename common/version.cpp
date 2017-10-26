static char __nesicide_version__ [] = "v2.0.0"
#if defined ( QT_NO_DEBUG )
" RELEASE";
#else
" DEBUG";
#endif

char* nesicideGetVersion()
{
   return __nesicide_version__;
}

// main menu items that are in base classes now don't work because i ripped out my hacks.  need to put them back in--properly.
// ID_VIEW_TOOLBAR, ID_VIEW_STATUS_BAR

// QFontMetrics::averageCharWidth hack in MapDialogRect 

// FillSolidRect and Draw3dRect do exactly what MFC does

// OnTimer ASSERT in FamiTrackerView when updating meters when
// interrupted by a file reload with a file with less configured
// channels than the currently loaded file.

// Key presses are being passed up the chain of Qt event system when they shouldn't 
// be because they've been acted-upon by the MFC event system.  But there's no clean
// way to differentiate...that I've found...yet.  This impacts things like shortcut
// accelerators, and pressing RETURN to close a dialog also starts/stops the tracker
// because RETURN is "play toggle" shortcut.

// CFrameWnd also handles the WM_ENTERIDLE message to describe the current menu
// item selected on the status bar.  msdn.microsoft.com/en-us/library/xt2c310k.aspx
