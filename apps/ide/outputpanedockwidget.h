#ifndef OUTPUTPANEDOCKWIDGET_H
#define OUTPUTPANEDOCKWIDGET_H

#include <QDockWidget>
#include <QPushButton>

#include "searchwidget.h"

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
   explicit OutputPaneDockWidget(QWidget *parent = 0);
   virtual ~OutputPaneDockWidget();

   void initialize();
   void clearAllPanes();
   void clearPane(int tab);

   bool eventFilter(QObject *object, QEvent *event);

public slots:
   void resetPane(int tab);
   void showPane(int tab);
   void updateGeneralPane(QString text);
   void updateBuildPane(QString text);
   void updateDebugPane(QString text);
   void updateSearchPane(QString text);
   void showGeneralPane();
   void showBuildPane();
   void showDebugPane();
   void showSearchPane();
   void eraseGeneralPane();
   void eraseBuildPane();
   void eraseDebugPane();
   void eraseSearchPane();
   void compiler_compileStarted();
   void compiler_compileDone(bool ok);
   void compiler_cleanStarted();
   void compiler_cleanDone(bool ok);
   void searcher_searchDone(int results);
   void handleVisibilityChanged(bool visible);

signals:
   void snapTo(QString item);
   void addStatusBarWidget(QWidget* item);
   void removeStatusBarWidget(QWidget* item);
   void addPermanentStatusBarWidget(QWidget* item);
   void removePermanentStatusBarWidget(QWidget* item);

protected:
   virtual void contextMenuEvent ( QContextMenuEvent* event );

private:
   Ui::OutputPaneDockWidget *ui;
   QPushButton* general;
   QPushButton* searchResults;
   QPushButton* buildResults;
   QPushButton* debugInfo;
   SearchWidget* m_pSearch;
};

#endif // OUTPUTPANEDOCKWIDGET_H
