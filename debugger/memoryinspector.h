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

public slots:
   void showMe(eMemoryType display);

protected:
   MemoryDisplayDialog* dialog;
   QFrame* frame;
   eMemoryType m_display;
};

#endif // MEMORYINSPECTOR_H
