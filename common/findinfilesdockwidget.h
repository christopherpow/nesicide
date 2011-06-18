#ifndef FINDINFILESDOCKWIDGET_H
#define FINDINFILESDOCKWIDGET_H

#include <QDockWidget>
#include <QFileSystemModel>

namespace Ui {
   class FindInFilesDockWidget;
}

class FindInFilesDockWidget : public QDockWidget
{
   Q_OBJECT

public:
   explicit FindInFilesDockWidget(QWidget *parent = 0);
   ~FindInFilesDockWidget();

private:
   Ui::FindInFilesDockWidget *ui;
   QFileSystemModel* model;

private slots:
   void on_find_clicked();
   void on_browse_clicked();
};

#endif // FINDINFILESDOCKWIDGET_H
