#include "executioninspector.h"

#include "executiontracerdialog.h"

ExecutionInspector::ExecutionInspector()
{
   frame = new QFrame ( this );
   frame->setFrameRect ( this->rect() );

   dialog = new ExecutionTracerDialog ( frame );
   this->setWidget ( dialog );
}

ExecutionInspector::~ExecutionInspector()
{
   delete dialog;
   delete frame;
}
