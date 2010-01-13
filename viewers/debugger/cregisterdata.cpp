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
   new CRegisterData(0x00, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x01, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x02, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x03, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x04, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x05, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x06, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x07, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x08, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x09, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x0A, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x0B, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x0C, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x0D, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x0E, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x0F, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x10, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x11, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x12, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x13, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x14, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x15, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x16, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x17, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x18, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x19, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x1A, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x1B, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x1C, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x1D, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x1E, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x1F, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x20, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x21, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x22, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x23, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x24, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x25, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x26, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x27, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x28, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x29, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x2A, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x2B, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x2C, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x2D, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x2E, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x2F, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x30, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x31, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x32, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x33, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x34, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x35, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x36, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x37, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x38, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x39, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x3A, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x3B, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x3C, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x3D, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x3E, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x3F, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x40, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x41, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x42, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x43, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x44, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x45, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x46, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x47, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x48, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x49, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x4A, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x4B, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x4C, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x4D, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x4E, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x4F, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x50, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x51, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x52, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x53, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x54, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x55, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x56, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x57, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x58, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x59, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x5A, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x5B, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x5C, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x5D, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x5E, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x5F, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x60, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x61, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x62, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x63, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x64, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x65, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x66, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x67, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x68, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x69, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x6A, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x6B, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x6C, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x6D, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x6E, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x6F, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x70, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x71, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x72, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x73, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x74, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x75, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x76, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x77, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x78, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x79, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x7A, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x7B, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x7C, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x7D, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x7E, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x7F, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x80, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x81, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x82, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x83, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x84, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x85, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x86, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x87, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x88, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x89, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x8A, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x8B, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x8C, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x8D, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x8E, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x8F, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x90, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x91, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x92, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x93, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x94, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x95, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x96, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x97, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x98, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x99, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x9A, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x9B, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0x9C, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0x9D, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0x9E, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0x9F, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xA0, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xA1, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xA2, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xA3, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xA4, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xA5, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xA6, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xA7, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xA8, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xA9, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xAA, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xAB, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xAC, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xAD, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xAE, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xAF, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xB0, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xB1, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xB2, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xB3, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xB4, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xB5, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xB6, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xB7, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xB8, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xB9, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xBA, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xBB, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xBC, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xBD, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xBE, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xBF, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xC0, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xC1, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xC2, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xC3, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xC4, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xC5, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xC6, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xC7, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xC8, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xC9, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xCA, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xCB, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xCC, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xCD, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xCE, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xCF, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xD0, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xD1, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xD2, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xD3, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xD4, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xD5, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xD6, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xD7, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xD8, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xD9, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xDA, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xDB, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xDC, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xDD, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xDE, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xDF, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xE0, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xE1, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xE2, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xE3, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xE4, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xE5, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xE6, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xE7, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xE8, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xE9, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xEA, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xEB, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xEC, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xED, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xEE, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xEF, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xF0, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xF1, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xF2, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xF3, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xF4, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xF5, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xF6, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xF7, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xF8, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xF9, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xFA, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xFB, "OAMY", 1, tblOAMYBitfields),
   // Sprite
   new CRegisterData(0xFC, "OAMX", 1, tblOAMXBitfields),
   new CRegisterData(0xFD, "OAMPAT", 1, tblOAMPATBitfields),
   new CRegisterData(0xFE, "OAMATT", 4, tblOAMATTBitfields),
   new CRegisterData(0xFF, "OAMY", 1, tblOAMYBitfields)
};
