#ifndef INSPECTORREGISTRY_H
#define INSPECTORREGISTRY_H

#include <QHash>
#include <QString>
#include <QDockWidget>

class InspectorRegistry
{
public:
   InspectorRegistry() {};
   static QDockWidget* getInspector(const QString& name);
   static void         addInspector(const QString& name, QDockWidget* inspector, bool visible = false);
   static void         hideAll();
   static void         saveVisibility();
   static bool         visible(const QString& name);
   static void         enable();
   static void         disable();
   static void         restoreVisibility();

private:
   static QHash<QString,QDockWidget*> inspectors;
   static QHash<QString,bool>         visibility;
};

#endif // INSPECTORREGISTRY_H
