#ifndef CDOCKWIDGETREGISTRY_H
#define CDOCKWIDGETREGISTRY_H

#include <QHash>
#include <QString>
#include <QDockWidget>
#include <QMutex>

class CDockWidgetRegistry
{
public:
   static CDockWidgetRegistry *_instance;
   static CDockWidgetRegistry *instance()
   {
      if ( !_instance )
      {
         _instance = new CDockWidgetRegistry();
      }
      return _instance;
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
