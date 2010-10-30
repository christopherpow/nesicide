#include "mapperinformationinspector.h"

#include "mapperinformationdialog.h"

MapperInformationInspector::MapperInformationInspector()
{
   frame = new QFrame ( this );
   frame->setFrameRect ( this->rect() );

   dialog = new MapperInformationDialog ( frame );
   this->setWidget ( dialog );

   QObject::connect(dialog,SIGNAL(showMe()),this,SLOT(showMe()));
}

MapperInformationInspector::~MapperInformationInspector()
{
   delete dialog;
   delete frame;
}

void MapperInformationInspector::showMe()
{
   this->show();
}
