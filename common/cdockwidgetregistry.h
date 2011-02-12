#ifndef CDOCKWIDGETREGISTRY_H
#define CDOCKWIDGETREGISTRY_H

#include <QHash>
#include <QString>
#include <QDockWidget>

class CDockWidgetRegistry
{
public:
   enum Flag
   {
      DockWidgetDisabledOnEmulatorRun  = 0x00000001,
      DockWidgetDisabledOnCompileError = 0x00000002,
   };
   Q_DECLARE_FLAGS(Flags, Flag)
   enum DisableReason
   {
      DockWidgetDisableForEmulatorRun,
      DockWidgetDisableForCompileError
   };
   Q_DECLARE_FLAGS(DisableReasons, DisableReason)
   
   typedef struct
   {
      QDockWidget* widget;
      bool         visible;
      bool         enabled;
      QFlags<Flags>   flags;
   } CDockWidgetManager;
   CDockWidgetRegistry() {};
   static QDockWidget* getWidget(const QString& name);
   static void         addWidget(const QString& name, 
                                 QDockWidget* inspector, 
                                 bool visible = false);
   static void         hideAll();
   static void         saveVisibility();
   static bool         visible(const QString& name);
   static void         enable();
   static void         disable(DisableReason reason);
   static void         restoreVisibility();
   static void         setFlags(const QString& name,
                                Flags flags);
private:   
   static QHash<QString,CDockWidgetManager*> widgets;
};
   
   Q_DECLARE_OPERATORS_FOR_FLAGS(CDockWidgetRegistry::Flags)
   Q_DECLARE_OPERATORS_FOR_FLAGS(CDockWidgetRegistry::DisableReasons)  

#endif // CDOCKWIDGETREGISTRY_H
