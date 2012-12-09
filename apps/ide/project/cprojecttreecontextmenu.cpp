#include "cprojecttreecontextmenu.h"
#include "model/cprojectmodel.h"
#include "model/csourcefilemodel.h"
#include <QMessageBox>


const int INVALID_ACTION = -1;

//--------------------------------------------------------------------------------------
// Context menu strings
//--------------------------------------------------------------------------------------
const QString NEW_ACTION                  = "New";
const QString DELETE_ACTION               = "Delete";
const QString REMOVE_ACTION               = "Remove \"%1\" from Project";
const QString COPY_ACTION                 = "Copy";
const QString NEW_ITEM_ACTION             = "New %1...";
const QString ADD_EXISTING_ACTION         = "Add Existing";
const QString ADD_ITEM_ACTION             = "Add %1...";
const QString INSERT_FILTER_ACTION        = "Insert Filter here...";


const QString CONFIRM_REMOVE_CAPTION      = "Remove from Project";
const QString CONFIRM_REMOVE_TEXT         = "Are you sure you want to remove \"%1\" from the project?";

const QString GRAPHICS_BANK = "Graphics Bank";
const QString PALETTE       = "Palette";
const QString SOURCE_FILE   = "Source File";
const QString BINARY_FILE   = "Binary File";
const QString TILE          = "Tile";
const QString SCREEN        = "Screen";

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

void CProjectTreeContextMenu::visit(CAttributeUuid &)
{
}

void CProjectTreeContextMenu::visit(CBinaryFileUuid &data)
{
   m_targetUuid = data.uuid;

   QMenu menu(m_parent);
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

void CProjectTreeContextMenu::visit(CGraphicsBankUuid &)
{
}

void CProjectTreeContextMenu::visit(CSourceFileUuid &data)
{
   m_targetUuid = data.uuid;

   QString name = m_project->getSourceFileModel()->getFileName(data.uuid);

   QMenu menu(m_parent);
   menu.addAction(REMOVE_ACTION.arg(name), this, SLOT(removeSourceFile()));
   appendGlobalMenuItems(&menu);
   menu.exec(m_position);
}

void CProjectTreeContextMenu::visit(CTileStampUuid &data)
{
   m_targetUuid = data.uuid;

   QMenu menu(m_parent);
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
int CProjectTreeContextMenu::showMenu(const QString &a1, const QString &a2, const QString &a3)
{
   QMenu menu(m_parent);

   menu.addSeparator();

   // Local actions
   if (!a1.isEmpty())
      menu.addAction(a1);
   if (!a2.isEmpty())
      menu.addAction(a2);
   if (!a3.isEmpty())
      menu.addAction(a3);

   QAction* action = menu.exec(m_position);
   if (action == NULL)
      return INVALID_ACTION;

   if (action->text() == a1)
      return 0;
   else if (!a2.isEmpty() && action->text() == a2)
      return 1;
   else if (!a3.isEmpty() && action->text() == a3)
      return 2;

   // "New" actions
   // "Add Existing" actions
   // "Insert Filter here" action
   //if (action->text() == INSERT_FILTER_ACTION)

   return INVALID_ACTION;
}

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
   menu->addMenu(newMenu);

   // Add exisiting -> menu entry
   QMenu* addExistingMenu = new QMenu(menu);
   addExistingMenu->setTitle(ADD_EXISTING_ACTION);
   addExistingMenu->addAction(BINARY_FILE + DOTS, this, SLOT(addBinaryFile()) );
   addExistingMenu->addAction(SOURCE_FILE + DOTS, this, SLOT(addSourceFile()) );
   menu->addMenu(addExistingMenu);

   menu->addSeparator();
   menu->addAction(INSERT_FILTER_ACTION);
}

bool CProjectTreeContextMenu::confirmChoice(const QString &caption, const QString &text)
{
   int result = QMessageBox::question(m_parent, caption, text, QMessageBox::Yes, QMessageBox::No);
   return result == QMessageBox::Yes;
}

QList<QString> CProjectTreeContextMenu::selectExistingFiles(const QString &caption)
{
   return QList<QString>();
}

QString CProjectTreeContextMenu::selectNewFileName()
{
   return QString();
}

QString CProjectTreeContextMenu::selectNewItemName(const QString &caption, const QString &text)
{
   return QString();
}

void CProjectTreeContextMenu::newGraphicsBank()
{

}

void CProjectTreeContextMenu::newPalette()
{

}

void CProjectTreeContextMenu::newSourceFile()
{
}

void CProjectTreeContextMenu::newTile()
{
}

void CProjectTreeContextMenu::newScreen()
{
}


void CProjectTreeContextMenu::addBinaryFile()
{
}

void CProjectTreeContextMenu::addSourceFile()
{
}


void CProjectTreeContextMenu::deletePalette()
{
}

void CProjectTreeContextMenu::deleteTile()
{
}

void CProjectTreeContextMenu::removeBinaryFile()
{
}

void CProjectTreeContextMenu::removeSourceFile()
{
   QString name = m_project->getSourceFileModel()->getFileName(m_targetUuid);
   if (confirmChoice(CONFIRM_REMOVE_CAPTION, CONFIRM_REMOVE_TEXT.arg(name)))
   {
      m_project->getSourceFileModel()->removeSourceFile(m_targetUuid);
   }
}
