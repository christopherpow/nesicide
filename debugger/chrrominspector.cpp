#include "chrrominspector.h"
#include "cnesppu.h"

#include "cnesppu.h"

CHRROMInspector::CHRROMInspector()
{
   frame = new QFrame ( this );
   frame->setFrameRect ( this->rect() );

   dialog = new CHRROMDisplayDialog ( frame, true, NULL );
   this->setWidget ( dialog );
}

CHRROMInspector::~CHRROMInspector()
{
   delete dialog;
   delete frame;
}
