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
      Output_Debug
   };
   explicit OutputPaneDockWidget(QWidget *parent = 0);
   ~OutputPaneDockWidget();

   void clearAllPanes();
   void clearPane(int tab);

public slots:
   void showPane(int tab);

protected:
   virtual void contextMenuEvent ( QContextMenuEvent* event );

private slots:
   void updateData();

private:
   Ui::OutputPaneDockWidget *ui;
};

#endif // OUTPUTPANEDOCKWIDGET_H
