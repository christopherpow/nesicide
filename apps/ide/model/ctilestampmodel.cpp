
#include "ctilestampmodel.h"

#include "cnesicideproject.h"
#include "model/projectsearcher.h"

CTileStampModel::CTileStampModel()
   : CSubModel()
{
}

QList<QUuid> CTileStampModel::getUuids() const
{
   if (m_pProject == NULL)
      return QList<QUuid>();

   return ProjectSearcher::findUuidsOfType<CTileStamp>(m_pProject);
}
