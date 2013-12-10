#include "cdockwidgetregistry.h"

QHash<QString,CDockWidgetRegistry::CDockWidgetManager*> CDockWidgetRegistry::widgets;

QWidget* CDockWidgetRegistry::getWidget(const QString& name)
{
   if ( widgets.contains(name) )
   {
      return widgets[name]->widget;
   }

   return 0;
}

void CDockWidgetRegistry::addWidget(const QString& name, QWidget* widget, bool visible, bool permanent)
{
   CDockWidgetManager* pDockWidgetManager = new CDockWidgetManager;
   pDockWidgetManager->widget = widget;
   pDockWidgetManager->visible = visible;
   pDockWidgetManager->enabled = false;
   pDockWidgetManager->permanent = permanent;

   widgets.insert ( name, pDockWidgetManager );
}

void CDockWidgetRegistry::removeWidget(const QString &name)
{
   widgets.remove(name);
}

void CDockWidgetRegistry::hideAll()
{
   QHash<QString,CDockWidgetManager*>::const_iterator i;

   for (i = widgets.begin(); i != widgets.end(); ++i)
   {
      if ( !i.value()->permanent )
      {
         i.value()->widget->hide();
         i.value()->visible = false;
      }
   }
}

void CDockWidgetRegistry::saveVisibility()
{
   QHash<QString,CDockWidgetManager*>::const_iterator i;

   for (i = widgets.begin(); i != widgets.end(); ++i)
   {
      i.value()->visible = i.value()->widget->isVisible();
   }
}

void CDockWidgetRegistry::restoreVisibility()
{
   QHash<QString,CDockWidgetManager*>::const_iterator i;

   for (i = widgets.begin(); i != widgets.end(); ++i)
   {
      if ( i.value()->visible )
      {
         i.value()->widget->show();
      }
   }
}

bool CDockWidgetRegistry::visible(const QString& name)
{
   return widgets.find(name).value()->visible;
}
