#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>

class AppSettings : public QObject
{
   Q_OBJECT
public:
   static AppSettings *instance()
   {
      if ( !_instance )
      {
         _instance = new AppSettings();
      }
      return _instance;
   }
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

private:
   static AppSettings *_instance;
   AppSettings(QObject* parent = 0) : QObject(parent) {};

signals:
   void appSettingsChanged();
};

#endif // APPSETTINGS_H
