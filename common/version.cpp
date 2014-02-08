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

// context menu events go to wrong widget causing crash?
// focusing back to pattern editor from frame editor is broken.
