
#include "cprojecttreecontextmenu.h"

#include "model/cprojectmodel.h"

#include "model/cattributemodel.h"
#include "model/cbinaryfilemodel.h"
#include "model/ccartridgemodel.h"
#include "model/cfiltermodel.h"
#include "model/cgraphicsbankmodel.h"
#include "model/csourcefilemodel.h"
#include "model/ctilestampmodel.h"
#include "model/cmusicmodel.h"

#include <QMessageBox>
#include <QDir>
#include <newfiledialog.h>
#include <QInputDialog>
#include <QFileDialog>

#include "filepropertiesdialog.h"

const int INVALID_ACTION = -1;

//--------------------------------------------------------------------------------------
// Context menu strings
//--------------------------------------------------------------------------------------
const QString NEW_ACTION                  = "New";
const QString DELETE_ACTION               = "Delete %1";
const QString REMOVE_ACTION               = "Remove \"%1\" from Project";
const QString COPY_ACTION                 = "Copy";
const QString NEW_ITEM_ACTION             = "New %1...";
const QString ADD_EXISTING_ACTION         = "Add Existing";
const QString ADD_ITEM_ACTION             = "Add %1...";
const QString INSERT_FILTER_ACTION        = "Insert Filter here...";

const QString FILE_PROPERTIES_ACTION      = "Properties of \"%1\"";

const QString CONFIRM_REMOVE_CAPTION      = "Remove from Project";
const QString CONFIRM_REMOVE_TEXT         = "Are you sure you want to remove \"%1\" from the project?";
const QString NEW_ITEM_CAPTION            = "New %1";
const QString NEW_ITEM_TEXT               = "What name would you like to use to identify this %1?";
const QString DELETE_ITEM_CAPTION         = "Delete %1";
const QString DELETE_ITEM_TEXT            = "Are you sure you want to delete \"%1\" permanently?";

const QString NEW_SOURCE_MENU_TEXT        = "New Source";
const QString NEW_MUSIC_MENU_TEXT         = "New Music";
const QString IMPORT_FILES_MENU_TEXT      = "Add Existing File(s)";

const QString GRAPHICS_BANK = "Graphics Bank";
const QString PALETTE       = "Palette";
const QString SOURCE_FILE   = "Source File";
const QString BINARY_FILE   = "Binary File";
const QString TILE          = "Tile";
const QString SCREEN        = "Screen";
const QString MUSIC_FILE    = "Music File";

const QString DOTS          = "...";
//--------------------------------------------------------------------------------------
// Context menu dispatch
//--------------------------------------------------------------------------------------
CProjectTreeContextMenu::CProjectTreeContextMenu(QWidget* menuParent, QPoint pos, CProjectModel* project)
   : m_position(pos), m_parent(menuParent), m_project(project)
{
}

void CProjectTreeContextMenu::visit(CUuid &)
{
   // Unknown data item, show global menu only.
   QMenu menu(m_parent);
   appendGlobalMenuItems(&menu);
   menu.exec(m_position);
}

void CProjectTreeContextMenu::visit(CAttributeUuid &data)
{
   m_targetUuid = data.uuid;

   QMenu menu(m_parent);
   menu.addAction(DELETE_ACTION.arg(PALETTE), this, SLOT(deletePalette()));
   appendGlobalMenuItems(&menu);
   menu.exec(m_position);
}

void CProjectTreeContextMenu::visit(CBinaryFileUuid &data)
{
   m_targetUuid = data.uuid;

   QString name = m_project->getBinaryFileModel()->getFileName(data.uuid);

   QMenu menu(m_parent);
   menu.addAction(REMOVE_ACTION.arg(name), this, SLOT(removeBinaryFile()));
   appendGlobalMenuItems(&menu);
   menu.exec(m_position);
}

void CProjectTreeContextMenu::visit(CFilterUuid &data)
{
   m_targetUuid = data.uuid;

   QMenu menu(m_parent);
   appendGlobalMenuItems(&menu);
   menu.exec(m_position);
}

void CProjectTreeContextMenu::visit(CGraphicsBankUuid &data)
{
   m_targetUuid = data.uuid;

   QMenu menu(m_parent);
   menu.addAction(DELETE_ACTION.arg(GRAPHICS_BANK), this, SLOT(deleteGraphicsBank()));
   appendGlobalMenuItems(&menu);
   menu.exec(m_position);
}

void CProjectTreeContextMenu::visit(CSourceFileUuid &data)
{
   m_targetUuid = data.uuid;

   QString name = m_project->getSourceFileModel()->getFileName(data.uuid);

   QMenu menu(m_parent);
   menu.addAction(FILE_PROPERTIES_ACTION.arg(name), this, SLOT(fileProperties()));
   menu.addSeparator();
   menu.addAction(REMOVE_ACTION.arg(name), this, SLOT(removeSourceFile()));
   appendGlobalMenuItems(&menu);
   menu.exec(m_position);
}

