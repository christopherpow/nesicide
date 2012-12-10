
#include "cprojecttreeopenaction.h"
#include "cprojecttabwidget.h"
#include "cprojecttreewidget.h"
#include "cdesignereditorbase.h"
#include "model/cprojectmodel.h"
#include "model/cattributemodel.h"
#include "model/ccartridgemodel.h"
#include "model/cgraphicsbankmodel.h"
#include "model/csourcefilemodel.h"
#include "model/ctilestampmodel.h"

CProjectTreeOpenAction::CProjectTreeOpenAction(CProjectTabWidget *tabWidget, CProjectTreeWidget *openItems, CProjectModel *project)
   : m_tabWidget(tabWidget), m_openItems(openItems), m_project(project)
{
}

void CProjectTreeOpenAction::visit(CUuid &)
{
   // Unknown uuid type, ignore.
}

void CProjectTreeOpenAction::visit(CAttributeUuid &data)
{
   doVisit(m_project->getAttributeModel(), data.uuid);
}

void CProjectTreeOpenAction::visit(CBinaryFileUuid &)
{
   // Nothing to do; can't open/modify.
}

void CProjectTreeOpenAction::visit(CFilterUuid &)
{
   // Nothing to do; can't open/modify.
}

void CProjectTreeOpenAction::visit(CGraphicsBankUuid &data)
{
   doVisit(m_project->getGraphicsBankModel(), data.uuid);
}

void CProjectTreeOpenAction::visit(CSourceFileUuid &data)
{
   doVisit(m_project->getSourceFileModel(), data.uuid);
}

void CProjectTreeOpenAction::visit(CTileStampUuid &data)
{
   doVisit(m_project->getTileStampModel(), data.uuid);
}

void CProjectTreeOpenAction::visit(CChrRomUuid &data)
{
   doVisit(m_project->getCartridgeModel(), data.uuid);
}

void CProjectTreeOpenAction::visit(CPrgRomUuid &data)
{
   doVisit(m_project->getCartridgeModel(), data.uuid);
}

void CProjectTreeOpenAction::doVisit(CSubModel *model, const QUuid &uuid)
{
   if (m_openItems->containsUuid(uuid))
   {
      // If already open, bring item to front.
      m_openItems->setCurrentUuid(uuid);
   }
   else
   {
      // If not open, create new tab.
      CDesignerEditorBase* pEditor = model->createEditorWidget(uuid);
      if (pEditor == NULL)
         return;

      m_tabWidget->addTab(pEditor, model->getName(uuid) );
      m_tabWidget->setCurrentWidget(pEditor);

      // Add tab to open items.
      m_openItems->addItem(m_project, uuid, QUuid());
   }
}
