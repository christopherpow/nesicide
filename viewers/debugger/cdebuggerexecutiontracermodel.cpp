#include "cdebuggerexecutiontracermodel.h"

void GetPrintable ( TracerInfo* pSample, int subItem, char* str );

CDebuggerExecutionTracerModel::CDebuggerExecutionTracerModel(QObject* parent)
{
   m_pTracer = CNES::TRACER();
   m_bShowCPU = true;
   m_bShowPPU = false;
}

CDebuggerExecutionTracerModel::~CDebuggerExecutionTracerModel()
{
}

QVariant CDebuggerExecutionTracerModel::data(const QModelIndex &index, int role) const
{
   char data [ 64 ];

   if (!index.isValid())
      return QVariant();

   if (role != Qt::DisplayRole)
      return QVariant();

   GetPrintable((TracerInfo*)index.internalPointer(), index.column(), data);

   return data;
}

Qt::ItemFlags CDebuggerExecutionTracerModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    return flags;
}

QVariant CDebuggerExecutionTracerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role != Qt::DisplayRole)
      return QVariant();

   if ( orientation == Qt::Horizontal )
   {
      switch ( section )
      {
         case eTracerCol_Cycle:
            return QString("Cycle");
         break;
         case eTracerCol_Source:
            return QString("Source");
         break;
         case eTracerCol_Type:
            return QString("Type");
         break;
         case eTracerCol_Target:
            return QString("Target");
         break;
         case eTracerCol_Addr:
            return QString("Address");
         break;
         case eTracerCol_Data:
            return QString("Data");
         break;
         case eTracerCol_Info:
            return QString("Info");
         break;
         case eTracerCol_CPU_A:
            return QString("A");
         break;
         case eTracerCol_CPU_X:
            return QString("X");
         break;
         case eTracerCol_CPU_Y:
            return QString("Y");
         break;
         case eTracerCol_CPU_SP:
            return QString("SP");
         break;
         case eTracerCol_CPU_F:
            return QString("Flags");
         break;
         case eTracerCol_CPU_EA:
            return QString("Effective Address");
         break;
      }
   }
   else
   {
      return section;
   }

   return  QVariant();
}

QModelIndex CDebuggerExecutionTracerModel::index(int row, int column, const QModelIndex &parent) const
{
   // CPTODO: need to address index for combined tracers
   if ( (m_bShowCPU) && (m_bShowPPU) )
   {
      return createIndex(row, column, m_pTracer->GetSample(row));
   }
   else if ( m_bShowCPU )
   {
      return createIndex(row, column, m_pTracer->GetCPUSample(row));
   }
   else if ( m_bShowPPU )
   {
      return createIndex(row, column, m_pTracer->GetPPUSample(row));
   }
   return QModelIndex();
}

int CDebuggerExecutionTracerModel::rowCount(const QModelIndex &parent) const
{
   int rows = 0;

   if ( m_bShowCPU )
   {
      rows += m_pTracer->GetNumCPUSamples();
   }
   if ( m_bShowPPU )
   {
      rows += m_pTracer->GetNumPPUSamples();
   }
   rows--; // otherwise a repeat of the first row shows up...
   return rows;
}

int CDebuggerExecutionTracerModel::columnCount(const QModelIndex &parent) const
{
   if ( parent.isValid())
   {
      return 0;
   }
   return 13;
}

void CDebuggerExecutionTracerModel::layoutChangedEvent()
{
    this->layoutChanged();
}

