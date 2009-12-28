#ifndef MEMORYINSPECTOR_H
#define MEMORYINSPECTOR_H

#include <QDockWidget>
#include <QFrame>
#include <QGridLayout>

#include "memorydisplaydialog.h"

class MemoryInspector : public QDockWidget
{
   Q_OBJECT
public:
   MemoryInspector(eMemoryType display);
   virtual ~MemoryInspector();
protected:
   MemoryDisplayDialog* dialog;
   QFrame* frame;
};

#endif // MEMORYINSPECTOR_H
