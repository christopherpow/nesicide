#ifndef ENVIRONMENTSETTINGSDIALOG_H
#define ENVIRONMENTSETTINGSDIALOG_H

#include <QDialog>
#include <QColorDialog>
#include <QFileDialog>

#include <Qsci/qscilexercpp.h>
#include "qscilexerca65.h"
#include "qscilexerdefault.h"

namespace Ui
{
class EnvironmentSettingsDialog;
}

class EnvironmentSettingsDialog : public QDialog
{
   Q_OBJECT
public:
   EnvironmentSettingsDialog(QWidget* parent = 0);
   virtual ~EnvironmentSettingsDialog();

   // Interface to retrieve values from QSettings and store them
   // here locally.
   static void readSettings();

   // Accessors
   static bool useInternalGameDatabase() { return m_useInternalGameDatabase; }
   static QString getGameDatabase() { return m_gameDatabase; }
   static bool showWelcomeOnStart() { return m_showWelcomeOnStart; }
   static bool saveAllOnCompile() { return m_saveAllOnCompile; }
   static bool rememberWindowSettings() { return m_rememberWindowSettings; }
   static bool trackRecentProjects() { return m_trackRecentProjects; }
   static QString romPath() { return m_romPath; }
   static bool runRomOnLoad() { return m_runRomOnLoad; }
   static bool followExecution() { return m_followExecution; }
   static int debuggerUpdateRate() { return m_debuggerUpdateRate; }
   static int soundBufferDepth() { return m_soundBufferDepth; }
   static QColor marginBackgroundColor() { return m_marginBackgroundColor; }
   static QColor marginForegroundColor() { return m_marginForegroundColor; }
   static bool lineNumbersEnabled() { return m_lineNumbersEnabled; }
   static QColor highlightBarColor() { return m_highlightBarColor; }
   static bool highlightBarEnabled() { return m_highlightBarEnabled; }
   static bool showSymbolTips() { return m_showSymbolTips; }
   static bool showOpcodeTips() { return m_showOpcodeTips; }
   static bool autoIndent() { return m_autoIndentEnabled; }
   static bool replaceTabs() { return m_tabReplacementEnabled; }
   static int spacesPerTab() { return m_spacesForTabs; }
   static QString sourceExtensionsForC() { return m_cSourceExtensions; }
   static QString sourceExtensionsForAssembly() { return m_asmSourceExtensions; }

   // Modifiers (only provided for settings that are also found in menus not just in this dialog)

protected:
   void changeEvent(QEvent* e);

private:
   Ui::EnvironmentSettingsDialog* ui;

   // Write settings from local to QSettings.
   void writeSettings();

   void setupCodeEditor(int index);

   QsciScintilla* m_scintilla;
   QsciLexerDefault* m_defaultLexer;
   QsciLexerCA65* m_ca65Lexer;
   QsciLexerCPP* m_cppLexer;
   QsciLexer* m_lexer;

   // Settings data structures.
   static int  m_lastActiveTab;
   static bool m_useInternalGameDatabase;
   static QString m_gameDatabase;
   static bool m_showWelcomeOnStart;
   static bool m_saveAllOnCompile;
   static bool m_rememberWindowSettings;
   static bool m_trackRecentProjects;
   static QString m_romPath;
   static bool m_runRomOnLoad;
   static bool m_followExecution;
   static int m_debuggerUpdateRate;
   static int m_soundBufferDepth;
   static QColor m_marginBackgroundColor;
   static QColor m_marginForegroundColor;
   static bool m_lineNumbersEnabled;
   static QColor m_highlightBarColor;
   static bool m_highlightBarEnabled;
   static bool m_showSymbolTips;
   static bool m_showOpcodeTips;
   static bool m_autoIndentEnabled;
   static bool m_tabReplacementEnabled;
   static int m_spacesForTabs;
   static QString m_cSourceExtensions;
   static QString m_asmSourceExtensions;

private slots:
   void on_language_currentIndexChanged(int index);
   void on_fontSize_valueChanged(int value);
   void on_showLineNumberMargin_toggled(bool checked);
   void on_showHighlightBar_toggled(bool checked);
   void on_highlightBarColor_clicked();
   void on_marginBackgroundColor_clicked();
   void on_marginForegroundColor_clicked();
   void on_backgroundColor_clicked();
   void on_styleColor_clicked();
   void on_styleFont_currentIndexChanged(QString font);
   void on_fontUnderline_toggled(bool checked);
   void on_fontItalic_toggled(bool checked);
   void on_fontBold_toggled(bool checked);
   void on_styleName_currentIndexChanged(int index);
   void on_soundBufferDepth_valueChanged(int value);
   void on_soundBufferDepth_sliderMoved(int position);
   void on_debuggerUpdateRate_valueChanged(int value);
   void on_debuggerUpdateRate_sliderMoved(int position);
   void on_ROMPathBrowse_clicked();
   void on_GameDatabasePathButton_clicked();
   void on_useInternalDB_toggled(bool checked);
   void on_buttonBox_accepted();
   void on_PluginPathButton_clicked();
};

#endif // ENVIRONMENTSETTINGSDIALOG_H
