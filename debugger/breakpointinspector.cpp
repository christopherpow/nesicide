#include "breakpointinspector.h"

#include "breakpointdialog.h"

BreakpointInspector::BreakpointInspector()
{
   frame = new QFrame ( this );
   frame->setFrameRect ( this->rect() );

   dialog = new BreakpointDialog ( frame );
   this->setWidget ( dialog );

   QObject::connect(dialog,SIGNAL(showMe()),this,SLOT(showMe()));
}

BreakpointInspector::~BreakpointInspector()
{
   delete dialog;
   delete frame;
}

void BreakpointInspector::showMe()
{
   this->show();
}
