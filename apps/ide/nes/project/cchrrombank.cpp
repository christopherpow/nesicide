#include <QFileDialog>

#include "cchrrombank.h"
#include "cimageconverters.h"
#include "cobjectregistry.h"
#include "main.h"

CCHRROMBank::CCHRROMBank(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem("",parent);

   // Allocate attributes
}

CCHRROMBank::~ CCHRROMBank()
{
}

bool CCHRROMBank::serialize(QDomDocument& /*doc*/, QDomNode& /*node*/)
{
   // Don't carry the ROM data around with the project.
   return true;
}

bool CCHRROMBank::deserialize(QDomDocument& /*doc*/, QDomNode& /*node*/, QString& /*errors*/)
{
   // Don't carry the ROM data around with the project.
   return true;
}

QString CCHRROMBank::caption() const
{
   return "CHR Bank " + QString::number(m_bankIndex, 10);
}

// CPTODO: CHECK TO MAKE SURE THIS IS IN NEW INFRASTRUCTURE JSOLO
//void CCHRROMBank::contextMenuEvent(QContextMenuEvent* event, QTreeView* parent)
//{
//   const QString EXPORT_PNG_TEXT = "Export as PNG";
//   const QString IMPORT_PNG_TEXT = "Import from PNG";

//   QMenu menu(parent);
//   menu.addAction(EXPORT_PNG_TEXT);
//   menu.addAction(IMPORT_PNG_TEXT);

//   QAction* ret = menu.exec(event->globalPos());

//   if (ret)
//   {
//      if (ret->text() == EXPORT_PNG_TEXT)
//      {
//         exportAsPNG();
//      }
//      else if ( ret->text() == IMPORT_PNG_TEXT)
//      {
//         importFromPNG();
//      }
//   }
//}

void CCHRROMBank::exportAsPNG()
{
   QString fileName = QFileDialog::getSaveFileName(NULL,"Export CHR-ROM Bank as PNG",QDir::currentPath(),"PNG Files (*.png)");

   if ( !fileName.isEmpty() )
   {
#if QT_VERSION >= 0x040700
      QByteArray chrData;
      chrData.setRawData((const char*)getBankData(),MEM_8KB);
#else
      QByteArray chrData((const char*)getBankData(),MEM_8KB);
#endif
      QImage imgOut = CImageConverters::toIndexed8(chrData);

      imgOut.save(fileName,"png");
   }
}

void CCHRROMBank::importFromPNG()
{
   NESEmulatorThread* emulator = dynamic_cast<NESEmulatorThread*>(CObjectRegistry::instance()->getObject("Emulator"));
   QString fileName = QFileDialog::getOpenFileName(NULL,"Import CHR-ROM Bank from PNG",QDir::currentPath(),"PNG Files (*.png)");
   QByteArray chrData;
   QByteArray imgData;
   QImage imgIn;
   CCHRROMBanks* chrRomBanks = nesicideProject->getCartridge()->getChrRomBanks();

   if ( !fileName.isEmpty() )
   {
      imgIn.load(fileName);

      chrData = CImageConverters::fromIndexed8(imgIn);

      setBankData(chrData.constData());

      emulator->primeEmulator();
   }
}

void CCHRROMBank::openItemEvent(CProjectTabWidget* tabWidget)
{
   if (m_editor)
   {
      tabWidget->setCurrentWidget(m_editor);
   }
   else
   {
      m_editor = new CHRROMDisplayDialog(false, (qint8*)m_bankData,this);
      tabWidget->addTab(m_editor, this->caption());
      tabWidget->setCurrentWidget(m_editor);
   }
}
