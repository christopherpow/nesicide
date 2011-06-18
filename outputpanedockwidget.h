#ifndef OUTPUTPANEDOCKWIDGET_H
#define OUTPUTPANEDOCKWIDGET_H

#include <QDockWidget>

#include "cprojecttabwidget.h"

namespace Ui {
   class OutputPaneDockWidget;
}

class OutputPaneDockWidget : public QDockWidget
{
   Q_OBJECT

public:
   enum
   {
      Output_General = 0,
      Output_Build,
      Output_Debug,
      Output_Search
   };
   explicit OutputPaneDockWidget(CProjectTabWidget* pTarget, QWidget *parent = 0);
   ~OutputPaneDockWidget();

   void clearAllPanes();
   void clearPane(int tab);

public slots:
   void showPane(int tab);
   void updateGeneralPane(QString text);
   void updateBuildPane(QString text);
   void updateDebugPane(QString text);
   void updateSearchPane(QString text);
   void eraseGeneralPane();
   void eraseBuildPane();
   void eraseDebugPane();
   void eraseSearchPane();

protected:
   virtual void contextMenuEvent ( QContextMenuEvent* event );

private:
   Ui::OutputPaneDockWidget *ui;
   CProjectTabWidget* m_pTarget;

private slots:
   void on_compilerOutputTextEdit_selectionChanged();
   void on_searchOutputTextEdit_selectionChanged();
};

#endif // OUTPUTPANEDOCKWIDGET_H
