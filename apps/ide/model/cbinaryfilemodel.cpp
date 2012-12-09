
#include "cbinaryfilemodel.h"

#include "cnesicideproject.h"
#include "model/projectsearcher.h"

CBinaryFileModel::CBinaryFileModel()
   : CSubModel()
{
}


QList<QUuid> CBinaryFileModel::getUuids() const
{
   if (m_pProject == NULL)
       return QList<QUuid>();

   return ProjectSearcher::findUuidsOfType<CBinaryFile>(m_pProject);
}
