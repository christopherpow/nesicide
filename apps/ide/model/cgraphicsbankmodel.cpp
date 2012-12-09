
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
   if (m_pProject == NULL)
      return QString();

   CGraphicsBank* bank = ProjectSearcher::findItemByUuid<CGraphicsBank>(m_pProject, uuid);
   return bank != NULL ? bank->caption() : QString();
}


QUuid CGraphicsBankModel::newGraphicsBank(const QString &name)
{
   if (m_pProject == NULL)
      return QUuid();

   CGraphicsBanks* banks = m_pProject->getProject()->getGraphicsBanks();
   CGraphicsBank* pGraphicsBank = new CGraphicsBank(banks);
   pGraphicsBank->setName(name);
   banks->getGraphicsBanks().append(pGraphicsBank);
   banks->appendChild(pGraphicsBank);

   m_pProject->setDirty(true);
   emit graphicsBankAdded(pGraphicsBank->uuid());
   return pGraphicsBank->uuid();
}

void CGraphicsBankModel::deleteGraphicsBank(const QUuid &uuid)
{
   if (m_pProject == NULL)
      return;

   CGraphicsBank* bank = ProjectSearcher::findItemByUuid<CGraphicsBank>(m_pProject, uuid);
   if (bank == NULL)
      return;

   m_pProject->getProject()->getGraphicsBanks()->removeChild(bank);
   m_pProject->getProject()->getGraphicsBanks()->getGraphicsBanks().removeAll(bank);
   delete bank;

   emit graphicsBankDeleted(uuid);
}
