#include "apuinformationinspector.h"

#include "apuinformationdialog.h"

APUInformationInspector::APUInformationInspector()
{
   frame = new QFrame ( this );
   frame->setFrameRect ( this->rect() );

   dialog = new APUInformationDialog ( frame );
   this->setWidget ( dialog );

   QObject::connect(dialog,SIGNAL(showMe()),this,SLOT(showMe()));
}

APUInformationInspector::~APUInformationInspector()
{
   delete dialog;
   delete frame;
}

void APUInformationInspector::showMe()
{
   this->show();
}
