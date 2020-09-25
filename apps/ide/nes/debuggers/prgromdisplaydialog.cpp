#include "prgromdisplaydialog.h"
#include "ui_prgromdisplaydialog.h"

#include "qscilexerbin.h"

#include "environmentsettingsdialog.h"

#include "codeeditorform.h"
#include "nes_emulator_core.h"

PRGROMDisplayDialog::PRGROMDisplayDialog(uint8_t* bankData,IProjectTreeViewItem* link,QWidget* parent) :
   CDesignerEditorBase(link,parent),
   m_data(0),
   ui(new Ui::PRGROMDisplayDialog)
{
   ui->setupUi(this);

   m_data = bankData;

   m_editor = new QHexEdit();

   m_editor->setData(QByteArray((const char*)m_data,MEM_8KB));
   m_editor->setReadOnly(true);
   m_editor->setAddressAreaBackgroundColor(EnvironmentSettingsDialog::marginBackgroundColor());
   m_editor->setAddressAreaForegroundColor(EnvironmentSettingsDialog::marginForegroundColor());
   m_editor->setHexCaps(true);

#if defined(Q_OS_MAC) || defined(Q_OS_MACX) || defined(Q_OS_MAC64)
   m_editor->setFont(QFont("Monaco", 11));
#endif
#ifdef Q_OS_LINUX
   m_editor->setFont(QFont("Monospace", 10));
#endif
#ifdef Q_OS_WIN
   m_editor->setFont(QFont("Consolas", 11));
#endif

   setCentralWidget(m_editor);
}

PRGROMDisplayDialog::~PRGROMDisplayDialog()
{
   delete ui;

   delete m_editor;
}

void PRGROMDisplayDialog::changeEvent(QEvent* e)
{
   QWidget::changeEvent(e);

   switch (e->type())
   {
      case QEvent::LanguageChange:
         ui->retranslateUi(this);
         break;
      default:
         break;
   }
}

void PRGROMDisplayDialog::showEvent(QShowEvent* /*e*/)
{
   m_editor->setData(QByteArray((const char*)m_data,MEM_8KB));
   m_editor->update();
}

void PRGROMDisplayDialog::applyEnvironmentSettingsToTab()
{
   m_editor->setAddressAreaBackgroundColor(EnvironmentSettingsDialog::marginBackgroundColor());
   m_editor->setAddressAreaForegroundColor(EnvironmentSettingsDialog::marginForegroundColor());
   m_editor->update();
}
