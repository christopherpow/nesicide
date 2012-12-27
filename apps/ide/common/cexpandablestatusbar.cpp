#include "cexpandablestatusbar.h"
#include "ui_cexpandablestatusbar.h"

CExpandableStatusBar::CExpandableStatusBar(QWidget *parent) :
   QFrame(parent),
   ui(new Ui::CExpandableStatusBar)
{
   ui->setupUi(this);
}

void CExpandableStatusBar::addExpandingWidget ( QWidget * widget, int stretch )
{
   ui->frame->layout()->addWidget(widget);
   ui->frame->update();
}

void CExpandableStatusBar::addWidget ( QWidget * widget, int stretch )
{
   ui->horizontalLayout->insertWidget(0,widget,stretch,Qt::AlignLeft);
}

void CExpandableStatusBar::addPermanentWidget ( QWidget * widget, int stretch )
{
   ui->horizontalLayout->insertWidget(ui->horizontalLayout->count(),widget,stretch,Qt::AlignRight);
}

void CExpandableStatusBar::removeWidget ( QWidget * widget )
{
   ui->horizontalLayout->removeWidget(widget);
}
