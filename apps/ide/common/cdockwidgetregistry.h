#ifndef CDOCKWIDGETREGISTRY_H
#define CDOCKWIDGETREGISTRY_H

#include <QHash>
#include <QString>
#include <QDockWidget>
#include <QMutex>

class CDockWidgetRegistry
{
public:
   static CDockWidgetRegistry *instance;
   static CDockWidgetRegistry *getInstance()
   {
      if ( !instance )
      {
         instance = new CDockWidgetRegistry();
      }
      return instance;
   }
   typedef struct
   {
      QWidget* widget;
      bool     visible;
      bool     enabled;
      bool     permanent;
   } CDockWidgetManager;
   QWidget* getWidget(const QString& name);
   void     addWidget(const QString& name,
                             QWidget* widget,
                             bool visible = false,
                             bool permanent = false);
   void     removeWidget(const QString& name);
   void     hideAll();
   void     saveVisibility();
   bool     visible(const QString& name);
   void     restoreVisibility();

private:
   CDockWidgetRegistry();
   QHash<QString,CDockWidgetManager*> widgets;
   QMutex *mutex;
};

#endif // CDOCKWIDGETREGISTRY_H
