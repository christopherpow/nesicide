#include "executionvisualizer.h"

#include "executionvisualizerdialog.h"

ExecutionVisualizer::ExecutionVisualizer()
{
   frame = new QFrame ( this );
   frame->setFrameRect ( this->rect() );

   dialog = new ExecutionVisualizerDialog ( frame );
   this->setWidget ( dialog );
}

ExecutionVisualizer::~ExecutionVisualizer()
{
   delete dialog;
   delete frame;
}

void ExecutionVisualizer::showMe()
{
   this->show();
}
