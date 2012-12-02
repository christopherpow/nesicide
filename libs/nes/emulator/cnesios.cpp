#include "cnesios.h"
#include "cnesio.h"

IOFuncs iofunc[] =
{
   /* Disconnected    */ { CIO::IO, CIO::IO, CIO::_IO, CIO::_IO, NULL },
   /* Standard joypad */ { CIOStandardJoypad::IO, CIOStandardJoypad::IO, CIOStandardJoypad::_IO, CIOStandardJoypad::_IO, NULL },
   /* Turbo joypad    */ { CIOStandardJoypad::IO, CIOTurboJoypad::IO, CIOTurboJoypad::_IO, CIOTurboJoypad::_IO, NULL },
   /* Zapper          */ { CIOZapper::IO, CIOZapper::IO, CIOZapper::_IO, CIOZapper::_IO, NULL },
   /* Vaus(Arkanoid)  */ { CIOVaus::IO, CIOVaus::IO, CIOVaus::_IO, CIOVaus::_IO, CIOVaus::SPECIAL }
   /* ... */
};
