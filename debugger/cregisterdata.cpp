#include "cregisterdata.h"

// OAM 'Registers'
CBitfieldData* tblOAMYBitfields [] =
{
   new CBitfieldData("Y Position", 0, 8, "%02X", 0),
};

CBitfieldData* tblOAMPATBitfields [] =
{
   new CBitfieldData("Tile Index", 0, 8, "%02X", 0),
};

CBitfieldData* tblOAMATTBitfields [] =
{
   new CBitfieldData("Flip Vertical", 7, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Flip Horizontal", 6, 1, "%X", 2, "No", "Yes"),
   new CBitfieldData("Priority", 5, 1, "%X", 2, "In-front of Playfield", "Behind Playfield"),
   new CBitfieldData("Palette Index", 0, 2, "%X", 4, "$3F10", "$3F14", "$3F18", "$3F1C")
};

CBitfieldData* tblOAMXBitfields [] =
{
   new CBitfieldData("X Position", 0, 8, "%02X", 0),
};

CRegisterData* tblOAMRegisters [] =
{
   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields),

   // Sprite
   new CRegisterData(0x00, "OAMY", 1, tblOAMYBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMX", 1, tblOAMXBitfields)
};
