#include "memoryinspector.h"

#include "memorydisplaydialog.h"

MemoryInspector::MemoryInspector(eMemoryType display)
   : m_display(display)
{
   frame = new QFrame ( this );
   frame->setFrameRect ( this->rect() );

   dialog = new MemoryDisplayDialog ( frame, display );
   this->setWidget ( dialog );

   QObject::connect(dialog,SIGNAL(showMe(eMemoryType)),this,SLOT(showMe(eMemoryType)));
}

MemoryInspector::~MemoryInspector()
{
   delete dialog;
   delete frame;
}

void MemoryInspector::showMe(eMemoryType display)
{
   if ( m_display == display )
   {
      this->show();
   }
}
