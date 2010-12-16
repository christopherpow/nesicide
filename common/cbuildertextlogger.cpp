#include "cbuildertextlogger.h"

CTextLogger generalTextLogger;
CTextLogger buildTextLogger;
CTextLogger debugTextLogger;

CTextLogger::CTextLogger()
   : m_textEdit(NULL)
{
}

void CTextLogger::clear()
{
   m_text.clear();

   if (m_textEdit)
   {
      m_textEdit->clear();
   }
}

void CTextLogger::write(QString text)
{
   m_text.append(text);
   emit updateText();
}

void CTextLogger::update()
{
   if (m_textEdit)
   {
      if ( !m_text.isEmpty() )
      {
         m_textEdit->appendHtml(m_text);
      }

      m_text.clear();
   }
}
