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

   m_pProjectTreeviewModel = new CProjectTreeViewModel(ui->projectTreeWidget, nesicideProject);
   ui->projectTreeWidget->setModel(m_pProjectTreeviewModel);
   ui->projectTreeWidget->setTarget(m_pTarget);
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
   ui->projectTreeWidget->setModel(m_pProjectTreeviewModel);
}

void ProjectBrowserDockWidget::disableNavigation()
{
   ui->projectTreeWidget->setModel(NULL);
}
