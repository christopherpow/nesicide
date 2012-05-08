#ifndef OUTPUTPANEDOCKWIDGET_H
#define OUTPUTPANEDOCKWIDGET_H

#include <QDockWidget>

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

signals:
   void snapTo(QString item);

protected:
   virtual void contextMenuEvent ( QContextMenuEvent* event );

private:
   Ui::OutputPaneDockWidget *ui;

private slots:
   void on_compilerOutputTextEdit_selectionChanged();
   void on_searchOutputTextEdit_selectionChanged();
};

#endif // OUTPUTPANEDOCKWIDGET_H
