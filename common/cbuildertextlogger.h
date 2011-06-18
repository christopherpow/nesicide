#ifndef CBUILDERTEXTLOGGER_H
#define CBUILDERTEXTLOGGER_H

#include <QObject>

class CTextLogger : public QObject
{
   Q_OBJECT
public:
   CTextLogger();
   virtual void erase();
   virtual void write(QString text);

signals:
   void updateText(QString text);
   void eraseText();
};

extern CTextLogger* generalTextLogger;
extern CTextLogger* buildTextLogger;
extern CTextLogger* debugTextLogger;
extern CTextLogger* searchTextLogger;

#endif // CBUILDERTEXTLOGGER_H
