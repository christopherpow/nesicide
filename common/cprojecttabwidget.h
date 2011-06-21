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
   void removeTab(int index);

signals:
   void tabModified(int tab,bool modified);
   void tabAdded(int tab);
   void tabRemoved(int index);

public slots:
   void tabModified(bool modified);
   void projectDirtied(bool dirtied);
};

#endif // CPROJECTTABWIDGET_H
