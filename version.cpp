#if defined ( QT_NO_DEBUG )
static char __ide_version__ [] = "V1.001 RELEASE";
#else
static char __ide_version__ [] = "V1.001 DEBUG";
#endif

char* ideGetVersion()
{
   return __ide_version__;
}
