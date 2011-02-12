#include "cdockwidgetregistry.h"

QHash<QString,CDockWidgetRegistry::CDockWidgetManager*> CDockWidgetRegistry::widgets;

QDockWidget* CDockWidgetRegistry::getWidget(const QString& name)
{
   if ( widgets.contains(name) )
   {
      return widgets[name]->widget;
   }

   return 0;
}

void CDockWidgetRegistry::addWidget(const QString& name, QDockWidget* widget, bool visible)
{
   CDockWidgetManager* pDockWidgetManager = new CDockWidgetManager;
   pDockWidgetManager->widget = widget;
   pDockWidgetManager->visible = visible;
   pDockWidgetManager->enabled = false;
   pDockWidgetManager->flags = 0;
   
   widgets.insert ( name, pDockWidgetManager );
}

void CDockWidgetRegistry::enable()
{
   QHash<QString,CDockWidgetManager*>::const_iterator i;

   for (i = widgets.begin(); i != widgets.end(); ++i)
   {
      i.value()->widget->setEnabled(true);
   }
}

void CDockWidgetRegistry::disable(DisableReason reason)
{
   QHash<QString,CDockWidgetManager*>::const_iterator i;

   for (i = widgets.begin(); i != widgets.end(); ++i)
   {
      if ( ((reason == DockWidgetDisableForEmulatorRun) && (i.value()->flags&DockWidgetDisabledOnEmulatorRun)) ||
           ((reason == DockWidgetDisableForCompileError) && (i.value()->flags&DockWidgetDisabledOnCompileError)) )
      {
         i.value()->widget->setEnabled(false);
      }
   }
}

void CDockWidgetRegistry::hideAll()
{
   QHash<QString,CDockWidgetManager*>::const_iterator i;

   for (i = widgets.begin(); i != widgets.end(); ++i)
   {
      i.value()->widget->hide();
      i.value()->visible = false;
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

void CDockWidgetRegistry::setFlags(const QString& name,
                                   Flags flags)
{
   QHash<QString,CDockWidgetManager*>::const_iterator i;
   i = widgets.find(name);
   
   i.value()->flags = flags;
}                                 

bool CDockWidgetRegistry::visible(const QString& name)
{
   return widgets.find(name).value()->visible;
}
