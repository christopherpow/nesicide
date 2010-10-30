#include "ppuinformationinspector.h"

#include "ppuinformationdialog.h"

PPUInformationInspector::PPUInformationInspector()
{
   frame = new QFrame ( this );
   frame->setFrameRect ( this->rect() );

   dialog = new PPUInformationDialog ( frame );
   this->setWidget ( dialog );

   QObject::connect(dialog,SIGNAL(showMe()),this,SLOT(showMe()));
}

PPUInformationInspector::~PPUInformationInspector()
{
   delete dialog;
   delete frame;
}

void PPUInformationInspector::showMe()
{
   this->show();
}
