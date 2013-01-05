
#include "model/cfiltermodel.h"

#include "model/cprojectmodel.h"
#include "model/cattributemodel.h"
#include "model/cbinaryfilemodel.h"
#include "model/ccartridgemodel.h"
#include "model/cgraphicsbankmodel.h"
#include "model/csourcefilemodel.h"
#include "model/ctilestampmodel.h"
#include "model/cmusicmodel.h"

#include "cnesicideproject.h"

static const QString FILTER_NAME[] =
{
   "Project", "Primitives", "Attributes", "Tiles & Screens", "Source Code",
   "Binary Files", "Graphics Banks", "Sounds", "Musics", "Cartridge", "PRG-ROM Banks", "CHR-ROM Banks"
};

CFilterModel::CFilterModel(CProjectModel* project)
   : CSubModel(), m_pProjectModel(project)
{
   // During the switch between old and new backend we use hard coded filters.
   m_filters.append(QUuid::createUuid()); // 0 Project
   m_filters.append(QUuid::createUuid()); // 1  Primitives
   m_filters.append(QUuid::createUuid()); // 2   Attributes
   m_filters.append(QUuid::createUuid()); // 3   Tiles & Screens
   m_filters.append(QUuid::createUuid()); // 4  Source Code
   m_filters.append(QUuid::createUuid()); // 5  Binary Files
   m_filters.append(QUuid::createUuid()); // 6  Graphics Banks
   m_filters.append(QUuid::createUuid()); // 7  Sounds
   m_filters.append(QUuid::createUuid()); // 8   Musics
   m_filters.append(QUuid::createUuid()); // 9 Cartridge
   m_filters.append(QUuid::createUuid()); // 10  PRG ROM
   m_filters.append(QUuid::createUuid()); // 11  CHR ROM
}

QList<QUuid> CFilterModel::getUuids() const
{
   return m_filters;
}

QString CFilterModel::getName(const QUuid &uuid) const
{
   return getFilterName(uuid);
}

QUuid CFilterModel::newFilter(const QString &/*name*/)
{
   // Ignore for now.
   return QUuid();
}

bool CFilterModel::isFilter(const QUuid &uuid) const
{
   return getFilterId(uuid) != -1;
}

bool CFilterModel::compare(const QUuid &uuid1, const QUuid &uuid2) const
{
   return getFilterId(uuid1) < getFilterId(uuid2);
}

QList<QUuid> CFilterModel::getFilteredItems(const QUuid &uuid) const
{
   QList<QUuid> items;

   int id = getFilterId(uuid);
   switch(id)
   {
      case 0:
         items.append(m_filters[1]);
         items.append(m_filters[4]);
         items.append(m_filters[5]);
         items.append(m_filters[6]);
         items.append(m_filters[7]);
         break;

      case 1:
         items.append(m_filters[2]);
         items.append(m_filters[3]);
         break;

      case 2: return m_pProjectModel->getAttributeModel()->getUuids();
      case 3: return m_pProjectModel->getTileStampModel()->getUuids();
      case 4: return m_pProjectModel->getSourceFileModel()->getUuids();
      case 5: return m_pProjectModel->getBinaryFileModel()->getUuids();
      case 6: return m_pProjectModel->getGraphicsBankModel()->getUuids();

      case 7:
         items.append(m_filters[8]);
         break;

      case 8: return m_pProjectModel->getMusicModel()->getUuids();

      case 9:
         items.append(m_filters[10]);
         items.append(m_filters[11]);
         break;

      case 10: return m_pProjectModel->getCartridgeModel()->getPrgRomUuids();
      case 11: return m_pProjectModel->getCartridgeModel()->getChrRomUuids();

      default:
         break;
   }

   return items;
}

QString CFilterModel::getFilterName(const QUuid &uuid) const
{
   return isFilter(uuid) ? FILTER_NAME[getFilterId(uuid)] : QString();
}

void CFilterModel::addToFilter(const QUuid &/*filterUuid*/, const QUuid &/*uuid*/)
{
   // Ignore for now.
}

int CFilterModel::getFilterId(const QUuid &filterUuid) const
{
   for(int i=0; i < m_filters.count(); ++i)
   {
      if (m_filters[i] == filterUuid)
         return i;
   }
   return -1;
}
