#ifndef QKEYMAPITEMEDIT_H
#define QKEYMAPITEMEDIT_H

#include <QLineEdit>
#include <QKeyEvent>

class QKeymapItemEdit : public QLineEdit
{
   Q_OBJECT
public:
   explicit QKeymapItemEdit(QWidget* parent = 0);
protected:
   bool event(QEvent*);
   void contextMenuEvent(QContextMenuEvent*) {}
signals:

public slots:

};

#endif // QKEYMAPITEMEDIT_H
