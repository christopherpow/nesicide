
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

QString CTileStampModel::getName(const QUuid &uuid) const
{
   if (m_pProject == NULL)
      return QString();

   CTileStamp* tile = ProjectSearcher::findItemByUuid<CTileStamp>(m_pProject, uuid);
   return tile->caption();
}


QUuid CTileStampModel::newTileStamp(const QString &name)
{
   if (m_pProject == NULL)
      return QUuid();

   CTileStamps* pTileStamps = m_pProject->getProject()->getProjectPrimitives()->getTileStamps();
   CTileStamp* pTileStamp = new CTileStamp(pTileStamps);
   pTileStamp->setName(name);
   pTileStamps->getTileStampList().append(pTileStamp);
   pTileStamps->appendChild(pTileStamp);

   m_pProject->setDirty(true);
   emit tileStampAdded(pTileStamp->uuid());
   return pTileStamp->uuid();
}

QUuid CTileStampModel::newScreen(const QString &name)
{
   if (m_pProject == NULL)
      return QUuid();

   CTileStamps* pTileStamps = m_pProject->getProject()->getProjectPrimitives()->getTileStamps();
   CTileStamp* pTileStamp = new CTileStamp(pTileStamps);
   pTileStamp->setName(name);
   pTileStamp->setSize(256,240);
   pTileStamps->getTileStampList().append(pTileStamp);
   pTileStamps->appendChild(pTileStamp);

   m_pProject->setDirty(true);
   emit tileStampAdded(pTileStamp->uuid());
   return pTileStamp->uuid();
}

void CTileStampModel::deleteTileStamp(const QUuid &uuid)
{
   if (m_pProject == NULL)
      return;

   CTileStamp* pTileStamp = ProjectSearcher::findItemByUuid<CTileStamp>(m_pProject, uuid);
   if (pTileStamp == NULL)
      return;

   m_pProject->getProject()->getProjectPrimitives()->getTileStamps()->removeChild(pTileStamp);
   m_pProject->getProject()->getProjectPrimitives()->getTileStamps()->getTileStampList().removeAll(pTileStamp);
   m_pProject->setDirty(true);
   delete pTileStamp;

   emit tileStampRemoved(uuid);
}

CDesignerEditorBase *CTileStampModel::createEditorWidget(const QUuid &uuid) const
{
   CTileStamp* tile = ProjectSearcher::findItemByUuid<CTileStamp>(m_pProject, uuid);
   if (tile == NULL)
      return NULL;

   int x,y;
   tile->getSize(&x,&y);

   return new TileStampEditorForm(tile->getTileData(), tile->getAttributeData(),
                                  tile->getAttrTbl(), tile->getTileProperties(),
                                  x, y, tile->getGridSetting(),tile);
}
