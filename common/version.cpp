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

// Need to implement focus better.  CEdit's aren't getting focus properly in CDialogs, causing improper key input terminations.
