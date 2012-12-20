#ifndef SEARCHDOCKWIDGET_H
#define SEARCHDOCKWIDGET_H

#include <QDockWidget>
#include <QDir>

namespace Ui {
   class SearchDockWidget;
}

class SearchDockWidget : public QDockWidget
{
   Q_OBJECT

public:
   explicit SearchDockWidget(QWidget *parent = 0);
   virtual ~SearchDockWidget();

protected:
   void showEvent(QShowEvent *event);
   bool eventFilter(QObject *object, QEvent *event);

private:
   Ui::SearchDockWidget *ui;

signals:
   void search(QDir dir, QString searchText, QString pattern, bool subfolders, bool sourceSearchPaths, bool useRegex, bool caseSensitive);
   void snapTo(QString item);

private slots:
   void on_projectFolder_clicked(bool checked);
   void searcher_searchDone(int found);
   void on_find_clicked();
   void on_browse_clicked();
};

#endif // SEARCHDOCKWIDGET_H
