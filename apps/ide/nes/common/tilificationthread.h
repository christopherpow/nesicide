#ifndef TILIFICATIONTHREAD_H
#define TILIFICATIONTHREAD_H

#include <QThread>

#include "ichrrombankitem.h"

class TilificationThread : public QThread
{
   Q_OBJECT
public:
   explicit TilificationThread(int side,QObject *parent = 0);

protected:
   void run();

signals:
   void tilificationComplete(int side,QByteArray output);

public slots:
   void prepareToTilify(int side);
   void addToTilificator(int side,IChrRomBankItem* item);
   void tilify(int side);

private:
   int m_side;
   QList<IChrRomBankItem*> m_input;
   QByteArray m_output;
};

#endif // TILIFICATIONTHREAD_H
