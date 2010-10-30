#include "executioninspector.h"

#include "executiontracerdialog.h"

ExecutionInspector::ExecutionInspector()
{
   frame = new QFrame ( this );
   frame->setFrameRect ( this->rect() );

   dialog = new ExecutionTracerDialog ( frame );
   this->setWidget ( dialog );

   QObject::connect(dialog,SIGNAL(showMe()),this,SLOT(showMe()));
}

ExecutionInspector::~ExecutionInspector()
{
   delete dialog;
   delete frame;
}

void ExecutionInspector::showMe()
{
   this->show();
}
