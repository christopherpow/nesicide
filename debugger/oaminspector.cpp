#include "oaminspector.h"

OAMInspector::OAMInspector()
{
   frame = new QFrame ( this );
   frame->setFrameRect ( this->rect() );

   dialog = new OAMDisplayDialog ( frame );
   this->setWidget ( dialog );
}

OAMInspector::~OAMInspector()
{
   delete dialog;
   delete frame;
}
