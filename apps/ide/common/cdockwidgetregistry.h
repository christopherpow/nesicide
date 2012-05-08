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
   } CDockWidgetManager;
   CDockWidgetRegistry() {};
   static QWidget* getWidget(const QString& name);
   static void     addWidget(const QString& name,
                             QWidget* widget,
                             bool visible = false);
   static void     removeWidget(const QString& name);
   static void         hideAll();
   static void         saveVisibility();
   static bool         visible(const QString& name);
   static void         restoreVisibility();

private:
   static QHash<QString,CDockWidgetManager*> widgets;
};

#endif // CDOCKWIDGETREGISTRY_H
