#include "cdebuggerregisterdisplaymodel.h"

CBitfieldData* tblPPUCTRLBitfields [] =
{
   new CBitfieldData("NameTable", 0, 2, 4, "NT1", "NT2", "NT3", "NT4"),
   new CBitfieldData("VRAM Address Increment", 2, 1, 2, "+1", "+32"),
   new CBitfieldData("Sprite Pattern Table", 3, 1, 2, "$0000", "$1000"),
   new CBitfieldData("Playfield Pattern Table", 4, 1, 2, "$0000", "$1000"),
   new CBitfieldData("Sprite Size", 5, 1, 2, "8x8", "8x16"),
   new CBitfieldData("PPU Master/Slave", 6, 1, 2, "Master", "Slave"),
   new CBitfieldData("Generate NMI", 7, 1, 2, "No", "Yes")
};

CBitfieldData* tblPPUMASKBitfields [] =
{
   new CBitfieldData("Greyscale", 0, 1, 2, "No", "Yes"),
   new CBitfieldData("Playfield Clipping", 1, 1, 2, "Yes", "No"),
   new CBitfieldData("Sprite Clipping", 2, 1, 2, "Yes", "No"),
   new CBitfieldData("Playfield Rendering", 3, 1, 2, "Off", "On"),
   new CBitfieldData("Sprite Rendering", 4, 1, 2, "Off", "On"),
   new CBitfieldData("Red Emphasis", 5, 1, 2, "Off", "On"),
   new CBitfieldData("Green Emphasis", 6, 1, 2, "Off", "On"),
   new CBitfieldData("Blue Emphasis", 7, 1, 2, "Off", "On")
};

CBitfieldData* tblPPUSTATUSBitfields [] =
{
   new CBitfieldData("Sprite Overflow", 5, 1, 2, "No", "Yes"),
   new CBitfieldData("Sprite 0 Hit", 6, 1, 2, "No", "Yes"),
   new CBitfieldData("Vertical Blank", 7, 1, 2, "No", "Yes")
};

CBitfieldData* tblOAMADDRBitfields [] =
{
   new CBitfieldData("OAM Address", 0, 8, 0)
};

CBitfieldData* tblOAMDATABitfields [] =
{
   new CBitfieldData("OAM Data", 0, 8, 0)
};

CBitfieldData* tblPPUSCROLLBitfields [] =
{
   new CBitfieldData("PPU Scroll", 0, 8, 0)
};

CBitfieldData* tblPPUADDRBitfields [] =
{
   new CBitfieldData("PPU Address", 0, 8, 0)
};

CBitfieldData* tblPPUDATABitfields [] =
{
   new CBitfieldData("PPU Data", 0, 8, 0)
};

CRegisterData* tblPPURegisters [] =
{
   new CRegisterData("PPUCTRL", 7, tblPPUCTRLBitfields),
   new CRegisterData("PPUMASK", 8, tblPPUMASKBitfields),
   new CRegisterData("PPUSTATUS", 3, tblPPUSTATUSBitfields),
   new CRegisterData("OAMADDR", 1, tblOAMADDRBitfields),
   new CRegisterData("OAMDATA", 1, tblOAMDATABitfields),
   new CRegisterData("PPUSCROLL", 1, tblPPUSCROLLBitfields),
   new CRegisterData("PPUADDR", 1, tblPPUADDRBitfields),
   new CRegisterData("PPUDATA", 1, tblPPUDATABitfields)
};

CBitfieldData* tblAPUSQCTRLBitfields [] =
{
   new CBitfieldData("Volume/Envelope", 0, 4, 0),
   new CBitfieldData("Envelope Enabled", 4, 1, 2, "No", "Yes"),
   new CBitfieldData("Channel State", 5, 1, 2, "Running", "Halted"),
   new CBitfieldData("Duty Cycle", 6, 2, 4, "25%", "50%", "75%", "12.5%")
};

CBitfieldData* tblAPUSQSWEEPBitfields [] =
{
   new CBitfieldData("Sweep Enabled", 7, 1, 2, "No", "Yes"),
   new CBitfieldData("Sweep Divider", 4, 3, 0),
   new CBitfieldData("Sweep Direction", 3, 1, 2, "Down", "Up"),
   new CBitfieldData("Sweep Shift", 0, 3, 0)
};

CBitfieldData* tblAPUPERIODLOBitfields [] =
{
   new CBitfieldData("Period Low-bits", 0, 8, 0)
};

CBitfieldData* tblAPUPERIODLENBitfields [] =
{
   new CBitfieldData("Period High-bits", 0, 3, 0),
   new CBitfieldData("Length", 3, 5, 32, "$0A","$FE","$14","$02","$28","$04","$50","$06","$A0","$08","$3C","$0A","$0E","$0C","$1A","$0E","$0C","$10","$18","$12","$30","$14","$60","$16","$C0","$18","$48","$1A","$10","$1C","$20","$1E")
};

