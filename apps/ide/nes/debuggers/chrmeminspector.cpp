#include "chrmeminspector.h"

CHRMEMInspector::CHRMEMInspector(QWidget */*parent*/)
{
   frame = new QFrame ( this );
   frame->setFrameRect ( this->rect() );

   dialog = new CHRROMDisplayDialog ( true, NULL, NULL, frame );
   setWidget ( dialog );

   QObject::connect(dialog,SIGNAL(addStatusBarWidget(QWidget*)),this,SIGNAL(addStatusBarWidget(QWidget*)));
   QObject::connect(dialog,SIGNAL(removeStatusBarWidget(QWidget*)),this,SIGNAL(removeStatusBarWidget(QWidget*)));
}

CHRMEMInspector::~CHRMEMInspector()
{
   delete dialog;
   delete frame;
}
