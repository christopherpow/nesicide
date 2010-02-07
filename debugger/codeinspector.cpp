#include "codeinspector.h"

#include "codebrowserdialog.h"

CodeInspector::CodeInspector()
{
   frame = new QFrame ( this );
   frame->setFrameRect ( this->rect() );

   dialog = new CodeBrowserDialog ( frame );
   this->setWidget ( dialog );

   QObject::connect(dialog,SIGNAL(showMe()),this,SLOT(showMe()));
}

CodeInspector::~CodeInspector()
{
   delete dialog;
   delete frame;
}

void CodeInspector::showMe()
{
   this->show();
}
