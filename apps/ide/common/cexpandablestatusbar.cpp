#include "cexpandablestatusbar.h"
#include "ui_cexpandablestatusbar.h"

#include <QMouseEvent>
#include <QSplitter>

CExpandableStatusBar::CExpandableStatusBar(QWidget *parent) :
   QFrame(parent),
   ui(new Ui::CExpandableStatusBar)
{
   ui->setupUi(this);
   ui->splitter->handle(1)->installEventFilter(this);
   ui->splitter->setEnabled(false);
}

bool CExpandableStatusBar::eventFilter(QObject *object, QEvent *event)
{
   if ( object == ui->splitter->handle(1) )
   {
      QString str;
      if ( event->type() == QEvent::MouseButtonPress )
      {
         QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
         touchPos = mapToGlobal(mouseEvent->pos());
         str.sprintf("(%d,%d)",touchPos.x(),touchPos.y());
//         qDebug(str.toLatin1().constData());
      }
      if ( event->type() == QEvent::MouseMove )
      {
         QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
         if ( mouseEvent->buttons() == Qt::LeftButton )
         {
            if ( mapToGlobal(mouseEvent->pos()).y()-touchPos.y() < 0 )
            {
               // Moving bar upwards...
//               qDebug("upwards");
            }
            else if ( mapToGlobal(mouseEvent->pos()).y()-touchPos.y() > 0 )
            {
               // Moving bar downwards...
//               qDebug("downwards");
            }

//            ui->frame->setFixedHeight(ui->frame->height()-(mapToGlobal(mouseEvent->pos()).y()-touchPos.y()));

            touchPos = mapToGlobal(mouseEvent->pos());
            str.sprintf("(%d,%d)",touchPos.x(),touchPos.y());
//            qDebug(str.toLatin1().constData());

            event->accept();
            return true;
         }
      }
   }
   if ( object == expandableWidget )
   {
      if ( event->type() == QEvent::Hide )
      {
         ui->splitter->setEnabled(false);
         return false;
      }
      else if ( event->type() == QEvent::Show )
      {
         ui->splitter->setEnabled(true);
         return false;
      }
   }
   return false;
}

void CExpandableStatusBar::addExpandingWidget ( QWidget * widget, int stretch )
{
   widget->installEventFilter(this);
   expandableWidget = widget;
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

void CExpandableStatusBar::on_splitter_splitterMoved(int pos, int index)
{

}
