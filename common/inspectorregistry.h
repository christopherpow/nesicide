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
   static void         addInspector(const QString& name, QDockWidget* inspector);

private:
   static QHash<QString,QDockWidget*> inspectors;
};

#endif // INSPECTORREGISTRY_H
