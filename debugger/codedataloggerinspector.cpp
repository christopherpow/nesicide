#include "codedataloggerinspector.h"

#include "codedataloggerdialog.h"

CodeDataLoggerInspector::CodeDataLoggerInspector()
{
   frame = new QFrame ( this );
   frame->setFrameRect ( this->rect() );

   dialog = new CodeDataLoggerDialog ( frame );
   this->setWidget ( dialog );
}

CodeDataLoggerInspector::~CodeDataLoggerInspector()
{
   delete dialog;
   delete frame;
}

void CodeDataLoggerInspector::showMe()
{
   this->show();
}
