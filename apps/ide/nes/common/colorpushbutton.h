#ifndef COLORPUSHBUTTON_H
#define COLORPUSHBUTTON_H

#include <QPushButton>

class ColorPushButton : public QPushButton
{
   Q_OBJECT
public:
   explicit ColorPushButton(QWidget *parent = 0);

   void setCurrentColor(QColor color) { m_color = color; }
   QColor currentColor() { return m_color; }

protected:
   void paintEvent(QPaintEvent *event);

protected:
   QColor m_color;

signals:

public slots:

};

#endif // COLORPUSHBUTTON_H
