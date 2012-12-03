//    NESICIDE - an IDE for the 8-bit NES.
//    Copyright (C) 2009  Christopher S. Pow

//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "cnesmappers.h"
#include "cnesrom.h"
#include "cnesrommapper001.h"
#include "cnesrommapper002.h"
#include "cnesrommapper003.h"
#include "cnesrommapper004.h"
#include "cnesrommapper005.h"
#include "cnesrommapper007.h"
#include "cnesrommapper009.h"
#include "cnesrommapper010.h"
#include "cnesrommapper011.h"
#include "cnesrommapper013.h"
#include "cnesrommapper021.h"
#include "cnesrommapper022.h"
#include "cnesrommapper023.h"
#include "cnesrommapper024.h"
#include "cnesrommapper025.h"
#include "cnesrommapper026.h"
#include "cnesrommapper028.h"
#include "cnesrommapper033.h"
#include "cnesrommapper034.h"
#include "cnesrommapper065.h"
#include "cnesrommapper068.h"
#include "cnesrommapper069.h"
#include "cnesrommapper075.h"

MapperFuncs mapperfunc[] =
{
   /* 000 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 001 */ { CROMMapper001::RESET, CROM::HMAPPER,          CROMMapper001::HMAPPER, CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROMMapper001::SYNCCPU, CROMMapper001::DEBUGINFO, CROM::AMPLITUDE,          CROM::SOUNDENABLE,          true,  true },
   /* 002 */ { CROMMapper002::RESET, CROM::HMAPPER,          CROMMapper002::HMAPPER, CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROMMapper002::DEBUGINFO, CROM::AMPLITUDE,          CROM::SOUNDENABLE,          true,  false },
   /* 003 */ { CROMMapper003::RESET, CROM::HMAPPER,          CROMMapper003::HMAPPER, CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROMMapper003::DEBUGINFO, CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, true },
   /* 004 */ { CROMMapper004::RESET, CROM::HMAPPER,          CROMMapper004::HMAPPER, CROM::LMAPPER,          CROM::LMAPPER,          CROMMapper004::SYNCPPU, CROM::SYNCCPU,          CROMMapper004::DEBUGINFO, CROM::AMPLITUDE,          CROM::SOUNDENABLE,          true,  true },
   /* 005 */ { CROMMapper005::RESET, CROMMapper005::HMAPPER, CROM::HMAPPER,          CROMMapper005::LMAPPER, CROMMapper005::LMAPPER, CROMMapper005::SYNCPPU, CROM::SYNCCPU,          CROMMapper005::DEBUGINFO, CROM::AMPLITUDE,          CROM::SOUNDENABLE,          true,  true },
   /* 006 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 007 */ { CROMMapper007::RESET, CROM::HMAPPER,          CROMMapper007::HMAPPER, CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROMMapper007::DEBUGINFO, CROM::AMPLITUDE,          CROM::SOUNDENABLE,          true,  false },
   /* 008 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 009 */ { CROMMapper009::RESET, CROM::HMAPPER,          CROMMapper009::HMAPPER, CROM::LMAPPER,          CROM::LMAPPER,          CROMMapper009::SYNCPPU, CROM::SYNCCPU,          CROMMapper009::DEBUGINFO, CROM::AMPLITUDE,          CROM::SOUNDENABLE,          true,  true },
   /* 010 */ { CROMMapper010::RESET, CROM::HMAPPER,          CROMMapper010::HMAPPER, CROM::LMAPPER,          CROM::LMAPPER,          CROMMapper010::SYNCPPU, CROM::SYNCCPU,          CROMMapper010::DEBUGINFO, CROM::AMPLITUDE,          CROM::SOUNDENABLE,          true,  true },
   /* 011 */ { CROMMapper011::RESET, CROM::HMAPPER,          CROMMapper011::HMAPPER, CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROMMapper011::DEBUGINFO, CROM::AMPLITUDE,          CROM::SOUNDENABLE,          true,  true },
   /* 012 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 013 */ { CROMMapper013::RESET, CROM::HMAPPER,          CROMMapper013::HMAPPER, CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROMMapper013::DEBUGINFO, CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, true },
   /* 014 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 015 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 016 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 017 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 018 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 019 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 020 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 021 */ { CROMMapper021::RESET, CROM::HMAPPER,          CROMMapper021::HMAPPER, CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROMMapper021::SYNCCPU, CROMMapper021::DEBUGINFO, CROM::AMPLITUDE,          CROM::SOUNDENABLE,          true,  true },
   /* 022 */ { CROMMapper022::RESET, CROM::HMAPPER,          CROMMapper022::HMAPPER, CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROMMapper022::DEBUGINFO, CROM::AMPLITUDE,          CROM::SOUNDENABLE,          true,  true },
   /* 023 */ { CROMMapper023::RESET, CROM::HMAPPER,          CROMMapper023::HMAPPER, CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROMMapper023::SYNCCPU, CROMMapper023::DEBUGINFO, CROM::AMPLITUDE,          CROM::SOUNDENABLE,          true,  true },
   /* 024 */ { CROMMapper024::RESET, CROM::HMAPPER,          CROMMapper024::HMAPPER, CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROMMapper024::SYNCCPU, CROMMapper024::DEBUGINFO, CROMMapper024::AMPLITUDE, CROMMapper024::SOUNDENABLE, true,  true },
   /* 025 */ { CROMMapper025::RESET, CROM::HMAPPER,          CROMMapper025::HMAPPER, CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROMMapper025::SYNCCPU, CROMMapper025::DEBUGINFO, CROM::AMPLITUDE,          CROM::SOUNDENABLE,          true,  true },
   /* 026 */ { CROMMapper026::RESET, CROM::HMAPPER,          CROMMapper026::HMAPPER, CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROMMapper026::SYNCCPU, CROMMapper026::DEBUGINFO, CROMMapper024::AMPLITUDE, CROMMapper024::SOUNDENABLE, true,  true },
   /* 027 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 028 */ { CROMMapper028::RESET, CROM::HMAPPER,          CROMMapper028::HMAPPER, CROMMapper028::LMAPPER, CROMMapper028::LMAPPER, CROM::SYNCPPU,          CROM::SYNCCPU,          CROMMapper028::DEBUGINFO, CROM::AMPLITUDE,          CROM::SOUNDENABLE,          true,  true },
   /* 029 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 030 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 031 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 032 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 033 */ { CROMMapper033::RESET, CROM::HMAPPER,          CROMMapper033::HMAPPER, CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROMMapper033::DEBUGINFO, CROM::AMPLITUDE,          CROM::SOUNDENABLE,          true,  true },
   /* 034 */ { CROMMapper034::RESET, CROM::HMAPPER,          CROMMapper034::HMAPPER, CROMMapper034::LMAPPER, CROMMapper034::LMAPPER, CROM::SYNCPPU,          CROM::SYNCCPU,          CROMMapper034::DEBUGINFO, CROM::AMPLITUDE,          CROM::SOUNDENABLE,          true,  true },
   /* 035 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 036 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 037 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 038 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 039 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 040 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 041 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 042 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 043 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 044 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 045 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 046 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 047 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 048 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 049 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 050 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 051 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 052 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 053 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 054 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 055 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 056 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 057 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 058 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 059 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 060 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 061 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 062 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 063 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 064 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 065 */ { CROMMapper065::RESET, CROM::HMAPPER,          CROMMapper065::HMAPPER, CROM::LMAPPER,          CROM::LMAPPER,          CROMMapper065::SYNCPPU, CROM::SYNCCPU,          CROMMapper065::DEBUGINFO, CROM::AMPLITUDE,          CROM::SOUNDENABLE,          true,  true },
   /* 066 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 067 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 068 */ { CROMMapper068::RESET, CROM::HMAPPER,          CROMMapper068::HMAPPER, CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROMMapper068::DEBUGINFO, CROM::AMPLITUDE,          CROM::SOUNDENABLE,          true,  true },
   /* 069 */ { CROMMapper069::RESET, CROM::HMAPPER,          CROMMapper069::HMAPPER, CROMMapper069::LMAPPER, CROMMapper069::LMAPPER, CROM::SYNCPPU,          CROMMapper069::SYNCCPU, CROMMapper069::DEBUGINFO, CROM::AMPLITUDE,          CROM::SOUNDENABLE,          true,  true },
   /* 070 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 071 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 072 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 073 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 074 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
   /* 075 */ { CROMMapper075::RESET, CROM::HMAPPER,          CROMMapper075::HMAPPER, CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROMMapper075::DEBUGINFO, CROM::AMPLITUDE,          CROM::SOUNDENABLE,          true,  true },
   /* */
   /* 255 */ { CROM::RESET,          CROM::HMAPPER,          CROM::HMAPPER,          CROM::LMAPPER,          CROM::LMAPPER,          CROM::SYNCPPU,          CROM::SYNCCPU,          CROM::DEBUGINFO,          CROM::AMPLITUDE,          CROM::SOUNDENABLE,          false, false },
};
