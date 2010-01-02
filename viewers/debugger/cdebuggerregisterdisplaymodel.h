#ifndef CDEBUGGERREGISTERDISPLAYMODEL_H
#define CDEBUGGERREGISTERDISPLAYMODEL_H

#include <QAbstractItemModel>
#include "cnes6502.h"
#include "cnesppu.h"
#include "cnesapu.h"

#include "stdarg.h"

#include "cnesicidecommon.h"

class CBitfieldData
{
public:
   CBitfieldData(const char* name, char start, char width, int values, ... )
   {
      int idx;
      m_name = name;
      m_values = values;
      m_start = start;
      m_width = width;
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
   char* GetValue ( int data ) const
   {
      data >>= m_start;
      data &= ((1<<m_width)-1);
      return m_value[data];
   }
   char GetValueRaw ( int data ) const
   {
      data >>= m_start;
      data &= ((1<<m_width)-1);
      return data;
   }
protected:
   const char* m_name;
   char  m_start;
   char  m_width;
   char** m_value;
   int   m_values;
};

class CRegisterData
{
public:
   CRegisterData(const char* name, int fields, CBitfieldData** tblField )
   {
      m_name = name;
      m_fields = fields;
      m_field = tblField;
   }
   virtual ~CRegisterData () {};
   const char* GetName ( void ) const { return m_name; }
   int GetNumBitfields ( void ) const { return m_fields; }
   CBitfieldData* GetBitfield ( int field ) const { return m_field[field]; }
protected:
   const char* m_name;
   char m_fields;
   CBitfieldData** m_field;
};

class CDebuggerRegisterDisplayModel : public QAbstractTableModel
{
public:
   CDebuggerRegisterDisplayModel(QObject* parent = 0, eMemoryType display = eMemory_PPUregs);
   virtual ~CDebuggerRegisterDisplayModel();
   QVariant data(const QModelIndex &index, int role) const;
   Qt::ItemFlags flags(const QModelIndex &index) const;
   bool setData ( const QModelIndex & index, const QVariant & value, int role);
   QVariant headerData(int section, Qt::Orientation orientation,
                     int role = Qt::DisplayRole) const;
   QModelIndex index(int row, int column,
                   const QModelIndex &parent = QModelIndex()) const;
   int rowCount(const QModelIndex &parent = QModelIndex()) const;
   int columnCount(const QModelIndex &parent = QModelIndex()) const;
   void setRegister ( int newRegister ) { m_register = newRegister; layoutChangedEvent(); }
   void layoutChangedEvent();
private:
   eMemoryType m_display;
   int         m_register;
   CRegisterData** m_tblRegisters;
   unsigned int m_offset;
};

#endif // CDEBUGGERREGISTERDISPLAYMODEL_H
