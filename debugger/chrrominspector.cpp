#include "chrrominspector.h"
#include "cnesppu.h"

CHRROMInspector::CHRROMInspector()
{
   imgData = new char[256*256*3];

   frame = new QFrame ( this );
   frame->setFrameRect ( this->rect() );

   dialog = new CHRROMDisplayDialog ( frame, true );
   this->setWidget ( dialog );
}

CHRROMInspector::~CHRROMInspector()
{
   delete imgData;
   delete dialog;
   delete frame;
}