void GetPrintable ( TracerInfo* pSample, int subItem, char* str )
{
   if ( pSample )
   {
      switch ( subItem )
      {
         case eTracerCol_Cycle:
            sprintf ( str, "%u", pSample->cycle );
         break;
         case eTracerCol_Source:
            switch ( pSample->source )
            {
               case eSource_CPU:
                  strcpy ( str, "CPU" );
               break;
               case eSource_PPU:
                  strcpy ( str, "PPU" );
               break;
               case eSource_APU:
                  strcpy ( str, "APU" );
               break;
               case eSource_Mapper:
                  strcpy ( str, "Mapper" );
               break;
               case eSource_User:
                  strcpy ( str, "User" );
               break;
            }
         break;
         case eTracerCol_Type:
            switch ( pSample->type )
            {
               case eTracer_Unknown:
                  str[0] = 0;
               break;
               case eTracer_InstructionFetch:
                  strcpy ( str, "Instruction Fetch" );
               break;
               case eTracer_DataRead:
                  strcpy ( str, "Memory Read" );
               break;
               case eTracer_DataWrite:
                  strcpy ( str, "Memory Write" );
               break;
               case eTracer_DMA:
                  strcpy ( str, "DMA" );
               break;
               case eTracer_RESET:
                  strcpy ( str, "RESET" );
               break;
               case eTracer_NMI:
                  strcpy ( str, "NMI" );
               break;
               case eTracer_IRQ:
                  strcpy ( str, "IRQ" );
               break;
               case eTracer_GarbageRead:
                  strcpy ( str, "Garbage Fetch" );
               break;
               case eTracer_RenderBkgnd:
                  strcpy ( str, "Playfield Render" );
               break;
               case eTracer_RenderSprite:
                  strcpy ( str, "Sprite Render" );
               break;
               case eTracer_Sprite0Hit:
                  strcpy ( str, "Sprite 0 Hit" );
               break;
            }
         break;
         case eTracerCol_Target:
            switch ( pSample->target )
            {
               case eTarget_Unknown:
                  str[0] = 0;
               break;
               case eTarget_RAM:
                  strcpy ( str, "RAM" );
               break;
               case eTarget_PPURegister:
                  strcpy ( str, "PPU Register" );
               break;
               case eTarget_APURegister:
                  strcpy ( str, "APU Register" );
               break;
               case eTarget_IORegister:
                  strcpy ( str, "I/O Register" );
               break;
               case eTarget_SRAM:
                  strcpy ( str, "SRAM" );
               break;
               case eTarget_ROM:
                  strcpy ( str, "ROM" );
               break;
               case eTarget_Mapper:
                  strcpy ( str, "Mapper" );
               break;
               case eTarget_PatternMemory:
                  strcpy ( str, "Pattern Memory" );
               break;
               case eTarget_NameTable:
                  strcpy ( str, "NameTable RAM" );
               break;
               case eTarget_AttributeTable:
                  strcpy ( str, "AttributeTable RAM" );
               break;
               case eTarget_Palette:
                  strcpy ( str, "Palette RAM" );
               break;
            }
         break;
         case eTracerCol_Addr:
            if ( pSample->type == eTracer_GarbageRead )
            {
               strcpy ( str, "$XXXX" );
            }
            else if ( (pSample->type == eTracer_RESET) ||
                      (pSample->type == eTracer_NMI) ||
                      (pSample->type == eTracer_IRQ) )
            {
               str[0] = 0;
            }
            else
            {
               sprintf ( str, "$%04X", pSample->addr );
            }
         break;
         case eTracerCol_CPU_EA:
            if ( pSample->ea == 0xFFFFFFFF )
            {
               str[0] = 0;
            }
            else
            {
               sprintf ( str, "$%04X", pSample->ea );
            }
         break;
         case eTracerCol_Data:
            if ( pSample->type == eTracer_GarbageRead )
            {
               strcpy ( str, "$XX" );
            }
            else if ( (pSample->type == eTracer_RESET) ||
                      (pSample->type == eTracer_NMI) ||
                      (pSample->type == eTracer_IRQ) )
            {
               str[0] = 0;
            }
            else
            {
               sprintf ( str, "$%02X", pSample->data );
            }
         break;
         case eTracerCol_CPU_A:
            if ( pSample->regsset )
            {
               sprintf ( str, "$%02X", pSample->a );
            }
            else
            {
               str[0] = 0;
            }
         break;
         case eTracerCol_CPU_X:
            if ( pSample->regsset )
            {
               sprintf ( str, "$%02X", pSample->x );
            }
            else
            {
               str[0] = 0;
            }
         break;
         case eTracerCol_CPU_Y:
            if ( pSample->regsset )
            {
               sprintf ( str, "$%02X", pSample->y );
            }
            else
            {
               str[0] = 0;
            }
         break;
         case eTracerCol_CPU_SP:
            if ( pSample->regsset )
            {
               sprintf ( str, "$%02X", pSample->sp );
            }
            else
            {
               str[0] = 0;
            }
         break;
         case eTracerCol_CPU_F:
            if ( pSample->regsset )
            {
               sprintf ( str, "$%02X", pSample->f );
            }
            else
            {
               str[0] = 0;
            }
         break;
         case eTracerCol_Info:
            if ( (*(pSample->disassemble+3)) == 0x00 )
            {
               // Extra byte indicates whether an instruction should be decoded...
               C6502::Disassemble ( pSample->disassemble, str );
            }
            else
            {
               str[0] = 0;
            }
         break;
      }
   }
}
