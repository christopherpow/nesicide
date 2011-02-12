#ifndef CCOMPILERINTERFACE_H
#define CCOMPILERINTERFACE_H

#include <QObject>

#include "pasm_lib.h"

class CCompilerInterface : public QObject
{
    Q_OBJECT
public:
    explicit CCompilerInterface(QObject *parent = 0);

signals:

public slots:

};

#endif // CCOMPILERINTERFACE_H
