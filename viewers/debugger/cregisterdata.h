#ifndef CREGISTERDATA_H
#define CREGISTERDATA_H

#include "stdarg.h"

class CBitfieldData
{
public:
   CBitfieldData(const char* name, char start, char width, const char* displayFormat, int values, ... )
   {
      int idx;
      m_name = name;
      m_values = values;
      m_start = start;
      m_width = width;
      m_displayFormat = displayFormat;
      if ( values )
      {
         va_list va;

         va_start ( va, values );
         m_value = new char* [ values ];
         for ( idx = 0; idx < values; idx++ )
         {
            m_value[idx] = (char*)va_arg(va,const char*);
         }
         va_end ( va );
      }
   }
   virtual ~CBitfieldData()
   {
      delete m_name;
      delete [] m_value;
   }
   int GetNumValues ( void ) const { return m_values; }
   const char* GetName ( void ) const { return m_name; }
   const char* GetDisplayFormat ( void ) const { return m_displayFormat; }
   char* GetValue ( int data ) const
   {
      data >>= m_start;
      data &= ((1<<m_width)-1);
      return m_value[data];
   }
   int GetValueRaw ( int data ) const
   {
      data >>= m_start;
      data &= ((1<<m_width)-1);
      return data;
   }
   char* GetValueByIndex ( int index ) const
   {
      return m_value[index];
   }
   int InsertValue ( int data, int bitsToInsert )
   {
      int mask = ((1<<m_width)-1);
      mask <<= m_start;
      data &= (~(mask));
      bitsToInsert <<= m_start;
      data |= bitsToInsert;
      return data;
   }

protected:
   const char* m_name;
   const char* m_displayFormat;
   char  m_start;
   char  m_width;
   char** m_value;
   int   m_values;
};

class CRegisterData
{
public:
   CRegisterData(int addr, const char* name, int fields, CBitfieldData** tblField )
   {
      m_addr = addr;
      m_name = name;
      m_fields = fields;
      m_field = tblField;
   }
   virtual ~CRegisterData () {};
   const char* GetName ( void ) const { return m_name; }
   int GetNumBitfields ( void ) const { return m_fields; }
   CBitfieldData* GetBitfield ( int field ) const { return m_field[field]; }
   int GetAddr ( void ) const { return m_addr; }
protected:
   int m_addr;
   const char* m_name;
   char m_fields;
   CBitfieldData** m_field;
};

extern CRegisterData* tblCPURegisters [];
extern CRegisterData* tblPPURegisters [];
extern CRegisterData* tblAPURegisters [];
extern CRegisterData* tblOAMRegisters [];

#endif // CREGISTERDATA_H
