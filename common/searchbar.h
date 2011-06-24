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
   explicit SearchBar(QWidget *parent = 0);
   virtual ~SearchBar();

protected:
   void focusInEvent(QFocusEvent *event);

private:
   Ui::SearchBar *ui;

signals:
   void snapTo(QString item);

private slots:
    void on_direction_toggled(bool checked);
    void on_regex_toggled(bool checked);
    void on_caseSensitive_toggled(bool checked);
    void on_searchText_editTextChanged(QString );
    void on_searchText_activated(QString search);
};

#endif // SEARCHBAR_H
