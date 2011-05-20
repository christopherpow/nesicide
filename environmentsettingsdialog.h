#ifndef ENVIRONMENTSETTINGSDIALOG_H
#define ENVIRONMENTSETTINGSDIALOG_H

#include <QDialog>
#include <QColorDialog>
#include <QFileDialog>

#include "qscilexerca65.h"

namespace Ui
{
class EnvironmentSettingsDialog;
}

class EnvironmentSettingsDialog : public QDialog
{
   Q_OBJECT
public:
   EnvironmentSettingsDialog(QWidget* parent = 0);
   ~EnvironmentSettingsDialog();

protected:
   void changeEvent(QEvent* e);

private:
   Ui::EnvironmentSettingsDialog* ui;
#if 0
   QColor getIdealTextColor(const QColor& rBackgroundColor) const;
#endif

   QsciScintilla* m_scintilla;
   QsciLexerCA65* m_lexer;

private slots:
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