CBitfieldData* tblAPUTRICTRLBitfields [] =
{
   new CBitfieldData("Channel State", 7, 1, 2, "Running", "Halted"),
   new CBitfieldData("Linear Counter Reload", 0, 6, 0)
};

CBitfieldData* tblAPUDMZBitfields [] =
{
   new CBitfieldData("Unused", 0, 8, 0),
};

CBitfieldData* tblAPUNZCTRLBitfields [] =
{
   new CBitfieldData("Volume/Envelope", 0, 4, 0),
   new CBitfieldData("Envelope Enabled", 4, 1, 2, "No", "Yes"),
   new CBitfieldData("Channel State", 5, 1, 2, "Running", "Halted"),
};

CBitfieldData* tblAPUNZPERIODBitfields [] =
{
   new CBitfieldData("Period", 0, 4, 16, "$004","$008","$010","$020","$040","$060","$080","$0A0","$0CA","$0FE","$17C","$1FC","$2FA","$3F8","$7F2","$FE4"),
   new CBitfieldData("Mode", 7, 1, 2, "32,767 samples", "93 samples")
};

CBitfieldData* tblAPUNZLENBitfields [] =
{
   new CBitfieldData("Length", 3, 5, 32, "$0A","$FE","$14","$02","$28","$04","$50","$06","$A0","$08","$3C","$0A","$0E","$0C","$1A","$0E","$0C","$10","$18","$12","$30","$14","$60","$16","$C0","$18","$48","$1A","$10","$1C","$20","$1E")
};

CBitfieldData* tblAPUDMCCTRLBitfields [] =
{
   new CBitfieldData("Period", 0, 4, 16, "$1AC","$17C","$154","$140","$11E","$0FE","$0E2","$0D6","$0BE","$0A0","$08E","$080","$06A","$054","$048","$036"),
   new CBitfieldData("Loop", 6, 1, 2, "No", "Yes"),
   new CBitfieldData("IRQ Enabled", 7, 1, 2, "No", "Yes")
};

CBitfieldData* tblAPUDMCVOLBitfields [] =
{
   new CBitfieldData("Volume", 0, 7, 0)
};

CBitfieldData* tblAPUDMCADDRBitfields [] =
{
   new CBitfieldData("Sample Address", 0, 8, 0)
};

CBitfieldData* tblAPUDMCLENBitfields [] =
{
   new CBitfieldData("Sample Length", 0, 8, 0)
};

CBitfieldData* tblAPUCTRLBitfields [] =
{
   new CBitfieldData("Square1 Channel", 0, 1, 2, "Disabled", "Enabled"),
   new CBitfieldData("Square2 Channel", 1, 1, 2, "Disabled", "Enabled"),
   new CBitfieldData("Triangle Channel", 2, 1, 2, "Disabled", "Enabled"),
   new CBitfieldData("Noise Channel", 3, 1, 2, "Disabled", "Enabled"),
   new CBitfieldData("Delta Modulation Channel", 4, 1, 2, "Disabled", "Enabled"),
   new CBitfieldData("APU IRQ", 6, 1, 2, "Not Asserted", "Asserted"),
   new CBitfieldData("DMC IRQ", 7, 1, 2, "Not Asserted", "Asserted")
};

CBitfieldData* tblAPUMASKBitfields [] =
{
   new CBitfieldData("IRQ", 7, 1, 2, "Disabled", "Enabled"),
   new CBitfieldData("Sequencer Mode", 6, 1, 2, "4-step", "5-step")
};

CRegisterData* tblAPURegisters [] =
{
   new CRegisterData("SQ1CTRL", 4, tblAPUSQCTRLBitfields),
   new CRegisterData("SQ1SWEEP", 4, tblAPUSQSWEEPBitfields),
   new CRegisterData("SQ1PERLO", 1, tblAPUPERIODLOBitfields),
   new CRegisterData("SQ1PERLEN", 2, tblAPUPERIODLENBitfields),
   new CRegisterData("SQ2CTRL", 4, tblAPUSQCTRLBitfields),
   new CRegisterData("SQ2SWEEP", 4, tblAPUSQSWEEPBitfields),
   new CRegisterData("SQ2PERLO", 1, tblAPUPERIODLOBitfields),
   new CRegisterData("SQ2PERLEN", 2, tblAPUPERIODLENBitfields),
   new CRegisterData("TRICTRL", 2, tblAPUTRICTRLBitfields),
   new CRegisterData("TRIDMZ", 1, tblAPUDMZBitfields),
   new CRegisterData("TRIPERLO", 1, tblAPUPERIODLOBitfields),
   new CRegisterData("TRIPERLEN", 2, tblAPUPERIODLENBitfields),
   new CRegisterData("NOISECTRL", 3, tblAPUNZCTRLBitfields),
   new CRegisterData("NOISEDMZ", 1, tblAPUDMZBitfields),
   new CRegisterData("NOISEPERIOD", 2, tblAPUNZPERIODBitfields),
   new CRegisterData("NOISELEN", 1, tblAPUNZLENBitfields),
   new CRegisterData("DMCCTRL", 3, tblAPUDMCCTRLBitfields),
   new CRegisterData("DMCVOL", 1, tblAPUDMCVOLBitfields),
   new CRegisterData("DMCADDR", 1, tblAPUDMCADDRBitfields),
   new CRegisterData("DMCLEN", 1, tblAPUDMCLENBitfields),
   new CRegisterData("APUDMZ", 1, tblAPUDMZBitfields),
   new CRegisterData("APUCTRL", 7, tblAPUCTRLBitfields),
   new CRegisterData("APUDMZ", 1, tblAPUDMZBitfields),
   new CRegisterData("APUMASK", 2, tblAPUMASKBitfields),
};

