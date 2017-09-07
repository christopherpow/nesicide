
#include "cattributemodel.h"

#include "cnesicideproject.h"
#include "cdesignereditorbase.h"
#include "model/projectsearcher.h"

CAttributeModel::CAttributeModel()
   : CSubModel()
{
}


QList<QUuid> CAttributeModel::getUuids() const
{
   if (m_pNesicideProject == NULL)
      return QList<QUuid>();

   return ProjectSearcher::findUuidsOfType<CAttributeTable>(m_pNesicideProject);
}

QString CAttributeModel::getName(const QUuid &uuid) const
{
   CAttributeTable* palette = ProjectSearcher::findItemByUuid<CAttributeTable>(m_pNesicideProject, uuid);
   return palette != NULL ? palette->caption() : QString();
}


QUuid CAttributeModel::newPalette(const QString &name)
{
   if (m_pNesicideProject == NULL)
      return QUuid();

   CAttributeTables* attributes = m_pNesicideProject->getProject()->getProjectPrimitives()->getAttributeTables();
   CAttributeTable* pAttributeTable = new CAttributeTable(attributes);
   pAttributeTable->setName(name);
   attributes->getAttributeTableList().append(pAttributeTable);
   attributes->appendChild(pAttributeTable);
   m_pNesicideProject->setDirty(true);

   emit paletteAdded(pAttributeTable->uuid());
   return pAttributeTable->uuid();
}

void CAttributeModel::deletePalette(const QUuid &uuid)
{
   if (m_pNesicideProject == NULL)
      return;

   CAttributeTable* palette = ProjectSearcher::findItemByUuid<CAttributeTable>(m_pNesicideProject, uuid);
   if (palette == NULL)
      return;

   CAttributeTables* attributes = m_pNesicideProject->getProject()->getProjectPrimitives()->getAttributeTables();
   attributes->removeChild(palette);
   attributes->getAttributeTableList().removeAll(palette);
   m_pNesicideProject->setDirty(true);
   delete palette;

   emit paletteDeleted(uuid);
}


CDesignerEditorBase *CAttributeModel::createEditorWidget(const QUuid &uuid) const
{
   CAttributeTable* palette = ProjectSearcher::findItemByUuid<CAttributeTable>(m_pNesicideProject, uuid);
   if (palette == NULL)
      return NULL;

   // Item must know editor due to current architecture.
   palette->setEditor(new AttributeTableEditorForm(palette->getPalette(), palette));
   return palette->editor();
}
