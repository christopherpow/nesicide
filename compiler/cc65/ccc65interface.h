#ifndef CCC65INTERFACE_H
#define CCC65INTERFACE_H

#include <QProcess>

#include "iprojecttreeviewitem.h"

class CCC65Interface : public QObject
{
   Q_OBJECT
public:
   CCC65Interface();
   ~CCC65Interface();
   bool assemble();
};

#endif // CCC65INTERFACE_H
