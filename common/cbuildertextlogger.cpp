#include "cbuildertextlogger.h"

CTextLogger* generalTextLogger;
CTextLogger* buildTextLogger;
CTextLogger* debugTextLogger;

CTextLogger::CTextLogger()
{
}

void CTextLogger::erase()
{
   emit eraseText();
}

void CTextLogger::write(QString text)
{
   emit updateText(text);
}
