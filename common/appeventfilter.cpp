#include "appeventfilter.h"

AppEventFilter::AppEventFilter(QObject *parent) :
    QObject(parent)
{
   inActivationEvent = false;
}

bool AppEventFilter::eventFilter(QObject *obj, QEvent *event)
{
   if (!inActivationEvent && event->type() == QEvent::ApplicationActivate) {
      // The application activation can be handled here
      inActivationEvent = true;
      emit applicationActivationChanged(true);
      inActivationEvent = false;
      return true;
   }
   else if (!inActivationEvent && event->type() == QEvent::ApplicationDeactivate) {
      // The application deactivation can be handled here
      inActivationEvent = true;
      emit applicationActivationChanged(false);
      inActivationEvent = false;
      return true;
   }
   return QObject::eventFilter(obj, event); // Unhandled events are passed to the base class
}
