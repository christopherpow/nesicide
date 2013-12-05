#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>

class AppSettings : public QObject
{
   Q_OBJECT
public:
   AppSettings(QObject* parent = 0) : QObject(parent) {};
   virtual ~AppSettings() {};

   // Accessors
   enum AppMode
   {
      CodingMode,
      DebuggingMode
   };
   AppMode appMode() const { return m_appMode; }

   // Modifiers
   void setAppMode(AppMode mode);

protected:
   // Settings data structures.
   AppMode m_appMode;
   
signals:
   void appSettingsChanged();
};

#endif // APPSETTINGS_H
