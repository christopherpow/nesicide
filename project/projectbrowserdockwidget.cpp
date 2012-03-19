#include "projectbrowserdockwidget.h"
#include "ui_projectbrowserdockwidget.h"

#include "cnesicideproject.h"

#include "main.h"

ProjectBrowserDockWidget::ProjectBrowserDockWidget(CProjectTabWidget* pTarget,QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ProjectBrowserDockWidget)
{
   ui->setupUi(this);

   m_pTarget = pTarget;

   m_pProjectTreeviewModel = new CProjectTreeViewModel(ui->projectTreeView, nesicideProject);
   ui->projectTreeView->setModel(m_pProjectTreeviewModel);
}

ProjectBrowserDockWidget::~ProjectBrowserDockWidget()
{
   delete ui;
   delete m_pProjectTreeviewModel;
}

void ProjectBrowserDockWidget::layoutChangedEvent()
{
   m_pProjectTreeviewModel->layoutChangedEvent();
}

void ProjectBrowserDockWidget::enableNavigation()
{
   ui->projectTreeView->setModel(m_pProjectTreeviewModel);
}

void ProjectBrowserDockWidget::disableNavigation()
{
   ui->projectTreeView->setModel(NULL);
}

void ProjectBrowserDockWidget::on_projectTreeView_doubleClicked(const QModelIndex &index)
{
   IProjectTreeViewItem* item = (IProjectTreeViewItem*)index.internalPointer();

   item->openItemEvent(m_pTarget);
}
