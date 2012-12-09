
#include "cattributemodel.h"

#include "cnesicideproject.h"
#include "model/projectsearcher.h"

CAttributeModel::CAttributeModel()
   : CSubModel()
{
}


QList<QUuid> CAttributeModel::getUuids() const
{
   if (m_pProject == NULL)
      return QList<QUuid>();

   return ProjectSearcher::findUuidsOfType<CAttributeTable>(m_pProject);
}
