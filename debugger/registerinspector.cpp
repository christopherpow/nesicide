#include "registerinspector.h"

#include "registerdisplaydialog.h"

RegisterInspector::RegisterInspector(eMemoryType display)
   : m_display(display)
{
   frame = new QFrame ( this );
   frame->setFrameRect ( this->rect() );

   dialog = new RegisterDisplayDialog ( frame, display );
   this->setWidget ( dialog );

   QObject::connect(dialog,SIGNAL(showMe(eMemoryType)),this,SLOT(showMe(eMemoryType)));
}

RegisterInspector::~RegisterInspector()
{
   delete dialog;
   delete frame;
}

void RegisterInspector::showMe(eMemoryType display)
{
   if ( m_display == display )
   {
      this->show();
   }
}
