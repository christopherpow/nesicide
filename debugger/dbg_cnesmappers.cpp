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

#include "dbg_cnesmappers.h"

// NOTE: Be sure to update all the inspectorPage values for all mapper name
//       variants listed for a given mapper ID.

MapperInfo mapper[] =
{
   { "NES-NROM", 0, 0 },
   { "NROM", 0, 0 },
   { "MMC1", 1, 1 },
   { "NES-MMC1", 1, 1 },
   { "NES-S*ROM", 1, 1 },
   { "NES-SAROM", 1, 1 },
   { "NES-SBROM", 1, 1 },
   { "NES-SCROM", 1, 1 },
   { "NES-SEROM", 1, 1 },
   { "NES-SGROM", 1, 1 },
   { "NES-SKROM", 1, 1 },
   { "NES-SL1ROM", 1, 1 },
   { "NES-SLROM", 1, 1 },
   { "NES-SNROM", 1, 1 },
   { "NES-SOROM", 1, 1 },
   { "S*ROM", 1, 1 },
   { "SAROM", 1, 1 },
   { "SBROM", 1, 1 },
   { "SCROM", 1, 1 },
   { "SEROM", 1, 1 },
   { "SGROM", 1, 1 },
   { "SKROM", 1, 1 },
   { "SL1ROM", 1, 1 },
   { "SLROM", 1, 1 },
   { "SNROM", 1, 1 },
   { "SOROM", 1, 1 },
   { "U*ROM", 2, 0 },
   { "UNROM", 2, 0 },
   { "UOROM", 2, 0 },
   { "NES-U*ROM", 2, 0 },
   { "NES-UNROM", 2, 0 },
   { "NES-UOROM", 2, 0 },
   { "CNROM", 3, 0 },
   { "NES-CNROM", 3, 0 },
   { "MMC3", 4, 2 },
   { "MMC6", 4, 2 },
   { "H*ROM", 4, 2 },
   { "HKROM", 4, 2 },
   { "NES-B4", 4, 2 },
   { "NES-H*ROM", 4, 2 },
   { "NES-HKROM", 4, 2 },
   { "NES-MMC3", 4, 2 },
   { "NES-MMC6", 4, 2 },
   { "NES-NES-B4", 4, 2 },
   { "NES-T*ROM", 4, 2 },
   { "NES-TFROM", 4, 2 },
   { "NES-TGROM", 4, 2 },
   { "NES-TKROM", 4, 2 },
   { "NES-TLROM", 4, 2 },
   { "NES-TR1ROM", 4, 2 },
   { "NES-TSROM", 4, 2 },
   { "T*ROM", 4, 2 },
   { "TFROM", 4, 2 },
   { "TGROM", 4, 2 },
   { "TKROM", 4, 2 },
   { "TLROM", 4, 2 },
   { "TR1ROM", 4, 2 },
   { "TSROM", 4, 2 },
   { "MMC5", 5, 0 },
   { "E*ROM", 5, 0 },
   { "EKROM", 5, 0 },
   { "ELROM", 5, 0 },
   { "ETROM", 5, 0 },
   { "EWROM", 5, 0 },
   { "NES-E*ROM", 5, 0 },
   { "NES-EKROM", 5, 0 },
   { "NES-ELROM", 5, 0 },
   { "NES-ETROM", 5, 0 },
   { "NES-EWROM", 5, 0 },
   { "NES-MMC5", 5, 0 },
   { "FFE F4", 6, 0 },
   { "A*ROM", 7, 0 },
   { "AMROM", 7, 0 },
   { "ANROM", 7, 0 },
   { "AOROM", 7, 0 },
   { "NES-A*ROM", 7, 0 },
   { "NES-AMROM", 7, 0 },
   { "NES-ANROM", 7, 0 },
   { "NES-AOROM", 7, 0 },
   { "FFE F3", 8, 0 },
   { "MMC2", 9, 0 },
   { "NES-MMC2", 9, 0 },
   { "NES-P*ROM", 9, 0 },
   { "NES-PEEOROM", 9, 0 },
   { "NES-PNROM", 9, 0 },
   { "P*ROM", 9, 0 },
   { "PEEOROM", 9, 0 },
   { "PNROM", 9, 0 },
   { "MMC4", 10, 0 },
   { "NES-MMC4", 10, 0 },
   { "Color Dreams", 11, 0 },
   { "CPROM", 13, 0 },
   { "100-in-1 Contra Function 16", 15, 0 },
   { "Bandai", 16, 0 },
   { "FFE F8", 17, 0 },
   { "Jaleco SS8806", 18, 0 },
   { "Namcot 106", 19, 0 },
   { "Konami VRC4", 21, 0 },
   { "VRC4", 21, 0 },
   { "Konami VRC2 A", 22, 0 },
   { "Konami VRC2 Type A", 22, 0 },
   { "VRC2 A", 22, 0 },
   { "Konami VRC2 B", 23, 0 },
   { "Konami VRC2 Type B", 23, 0 },
   { "VRC2 B", 23, 0 },
   { "Konami VRC6 A1/A0", 24, 0 },
   { "VRC6 A1/A0", 24, 0 },
   { "Konami VRC4 Y", 25, 0 },
   { "Konami VRC4 Type Y", 25, 0 },
   { "VRC4 Y", 25, 0 },
   { "Irem G-101", 32, 0 },
   { "Taito TC0190", 33, 0 },
   { "TC0190", 33, 0 },
   { "BNROM", 34, 0 },
   { "NES-BNROM", 34, 0 },
   { "Nina-01", 34, 0 },
   { "SMB2j Pirate", 40, 0 },
   { "Caltron 6-in-1", 41, 0 },
   { "Mario Baby", 42, 0 },
   { "SMB2j (LF36)", 43, 0 },
   { "Super HiK 7 in 1 (MMC3)", 44, 0 },
   { "Super 1,000,000 in 1 (MMC3)", 45, 0 },
   { "GameStation/RumbleStation", 46, 0 },
   { "Super Spike & Nintendo World Cup Soccer (MMC3)", 47, 0 },
   { "Super Spike/World Cup", 47, 0 },
   { "1993 Super HiK 4-in-1 (MMC3)", 49, 0 },
   { "SMB2j rev. A", 50, 0 },
   { "11 in 1 Ball Games", 51, 0 },
   { "Mario 7 in 1 (MMC3)", 52, 0 },
   { "SMB3 Pirate", 56, 0 },
   { "Study & Game 32 in 1", 58, 0 },
   { "T3H53", 59, 0 },
   { "20-in-1", 61, 0 },
   { "700-in-1", 62, 0 },
   { "Hello Kitty 255 in 1", 63, 0 },
   { "Tengen RAMBO-1", 64, 0 },
   { "RAMBO-1", 64, 0 },
   { "Irem H-3001", 65, 0 },
   { "GNROM", 66, 0 },
   { "NES-GNROM", 66, 0 },
   { "Sunsoft 3", 67, 0 },
   { "Sunsoft Mapper #3", 67, 0 },
   { "Sunsoft 4", 68, 0 },
   { "Sunsoft Mapper #4", 68, 0 },
   { "Sunsoft FME-07", 69, 3 },
   { "FME-07", 69, 3 },
   { "Camerica (partial)", 71, 0 },
   { "Konami VRC3", 73, 0 },
   { "VRC3", 73, 0 },
   { "Konami VRC1", 75, 0 },
   { "VRC1", 75, 0 },
   { "Irem 74161/32", 78, 0 },
   { "NINA-03", 79, 0 },
   { "NINA-06", 79, 0 },
   { "Cony", 83, 0 },
   { "Konami VRC7", 85, 0 },
   { "VRC7", 85, 0 },
   { "Mapper 90", 90, 0 },
   { "Super Mario World", 90, 0 },
   { "PC-HK-SF3", 91, 0 },
   { "Dragon Buster (MMC3 variant)", 95, 0 },
   { "Dragon Buster", 95, 0 },
   { "Kid Niki (J)", 97, 0 },
   { "Nintendo VS Unisystem", 99, 0 },
   { "VS Unisystem", 99, 0 },
   { "Debugging Mapper", 100, 0 },
   { "Nintendo World Championship", 105, 0 },
   { "HES-Mapper #113", 113, 0 },
   { "TKSROM", 118, 0 },
   { "TLSROM", 118, 0 },
   { "NES-TKSROM", 118, 0 },
   { "NES-TLSROM", 118, 0 },
   { "TQROM", 119, 0 },
   { "NES-TQROM", 119, 0 },
   { "Sachen Mapper 141", 141, 0 },
   { "SMB2j Pirate (KS 202)", 142, 0 },
   { "Sachen Copy Protection", 143, 0 },
   { "AGCI", 144, 0 },
   { "Nintendo VS Unisystem (Extended)", 151, 0 },
   { "Super Donkey Kong", 182, 0 },
   { "72-in-1", 225, 0 },
   { "76-in-1", 226, 0 },
   { "1200-in-1", 227, 0 },
   { "Action 52", 228, 0 },
   { "31-in-1", 229, 0 },
   { "Camerica 9096", 232, 0 },
   { "Maxi 15", 234, 0 },
   { "Golden Game 150-in-1", 235, 0 },
   { "Sachen 74LS374N", 243, 0 },
   { NULL, 256, 0 },
   { "?", 256, 0 } // Unknown index, not parsed typically when going through the list...
};