void CProjectTreeContextMenu::visit(CTileStampUuid &data)
{
   m_targetUuid = data.uuid;

   QMenu menu(m_parent);
   menu.addAction(DELETE_ACTION.arg(TILE), this, SLOT(deleteTile()));
   appendGlobalMenuItems(&menu);
   menu.exec(m_position);
}

void CProjectTreeContextMenu::visit(CMusicFileUuid &data)
{
   m_targetUuid = data.uuid;

   QMenu menu(m_parent);
   menu.addAction(REMOVE_ACTION.arg(MUSIC_FILE), this, SLOT(removeMusicFile()));
   appendGlobalMenuItems(&menu);
   menu.exec(m_position);
}

void CProjectTreeContextMenu::visit(CChrRomUuid &)
{
}

void CProjectTreeContextMenu::visit(CPrgRomUuid &)
{
}

//--------------------------------------------------------------------------------------
// Modal dialogs
//--------------------------------------------------------------------------------------
void CProjectTreeContextMenu::appendGlobalMenuItems(QMenu *menu)
{
   menu->addSeparator();

   // New -> menu entry
   QMenu* newMenu = new QMenu(menu);
   newMenu->setTitle(NEW_ACTION);
   newMenu->addAction(PALETTE + DOTS, this, SLOT(newPalette()) );
   newMenu->addAction(SOURCE_FILE + DOTS, this, SLOT(newSourceFile()) );
   newMenu->addAction(SCREEN + DOTS, this, SLOT(newScreen()) );
   newMenu->addAction(TILE + DOTS, this, SLOT(newTile()) );
   newMenu->addAction(GRAPHICS_BANK + DOTS, this, SLOT(newGraphicsBank()) );
   newMenu->addAction(MUSIC_FILE + DOTS, this, SLOT(newMusicFile()) );
   menu->addMenu(newMenu);

   // Add exisiting -> menu entry
   QMenu* addExistingMenu = new QMenu(menu);
   addExistingMenu->setTitle(ADD_EXISTING_ACTION);
   addExistingMenu->addAction(BINARY_FILE + DOTS, this, SLOT(addBinaryFile()) );
   addExistingMenu->addAction(SOURCE_FILE + DOTS, this, SLOT(addSourceFile()) );
   addExistingMenu->addAction(MUSIC_FILE + DOTS, this, SLOT(addMusicFile()) );
   menu->addMenu(addExistingMenu);

   //menu->addSeparator();
   //menu->addAction(INSERT_FILTER_ACTION);
}

bool CProjectTreeContextMenu::confirmChoice(const QString &caption, const QString &text)
{
   int result = QMessageBox::question(m_parent, caption, text, QMessageBox::Yes, QMessageBox::No);
   return result == QMessageBox::Yes;
}

QList<QString> CProjectTreeContextMenu::selectExistingFiles(const QString &caption)
{
   return QFileDialog::getOpenFileNames(NULL, caption, QDir::currentPath(), "All Files (*.*)");
}

QString CProjectTreeContextMenu::selectNewFileName()
{
   return QString();
}

QString CProjectTreeContextMenu::selectNewItemName(const QString &caption, const QString &text)
{
   return QInputDialog::getText(m_parent, caption, text);
}

void CProjectTreeContextMenu::newGraphicsBank()
{
   QString name = selectNewItemName(NEW_ITEM_CAPTION.arg(GRAPHICS_BANK), NEW_ITEM_TEXT.arg(GRAPHICS_BANK));
   if (name.isEmpty())
      return;

   m_project->getGraphicsBankModel()->newGraphicsBank(name);
}

void CProjectTreeContextMenu::newPalette()
{
   QString name = selectNewItemName(NEW_ITEM_CAPTION.arg(PALETTE), NEW_ITEM_TEXT.arg(PALETTE));
   if (name.isEmpty())
      return;

   m_project->getAttributeModel()->newPalette(name);
}

void CProjectTreeContextMenu::newSourceFile()
{
   NewFileDialog dlg(NEW_SOURCE_MENU_TEXT, "", QDir::currentPath());

   int result = dlg.exec();
   if ( result == 0 )
      return;

   QString fileName = dlg.getName();
   QDir newDir( dlg.getPath() );

   if ( fileName.isEmpty() )
      return;

   // Project base directory (directory where the .nesproject file is)
   QDir dir( QDir::fromNativeSeparators( QDir::currentPath() ) );
   QString fullPath = dir.relativeFilePath( newDir.absoluteFilePath( fileName ) );

   m_project->getSourceFileModel()->newSourceFile(fullPath);
}

