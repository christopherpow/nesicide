#include "outputdockwidget.h"
#include "ui_outputdockwidget.h"

#include "outputdialog.h"

OutputDockWidget::OutputDockWidget()
{
   frame = new QFrame ( this );
   frame->setFrameRect ( this->rect() );

   dialog = new OutputDialog ( frame );
   this->setWidget ( dialog );
}

OutputDockWidget::~OutputDockWidget()
{
   delete dialog;
   delete frame;
}
