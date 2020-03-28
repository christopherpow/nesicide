
#include "cgraphicsbankmodel.h"

#include "model/projectsearcher.h"

#include "cnesicideproject.h"
#include "graphicsbankeditorform.h"

CGraphicsBankModel::CGraphicsBankModel()
   : CSubModel()
{
}


QList<QUuid> CGraphicsBankModel::getUuids() const
{
   if (m_pNesicideProject == NULL)
      return QList<QUuid>();

   return ProjectSearcher::findUuidsOfType<CGraphicsBank>(m_pNesicideProject);
}

QString CGraphicsBankModel::getName(const QUuid &uuid) const
{
   if (m_pNesicideProject == NULL)
      return QString();

   CGraphicsBank* bank = ProjectSearcher::findItemByUuid<CGraphicsBank>(m_pNesicideProject, uuid);
   return bank != NULL ? bank->caption() : QString();
}


QUuid CGraphicsBankModel::newGraphicsBank(const QString &name)
{
   if (m_pNesicideProject == NULL)
      return QUuid();

   CGraphicsBanks* banks = m_pNesicideProject->getProject()->getGraphicsBanks();
   CGraphicsBank* pGraphicsBank = new CGraphicsBank(banks);
   pGraphicsBank->setName(name);
   banks->getGraphicsBanks().append(pGraphicsBank);
   banks->appendChild(pGraphicsBank);

   m_pNesicideProject->setDirty(true);
   emit graphicsBankAdded(pGraphicsBank->uuid());
   return pGraphicsBank->uuid();
}

void CGraphicsBankModel::deleteGraphicsBank(const QUuid &uuid)
{
   if (m_pNesicideProject == NULL)
      return;

   CGraphicsBank* bank = ProjectSearcher::findItemByUuid<CGraphicsBank>(m_pNesicideProject, uuid);
   if (bank == NULL)
      return;

   m_pNesicideProject->getProject()->getGraphicsBanks()->removeChild(bank);
   m_pNesicideProject->getProject()->getGraphicsBanks()->getGraphicsBanks().removeAll(bank);
   delete bank;

   emit graphicsBankDeleted(uuid);
}


CDesignerEditorBase *CGraphicsBankModel::createEditorWidget(const QUuid &uuid) const
{
   CGraphicsBank* bank = ProjectSearcher::findItemByUuid<CGraphicsBank>(m_pNesicideProject, uuid);
   if (bank == NULL)
      return NULL;
   // Data item needs to know its editor.
   bank->setEditor(new GraphicsBankEditorForm(bank->getSize(),bank->getGraphics(), bank));
   return bank->editor();
}

