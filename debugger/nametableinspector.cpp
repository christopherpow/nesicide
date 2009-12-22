#include "nametableinspector.h"

NameTableInspector::NameTableInspector()
{
   frame = new QFrame ( this );
   frame->setFrameRect ( this->rect() );

   dialog = new NameTableDisplayDialog ( frame );
   this->setWidget ( dialog );
}

NameTableInspector::~NameTableInspector()
{
   delete dialog;
   delete frame;
}
