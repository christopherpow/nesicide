#include "inspectorregistry.h"

QHash<QString,QDockWidget*> InspectorRegistry::inspectors;

QDockWidget* InspectorRegistry::getInspector(const QString& name)
{
   if ( inspectors.contains(name) )
   {
      return inspectors[name];
   }
   return 0;
}

void InspectorRegistry::addInspector(const QString& name, QDockWidget* inspector)
{
   inspectors.insert ( name, inspector );
}
