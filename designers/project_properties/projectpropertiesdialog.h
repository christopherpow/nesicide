#ifndef PROJECTPROPERTIESDIALOG_H
#define PROJECTPROPERTIESDIALOG_H

#include <QDialog>
#include <QtXml>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

#include "cpaletteitemdelegate.h"
#include "cnesicideproject.h"
#include "csourceitem.h"

namespace Ui
{
class ProjectPropertiesDialog;
}

class ProjectPropertiesDialog : public QDialog
{
   Q_OBJECT
public:
   ProjectPropertiesDialog(QWidget* parent = 0);
   ~ProjectPropertiesDialog();

protected:
   void changeEvent(QEvent* e);
   void updateExampleInvocation();

private:
   Ui::ProjectPropertiesDialog* ui;
   void updateUI(int colid = -1);
   void setMainSource(QString mainSource);
   QString getMainSource();
   QDomElement addElement( QDomDocument& doc, QDomNode& node,
                           const QString& tag,
                           const QString& value = QString::null )
   {
      QDomElement el = doc.createElement( tag );
      node.appendChild( el );

      if ( !value.isNull() )
      {
         QDomText txt = doc.createTextNode( value );
         el.appendChild( txt );
      }

      return el;
   }
   QList<QColor> currentPalette;

private slots:
   void on_projectNameLineEdit_textEdited(QString );
   void on_projectOutputBasePathBrowse_clicked();
   void on_projectSourceBasePathBrowse_clicked();
   void on_projectBasePathBrowse_clicked();
   void on_buttonBox_accepted();
   void on_includePathBrowse_clicked();
   void on_includePaths_textChanged();
   void on_undefinedSymbols_textChanged();
   void on_definedSymbols_textChanged();
   void on_blueHorizontalSlider_actionTriggered(int action);
   void on_greenHorizontalSlider_actionTriggered(int action);
   void on_redHorizontalSlider_actionTriggered(int action);
   void on_ImportPalettePushButton_clicked();
   void on_exportPalettePushButton_clicked();
   void on_resetPalettePushButton_clicked();
   void on_tableWidget_cellClicked(int row, int column);
};

#endif // PROJECTPROPERTIESDIALOG_H
