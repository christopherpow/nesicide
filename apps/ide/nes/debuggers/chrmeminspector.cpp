#include "chrmeminspector.h"

CHRMEMInspector::CHRMEMInspector(QWidget *parent)
{
   frame = new QFrame ( this );
   frame->setFrameRect ( this->rect() );

   dialog = new CHRROMDisplayDialog ( true, NULL, NULL, frame );
   this->setWidget ( dialog );
}

CHRMEMInspector::~CHRMEMInspector()
{
   delete dialog;
   delete frame;
}
