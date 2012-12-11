
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
   if (m_pProject == NULL)
      return QList<QUuid>();

   return ProjectSearcher::findUuidsOfType<CAttributeTable>(m_pProject);
}

QString CAttributeModel::getName(const QUuid &uuid) const
{
   CAttributeTable* palette = ProjectSearcher::findItemByUuid<CAttributeTable>(m_pProject, uuid);
   return palette != NULL ? palette->caption() : QString();
}


QUuid CAttributeModel::newPalette(const QString &name)
{
   if (m_pProject == NULL)
      return QUuid();

   CAttributeTables* attributes = m_pProject->getProject()->getProjectPrimitives()->getAttributeTables();
   CAttributeTable* pAttributeTable = new CAttributeTable(attributes);
   pAttributeTable->setName(name);
   attributes->getAttributeTableList().append(pAttributeTable);
   attributes->appendChild(pAttributeTable);
   m_pProject->setDirty(true);

   emit paletteAdded(pAttributeTable->uuid());
   return pAttributeTable->uuid();
}

void CAttributeModel::deletePalette(const QUuid &uuid)
{
   if (m_pProject == NULL)
      return;

   CAttributeTable* palette = ProjectSearcher::findItemByUuid<CAttributeTable>(m_pProject, uuid);
   if (palette == NULL)
      return;

   CAttributeTables* attributes = m_pProject->getProject()->getProjectPrimitives()->getAttributeTables();
   attributes->removeChild(palette);
   attributes->getAttributeTableList().removeAll(palette);
   m_pProject->setDirty(true);
   delete palette;

   emit paletteDeleted(uuid);
}


CDesignerEditorBase *CAttributeModel::createEditorWidget(const QUuid &uuid) const
{
   CAttributeTable* palette = ProjectSearcher::findItemByUuid<CAttributeTable>(m_pProject, uuid);
   if (palette == NULL)
      return NULL;

   return new AttributeTableEditorForm(palette->getPalette(), palette);
}
