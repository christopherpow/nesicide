#include "qkeymapitemedit.h"

QKeymapItemEdit::QKeymapItemEdit(QWidget* parent) :
   QLineEdit(parent)
{
   setReadOnly(true);
   setFocusPolicy(Qt::ClickFocus);
}

bool QKeymapItemEdit::event( QEvent* evt )
{
   // Consume all keypress events
   if ( (evt->type() == QEvent::KeyPress) ||
         (evt->type() == QEvent::KeyRelease))
   {
      QKeyEvent* ke = (QKeyEvent*)evt;
      QKeySequence ks(ke->key());

      setText(ks.toString());

      ke->accept();
      return true;
   }

   return QWidget::event( evt );
}
