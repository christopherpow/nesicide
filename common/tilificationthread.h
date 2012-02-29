#ifndef TILIFICATIONTHREAD_H
#define TILIFICATIONTHREAD_H

#include <QThread>

#include "ichrrombankitem.h"

class TilificationThread : public QThread
{
   Q_OBJECT
public:
   explicit TilificationThread(QObject *parent = 0);

protected:
   void run();

signals:
   void tilificationComplete(QByteArray output);

public slots:
   void prepareToTilify();
   void addToTilificator(IChrRomBankItem* item);
   void tilify();

private:
   QList<IChrRomBankItem*> m_input;
   QByteArray m_output;
};

#endif // TILIFICATIONTHREAD_H
