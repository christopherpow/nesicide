static char __emu_version__ [] = "V1.005"
#if defined ( QT_NO_DEBUG )
" RELEASE";
#else
" DEBUG";
#endif

char* emuGetVersion()
{
   return __emu_version__;
}
