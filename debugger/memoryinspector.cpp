#include "memoryinspector.h"

#include "memorydisplaydialog.h"

MemoryInspector::MemoryInspector(eMemoryType display)
{
   frame = new QFrame ( this );
   frame->setFrameRect ( this->rect() );

   dialog = new MemoryDisplayDialog ( frame, display );
   this->setWidget ( dialog );
}

MemoryInspector::~MemoryInspector()
{
   delete dialog;
   delete frame;
}