void CProjectTreeContextMenu::newTile()
{
   QString name = selectNewItemName(NEW_ITEM_CAPTION.arg(TILE), NEW_ITEM_TEXT.arg(TILE));
   if (name.isEmpty())
      return;

   m_project->getTileStampModel()->newTileStamp(name);
}

void CProjectTreeContextMenu::newScreen()
{
   QString name = selectNewItemName(NEW_ITEM_CAPTION.arg(SCREEN), NEW_ITEM_TEXT.arg(SCREEN));
   if (name.isEmpty())
      return;

   m_project->getTileStampModel()->newScreen(name);
}

void CProjectTreeContextMenu::newMusicFile()
{
   // TODO Remove this hack!
   NewFileDialog dlg(NEW_MUSIC_MENU_TEXT, "", QDir::currentPath());

   int result = dlg.exec();
   if ( result == 0 )
      return;

   QString fileName = dlg.getName();
   QDir newDir( dlg.getPath() );

   if ( fileName.isEmpty() )
      return;

   // Project base directory (directory where the .nesproject file is)
   QDir dir( QDir::fromNativeSeparators( QDir::currentPath() ) );
   QString fullPath = dir.relativeFilePath( newDir.absoluteFilePath( fileName ) );

   m_project->getMusicModel()->newMusicFile(fullPath);
}

void CProjectTreeContextMenu::addBinaryFile()
{
   QStringList fileNames = selectExistingFiles(IMPORT_FILES_MENU_TEXT);
   foreach ( QString fileName, fileNames )
   {
      if (!fileName.isEmpty())
      {
         m_project->getBinaryFileModel()->addExistingBinaryFile(fileName);
      }
   }
}

void CProjectTreeContextMenu::addSourceFile()
{
   QStringList fileNames = selectExistingFiles(IMPORT_FILES_MENU_TEXT);
   foreach ( QString fileName, fileNames )
   {
      if (!fileName.isEmpty())
      {
         m_project->getSourceFileModel()->addExistingSourceFile(fileName);
      }
   }
}

void CProjectTreeContextMenu::fileProperties()
{
   m_project->getSourceFileModel()->fileProperties(m_targetUuid);
}

void CProjectTreeContextMenu::addMusicFile()
{
   QStringList fileNames = selectExistingFiles(IMPORT_FILES_MENU_TEXT);
   foreach ( QString fileName, fileNames )
   {
      if (!fileName.isEmpty())
      {
         m_project->getMusicModel()->addExistingMusicFile(fileName);
      }
   }
}

void CProjectTreeContextMenu::deleteGraphicsBank()
{
   QString name = m_project->getGraphicsBankModel()->getName(m_targetUuid);

   if (!confirmChoice(DELETE_ITEM_CAPTION.arg(GRAPHICS_BANK), DELETE_ITEM_TEXT.arg(name)))
      return;

   m_project->getGraphicsBankModel()->deleteGraphicsBank(m_targetUuid);
}

void CProjectTreeContextMenu::deletePalette()
{
   QString name = m_project->getAttributeModel()->getName(m_targetUuid);

   if (!confirmChoice(DELETE_ITEM_CAPTION.arg(PALETTE), DELETE_ITEM_TEXT.arg(name)))
      return;

   m_project->getAttributeModel()->deletePalette(m_targetUuid);
}

void CProjectTreeContextMenu::deleteTile()
{
   QString name = m_project->getTileStampModel()->getName(m_targetUuid);

   if (!confirmChoice(DELETE_ITEM_CAPTION.arg(TILE), DELETE_ITEM_TEXT.arg(name)))
      return;

   m_project->getTileStampModel()->deleteTileStamp(m_targetUuid);
}

void CProjectTreeContextMenu::removeMusicFile()
{
   QString name = m_project->getMusicModel()->getName(m_targetUuid);

   if (!confirmChoice(DELETE_ITEM_CAPTION.arg(TILE), DELETE_ITEM_TEXT.arg(name)))
      return;

   m_project->getMusicModel()->removeMusicFile(m_targetUuid);
}

void CProjectTreeContextMenu::removeBinaryFile()
{
   QString name = m_project->getBinaryFileModel()->getFileName(m_targetUuid);
   if (confirmChoice(CONFIRM_REMOVE_CAPTION, CONFIRM_REMOVE_TEXT.arg(name)))
   {
      m_project->getBinaryFileModel()->removeBinaryFile(m_targetUuid);
   }
}

void CProjectTreeContextMenu::removeSourceFile()
{
   QString name = m_project->getSourceFileModel()->getFileName(m_targetUuid);
   if (confirmChoice(CONFIRM_REMOVE_CAPTION, CONFIRM_REMOVE_TEXT.arg(name)))
   {
      m_project->getSourceFileModel()->removeSourceFile(m_targetUuid);
   }
}
