#include "cdockwidgetregistry.h"

QHash<QString,CDockWidgetRegistry::CDockWidgetManager*> CDockWidgetRegistry::widgets;
QMutex CDockWidgetRegistry::mutex;

QWidget* CDockWidgetRegistry::getWidget(const QString& name)
{
   QWidget* widget = NULL;
   mutex.lock();
   if ( widgets.contains(name) )
   {
      widget = widgets[name]->widget;
   }
   mutex.unlock();

   return widget;
}

void CDockWidgetRegistry::addWidget(const QString& name, QWidget* widget, bool visible, bool permanent)
{
   CDockWidgetManager* pDockWidgetManager = new CDockWidgetManager;
   pDockWidgetManager->widget = widget;
   pDockWidgetManager->visible = visible;
   pDockWidgetManager->enabled = false;
   pDockWidgetManager->permanent = permanent;

   mutex.lock();
   widgets.insert ( name, pDockWidgetManager );
   mutex.unlock();
}

void CDockWidgetRegistry::removeWidget(const QString &name)
{
   mutex.lock();
   widgets.remove(name);
   mutex.unlock();
}

void CDockWidgetRegistry::hideAll()
{
   QHash<QString,CDockWidgetManager*>::const_iterator i;

   mutex.lock();
   for (i = widgets.begin(); i != widgets.end(); ++i)
   {
      if ( !i.value()->permanent )
      {
         i.value()->widget->hide();
         i.value()->visible = false;
      }
   }
   mutex.unlock();
}

void CDockWidgetRegistry::saveVisibility()
{
   QHash<QString,CDockWidgetManager*>::const_iterator i;

   mutex.lock();
   for (i = widgets.begin(); i != widgets.end(); ++i)
   {
      i.value()->visible = i.value()->widget->isVisible();
   }
   mutex.unlock();
}

void CDockWidgetRegistry::restoreVisibility()
{
   QHash<QString,CDockWidgetManager*>::const_iterator i;

   mutex.lock();
   for (i = widgets.begin(); i != widgets.end(); ++i)
   {
      if ( i.value()->visible )
      {
         i.value()->widget->show();
      }
   }
   mutex.unlock();
}

bool CDockWidgetRegistry::visible(const QString& name)
{
   bool visible = false;

   mutex.lock();
   visible = widgets.find(name).value()->visible;
   mutex.unlock();

   return visible;
}
