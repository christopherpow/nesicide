#ifndef CPROJECTTABWIDGET_H
#define CPROJECTTABWIDGET_H

#include <QTabWidget>

class CProjectTabWidget : public QTabWidget
{
   Q_OBJECT
public:
   explicit CProjectTabWidget(QWidget *parent = 0);

   int addTab(QWidget *widget, const QIcon &icon, const QString &label);
   int addTab(QWidget *widget, const QString &label);

signals:

public slots:
   void tabModified(bool modified);
};

#endif // CPROJECTTABWIDGET_H
