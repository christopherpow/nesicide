#ifndef CDOCKWIDGETREGISTRY_H
#define CDOCKWIDGETREGISTRY_H

#include <QHash>
#include <QString>
#include <QDockWidget>

class CDockWidgetRegistry
{
public:
   typedef struct
   {
      QWidget* widget;
      bool     visible;
      bool     enabled;
      bool     permanent;
   } CDockWidgetManager;
   CDockWidgetRegistry() {};
   static QWidget* getWidget(const QString& name);
   static void     addWidget(const QString& name,
                             QWidget* widget,
                             bool visible = false,
                             bool permanent = false);
   static void     removeWidget(const QString& name);
   static void         hideAll();
   static void         saveVisibility();
   static bool         visible(const QString& name);
   static void         restoreVisibility();

private:
   static QHash<QString,CDockWidgetManager*> widgets;
   static QMutex mutex;
};

#endif // CDOCKWIDGETREGISTRY_H