CDebuggerRegisterDisplayModel::CDebuggerRegisterDisplayModel(QObject* parent, eMemoryType display)
{
   m_display = display;
   m_register = 0;
   switch ( m_display )
   {
      case eMemory_PPUregs:
         m_offset = 0x2000;
         m_tblRegisters = tblPPURegisters;
      break;
      case eMemory_IOregs:
         m_offset = 0x4000;
         m_tblRegisters = tblAPURegisters;
      break;
      case eMemory_PPUoam:
      default:
         m_offset = 0;
         m_tblRegisters = NULL;
      break;
   }
}

CDebuggerRegisterDisplayModel::~CDebuggerRegisterDisplayModel()
{
}

QVariant CDebuggerRegisterDisplayModel::data(const QModelIndex &index, int role) const
{
   char data [ 64 ];

   if (!index.isValid())
      return QVariant();

   if (role != Qt::DisplayRole)
      return QVariant();

   CBitfieldData* pBitfield = m_tblRegisters[m_register]->GetBitfield ( index.row() );
   int regData = (int)index.internalPointer();
   if ( pBitfield->GetNumValues() )
   {
      sprintf ( data, "%s (%X)", pBitfield->GetValue(regData), (unsigned char)pBitfield->GetValueRaw(regData) );
   }
   else
   {
      sprintf ( data, "%02X", (unsigned char)pBitfield->GetValueRaw(regData) );
   }
   return data;
}

Qt::ItemFlags CDebuggerRegisterDisplayModel::flags(const QModelIndex &index) const
{
   Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
   return flags;
}

QVariant CDebuggerRegisterDisplayModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role != Qt::DisplayRole)
      return QVariant();
   char buffer [ 64 ];
   if ( orientation == Qt::Horizontal )
   {
      sprintf ( buffer, "Value" );
   }
   else
   {
      CBitfieldData* pBitfield = m_tblRegisters[m_register]->GetBitfield ( section );
      sprintf ( buffer, pBitfield->GetName() );
   }

   return  QString(buffer);
}

bool CDebuggerRegisterDisplayModel::setData ( const QModelIndex & index, const QVariant & value, int )
{
   unsigned char data;
   bool ok = false;

   data = value.toString().toInt(&ok,16);
   if ( ok && (data<256) )
   {
      switch ( m_display )
      {
         case eMemory_PPUregs:
            CPPU::_PPU(m_offset+index.column(), data);
         break;
         case eMemory_IOregs:
            CAPU::_APU(m_offset+(index.row()<<2)+index.column(), data);
         break;
         case eMemory_PPUoam:
            CPPU::OAM(index.column(), index.row(), data);
         break;
         default:
         break;
      }
   }
   return ok;
}

QModelIndex CDebuggerRegisterDisplayModel::index(int row, int column, const QModelIndex &parent) const
{
   switch ( m_display )
   {
      case eMemory_PPUregs:
         return createIndex(row, column, (int)CPPU::_PPU(m_offset+m_register));
      break;
      case eMemory_IOregs:
         return createIndex(row, column, (int)CAPU::_APU(m_offset+m_register));
      break;
      case eMemory_PPUoam:
         return createIndex(row, column, (int)CPPU::OAM(column,row));
      break;
      default:
         return QModelIndex();
      break;
   }
   return QModelIndex();
}

int CDebuggerRegisterDisplayModel::rowCount(const QModelIndex &parent) const
{
   return m_tblRegisters[m_register]->GetNumBitfields();
}

int CDebuggerRegisterDisplayModel::columnCount(const QModelIndex &parent) const
{
   return 1;
}

void CDebuggerRegisterDisplayModel::layoutChangedEvent()
{
    this->layoutChanged();
}
