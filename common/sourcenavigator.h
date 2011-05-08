#ifndef SOURCENAVIGATOR_H
#define SOURCENAVIGATOR_H

#include <QTabWidget>
#include <QWidget>

#include "iprojecttreeviewitem.h"

namespace Ui {
    class SourceNavigator;
}

class SourceNavigator : public QWidget
{
   Q_OBJECT

public:
   explicit SourceNavigator(QTabWidget* pTarget,QWidget *parent = 0);
   ~SourceNavigator();

   void shutdown();

private:
   Ui::SourceNavigator *ui;
   void updateSymbolsForFile(QString file);
   void updateFiles(bool doIt);
   QTabWidget* m_pTarget;

signals:
   void fileNavigator_fileChanged(QString file);
   void fileNavigator_symbolChanged(QString file, QString symbol, int linenumber);

private slots:
   void on_symbols_activated(QString );
   void on_files_activated(QString );

public slots:
   void compiler_compileDone(bool bOk);
   void emulator_emulatorPaused(bool show = true);
   void emulator_cartridgeLoaded();
   void projectTreeView_openItem(IProjectTreeViewItem* item);
};

#endif // SOURCENAVIGATOR_H
