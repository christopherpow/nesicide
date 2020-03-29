
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
#include "model/cmusicmodel.h"

CProjectTreeOpenAction::CProjectTreeOpenAction(CProjectTabWidget *tabWidget, CProjectModel *project)
   : m_tabWidget(tabWidget), m_project(project)
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

void CProjectTreeOpenAction::visit(CMusicFileUuid &data)
{
   doVisit(m_project->getMusicModel(), data.uuid);
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
   // If already open, bring item to front.
   for(int i=0; i < m_tabWidget->count(); ++i)
   {
      QWidget* tab = m_tabWidget->widget(i);

      // If tab is an editor, check uuid.
      CDesignerEditorBase* editor = dynamic_cast<CDesignerEditorBase*>(tab);
      if (editor != NULL)
      {
         IProjectTreeViewItem* item = editor->treeLink();
         if (item != NULL && item->uuid() == uuid.toString())
         {
            // Found the item, bring to front instead of opening it.
            m_tabWidget->setCurrentIndex(i);
            return;
         }
      }
   }

   // Not found, open new tab.
   CDesignerEditorBase* pEditor = model->createEditorWidget(uuid);
   if (pEditor == NULL)
      return;

   m_tabWidget->addTab(pEditor, model->getName(uuid) );
   m_tabWidget->setCurrentWidget(pEditor);
}
