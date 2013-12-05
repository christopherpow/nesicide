#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QWidget>
#include <QDir>

namespace Ui {
   class SearchWidget;
}

class SearchWidget : public QWidget
{
   Q_OBJECT

public:
   explicit SearchWidget(QWidget *parent = 0);
   virtual ~SearchWidget();

protected:
   void showEvent(QShowEvent *event);
   bool eventFilter(QObject *object, QEvent *event);

private:
   Ui::SearchWidget *ui;

signals:
   void search(QDir dir, QString searchText, QString pattern, bool subfolders, bool sourceSearchPaths, bool useRegex, bool caseSensitive);
   void snapTo(QString item);

private slots:
   void on_projectFolder_clicked(bool checked);
   void searcher_searchDone(int found);
   void on_find_clicked();
   void on_browse_clicked();
};

#endif // SEARCHWIDGET_H
