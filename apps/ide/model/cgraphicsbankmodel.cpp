
#include "cgraphicsbankmodel.h"

#include "cnesicideproject.h"
#include "model/projectsearcher.h"

CGraphicsBankModel::CGraphicsBankModel()
   : CSubModel()
{
}


QList<QUuid> CGraphicsBankModel::getUuids() const
{
   if (m_pProject == NULL)
      return QList<QUuid>();

   return ProjectSearcher::findUuidsOfType<CGraphicsBank>(m_pProject);
}

QString CGraphicsBankModel::getName(const QUuid &uuid) const
{
   CGraphicsBank* bank = ProjectSearcher::findItemByUuid<CGraphicsBank>(m_pProject, uuid);
   return bank->caption();
}