int mapperIndexFromID ( int32_t id )
{
   int32_t i = 0;

   // Return the name associated with the first
   // match of mapper ID in the list above.
   while ( mapper[i].name )
   {
      if ( mapper[i].id == id )
      {
         break;
      }

      i++;
   }

   if ( !mapper[i].name )
   {
      // Move index to the unknown...
      i++;
   }

   return i;
}

int mapperIDFromIndex ( int32_t index )
{
   return mapper[index].id;
}

const char* mapperNameFromID ( int32_t id )
{
   int32_t i = 0;

   // Return the name associated with the first
   // match of mapper ID in the list above.
   while ( mapper[i].name )
   {
      if ( mapper[i].id == id )
      {
         break;
      }

      i++;
   }

   if ( !mapper[i].name )
   {
      // Move index to the unknown...
      i++;
   }

   return mapper[i].name;
}

int32_t mapperInspectorPageFromID ( int32_t id )
{
   int32_t i = 0;

   // Return the name associated with the first
   // match of mapper ID in the list above.
   while ( mapper[i].name )
   {
      if ( mapper[i].id == id )
      {
         break;
      }

      i++;
   }

   if ( !mapper[i].name )
   {
      // Move index to the unknown...
      i++;
   }

   return mapper[i].inspectorPage;
}
