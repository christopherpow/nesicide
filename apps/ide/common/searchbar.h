#ifndef SEARCHBAR_H
#define SEARCHBAR_H

#include <QWidget>

namespace Ui {
   class SearchBar;
}

class SearchBar : public QWidget
{
   Q_OBJECT

public:
   explicit SearchBar(QString settingsPrefix,QWidget *parent = 0);
   virtual ~SearchBar();

   void showCloseButton(bool show);
   QString currentSearchText();
   bool    isCaseSensitive();
   bool    isRegularExpression();
   bool    searchIsDown();

protected:
   void focusInEvent(QFocusEvent *event);
   bool eventFilter(QObject *object, QEvent *event);

private:
   Ui::SearchBar *ui;
   QString m_settingsPrefix;

signals:
   void snapTo(QString item);
   void replaceText(QString from, QString to,bool replaceAll);

private slots:
   void on_close_clicked();
   void activateMe(QString item);
   void hintMe(bool found);
   void on_direction_toggled(bool checked);
   void on_regex_toggled(bool checked);
   void on_caseSensitive_toggled(bool checked);
   void on_searchText_editTextChanged(QString );
   void on_searchText_activated(QString search);
   void on_findForReplace_clicked();
   void on_replace_clicked();
   void on_replaceAll_clicked();
};

#endif // SEARCHBAR_H
