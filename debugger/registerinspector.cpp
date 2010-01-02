#include "registerinspector.h"

#include "registerdisplaydialog.h"

RegisterInspector::RegisterInspector(eMemoryType display)
{
   frame = new QFrame ( this );
   frame->setFrameRect ( this->rect() );

   dialog = new RegisterDisplayDialog ( frame, display );
   this->setWidget ( dialog );
}

RegisterInspector::~RegisterInspector()
{
   delete dialog;
   delete frame;
}
