#include "colorpushbutton.h"

#include <QPainter>

ColorPushButton::ColorPushButton(QWidget *parent) :
   QPushButton(parent)
{
}

void ColorPushButton::paintEvent(QPaintEvent */*event*/)
{
   QPainter p(this);
   int w = width();       // width of cell in pixels
   int h = height();         // height of cell in pixels

   //   p.setPen( QPen( Qt::black, 0, Qt::SolidLine ) );
   //   p.drawRect(3, 3, w - 7, h - 7);
   //   p.fillRect(QRect(4, 4, w - 8, h - 8), QBrush(c));
   p.fillRect(QRect(2, 2, w - 4, h - 4), QBrush(m_color));

   if (isChecked())
   {
      p.setPen( QPen( Qt::black, 0, Qt::SolidLine ) );
      p.drawRect(0, 0, w - 1, h - 1);
      p.setPen( QPen( Qt::gray, 0, Qt::SolidLine ) );
      p.drawRect(1, 1, w - 2, h - 2);
   }

// CPTODO: eventually have enough here to put the color index on the button.
//   p.setPen( QPen( Qt::white, 1, Qt::SolidLine) );
//   p.drawText(rect(),m_text,Qt::AlignCenter);
}
