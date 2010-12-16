#include "qkeymapitemedit.h"

QKeymapItemEdit::QKeymapItemEdit(QWidget* parent) :
   QLineEdit(parent)
{
   this->setReadOnly(true);
   this->setFocusPolicy(Qt::ClickFocus);
}

bool QKeymapItemEdit::event( QEvent* evt )
{
   // Consume all keypress events
   if ( (evt->type() == QEvent::KeyPress) ||
         (evt->type() == QEvent::KeyRelease))
   {
      QKeyEvent* ke = (QKeyEvent*)evt;

      if (!ke->text().trimmed().isEmpty())
      {
         this->setText(ke->text().toUpper());
      }
      else if (ke->key() != 0)
      {
         this->setText("$" + QString::number(ke->key(), 16));
      }
      else
      {
         this->setText("?");
      }

      ke->accept();
      return true;
   }

   return QWidget::event( evt );
}
