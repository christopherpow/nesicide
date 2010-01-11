#include "breakpointinspector.h"

#include "breakpointdialog.h"

BreakpointInspector::BreakpointInspector()
{
   frame = new QFrame ( this );
   frame->setFrameRect ( this->rect() );

   dialog = new BreakpointDialog ( frame );
   this->setWidget ( dialog );
}

BreakpointInspector::~BreakpointInspector()
{
   delete dialog;
   delete frame;
}
