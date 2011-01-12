#include "inspectorregistry.h"

QHash<QString,QDockWidget*> InspectorRegistry::inspectors;
QHash<QString,bool> InspectorRegistry::visibility;

QDockWidget* InspectorRegistry::getInspector(const QString& name)
{
   if ( inspectors.contains(name) )
   {
      return inspectors[name];
   }

   return 0;
}

void InspectorRegistry::addInspector(const QString& name, QDockWidget* inspector, bool visible)
{
   inspectors.insert ( name, inspector );
   visibility.insert ( name, visible );
}

void InspectorRegistry::hideAll()
{
   QHash<QString,QDockWidget*>::const_iterator i;
   QHash<QString,bool>::iterator j;

   for (i = inspectors.begin(); i != inspectors.end(); ++i)
   {
      i.value()->hide();
   }
   for (j = visibility.begin(); j != visibility.end(); ++j)
   {
      j.value() = false;
   }
}

void InspectorRegistry::saveVisibility()
{
   QHash<QString,QDockWidget*>::const_iterator i;
   QHash<QString,bool>::iterator j;

   for (i = inspectors.begin(), j = visibility.begin(); i != inspectors.end(); ++i, ++j)
   {
      j.value() = i.value()->isVisible();
   }
}

void InspectorRegistry::restoreVisibility()
{
   QHash<QString,QDockWidget*>::const_iterator i;
   QHash<QString,bool>::const_iterator j;

   for (i = inspectors.begin(), j = visibility.begin(); i != inspectors.end(); ++i, ++j)
   {
      if ( j.value() )
      {
         i.value()->show();
      }
   }
}

bool InspectorRegistry::visible(const QString& name)
{
   return visibility.find(name).value();
}
