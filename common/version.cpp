static char __nesicide_version__ [] = "V1.041"
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
