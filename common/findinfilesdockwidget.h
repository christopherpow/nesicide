#ifndef FINDINFILESDOCKWIDGET_H
#define FINDINFILESDOCKWIDGET_H

#include <QDockWidget>
#include <QDir>

namespace Ui {
   class FindInFilesDockWidget;
}

class FindInFilesDockWidget : public QDockWidget
{
   Q_OBJECT

public:
   explicit FindInFilesDockWidget(QWidget *parent = 0);
   virtual ~FindInFilesDockWidget();

protected:
   void showEvent(QShowEvent *event);

private:
   Ui::FindInFilesDockWidget *ui;
   void search(QDir dir,QString pattern,int* finds);

private slots:
   void on_projectFolder_clicked(bool checked);
   void searcher_searchDone(int found);
   void on_find_clicked();
   void on_browse_clicked();
};

#endif // FINDINFILESDOCKWIDGET_H
