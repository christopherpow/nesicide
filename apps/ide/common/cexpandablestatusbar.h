#ifndef CEXPANDABLESTATUSBAR_H
#define CEXPANDABLESTATUSBAR_H

#include <QFrame>
#include <QGridLayout>
#include <QBoxLayout>

namespace Ui
{
class CExpandableStatusBar;
}

class CExpandableStatusBar : public QFrame
{
   Q_OBJECT
public:
   explicit CExpandableStatusBar(QWidget *parent = 0);

   void addPermanentWidget ( QWidget * widget, int stretch = 0 );
   void addExpandingWidget ( QWidget * widget, int stretch = 0 );
   void addWidget ( QWidget * widget, int stretch = 0 );
   void removeWidget ( QWidget * widget );

private:
   Ui::CExpandableStatusBar* ui;

signals:

public slots:

protected:
};

#endif // CEXPANDABLESTATUSBAR_H
