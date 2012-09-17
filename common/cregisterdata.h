#ifndef CREGISTERDATA_H
#define CREGISTERDATA_H

#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "stdint.h"

#define eMemory_REGISTER_BASE 0x2000 // Must be unique from eMemory_MEMORY_BASE in cmemorydata.h
#define eMemory_CPUregs (eMemory_REGISTER_BASE+0)
#define eMemory_PPUregs (eMemory_REGISTER_BASE+1)
#define eMemory_PPUoam (eMemory_REGISTER_BASE+2)
#define eMemory_IOregs (eMemory_REGISTER_BASE+3)
#define eMemory_cartMapper (eMemory_REGISTER_BASE+4)
#define eMemory_SIDregs (eMemory_REGISTER_BASE+5)

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
      m_lsb = start;
      m_msb = start+width-1;

      if ( values )
      {
         va_list va;

         va_start ( va, values );
         m_value = new char* [ values ];

         for ( idx = 0; idx < values; idx++ )
         {
            char* temp = (char*)va_arg(va,const char*);
            m_value[idx] = new char [ strlen(temp)+6 ]; // account for raw addition
            sprintf ( m_value[idx], "%X: %s", idx, temp );
         }

         va_end ( va );
      }
   }
   virtual ~CBitfieldData()
   {
      delete m_name;
      delete [] m_value;
   }
   char GetWidth ( void ) const
   {
      return m_width;
   }
   char GetLsb ( void ) const
   {
      return m_lsb;
   }
   char GetMsb ( void ) const
   {
      return m_msb;
   }
   int GetNumValues ( void ) const
   {
      return m_values;
   }
   const char* GetName ( void ) const
   {
      return m_name;
   }
   const char* GetDisplayFormat ( void ) const
   {
      return m_displayFormat;
   }
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
   char  m_lsb;
   char  m_msb;
   char** m_value;
   int   m_values;
};

typedef void (*setRegFunc)(uint32_t,uint32_t);
typedef uint32_t (*getRegFunc)(uint32_t);

class CRegisterData
{
public:
   CRegisterData(int addr, const char* name, getRegFunc get, setRegFunc set, int fields, CBitfieldData** tblField )
   {
      m_addr = addr;
      m_name = name;
      m_fields = fields;
      m_field = tblField;
      m_get = get;
      m_set = set;
   }
   virtual ~CRegisterData () {};
   const char* GetName ( void ) const
   {
      return m_name;
   }
   int GetNumBitfields ( void ) const
   {
      return m_fields;
   }
   CBitfieldData* GetBitfield ( int field ) const
   {
      return m_field[field];
   }
   int GetAddr ( void ) const
   {
      return m_addr;
   }
   void Set ( uint32_t data ) { m_set(m_addr,data); }
   uint32_t Get () { return m_get(m_addr); }

protected:
   int m_addr;
   const char* m_name;
   int m_fields;
   CBitfieldData** m_field;
   getRegFunc m_get;
   setRegFunc m_set;
};

class CRegisterDatabase
{
public:
   CRegisterDatabase(int type,int rows,int columns,int registers,CRegisterData** tblRegisters,const char** rowHeadings,const char** columnHeadings )
   {
      m_type = type;
      m_register = tblRegisters;
      m_registers = registers;
      m_rows = rows;
      m_columns = columns;
      m_rowHeadings = rowHeadings;
      m_columnHeadings = columnHeadings;
   }

   int GetType() { return m_type; }
   bool Contains(int addr)
   {
      for ( int idx = 0; idx < m_registers; idx++ )
      {
         if ( m_register[idx]->GetAddr() == addr )
         {
            return true;
         }
      }
      return false;
   }
   CRegisterData** GetRegisters() { return m_register; }
   CRegisterData* GetRegister(int idx) { return m_register[idx]; }
   int GetRegisterAt(int addr)
   {
      for ( int idx = 0; idx < m_registers; idx++ )
      {
         if ( m_register[idx]->GetAddr() == addr )
         {
            return idx;
         }
      }
      return -1;
   }
   int GetNumRegisters() { return m_registers; }
   int GetNumRows() { return m_rows; }
   int GetNumColumns() { return m_columns; }
   const char* GetRowHeading(int idx) { return m_rowHeadings[idx]; }
   const char* GetColumnHeading(int idx) { return m_columnHeadings[idx]; }

protected:
   int m_type;
   CRegisterData** m_register;
   int m_registers;
   int m_rows;
   int m_columns;
   const char** m_rowHeadings;
   const char** m_columnHeadings;
};

typedef CRegisterDatabase* (*regDBFunc)();

#endif // CREGISTERDATA_H
