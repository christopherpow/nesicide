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
      m_textEdit->clear();
}

void CTextLogger::write(QString text, bool updateNow)
{
   m_text.append(text);
#if 0
   if ( updateNow )
   {
      update();
   }
#endif
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
