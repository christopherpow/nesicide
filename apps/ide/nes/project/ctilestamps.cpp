#include <stdint.h>

#include "ctilestamps.h"

#include "main.h"

CTileStamps::CTileStamps(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem(":/resources/folder_closed.png",parent);
}

CTileStamps::~CTileStamps()
{
   // Remove any allocated children
   for ( int i = 0; i < m_tileStamps.count(); i++ )
   {
      delete m_tileStamps.at(i);
   }

   // Initialize this node's attributes
   m_tileStamps.clear();
}

void CTileStamps::initializeProject()
{
   // Remove any allocated children
   for ( int i = 0; i < m_tileStamps.count(); i++ )
   {
      removeChild(m_tileStamps.at(i));
      delete m_tileStamps.at(i);
   }

   // Initialize this node's attributes
   m_tileStamps.clear();
}

void CTileStamps::terminateProject()
{
   // Remove any allocated children
   for ( int i = 0; i < m_tileStamps.count(); i++ )
   {
      removeChild(m_tileStamps.at(i));
      delete m_tileStamps.at(i);
   }

   // Initialize this node's attributes
   m_tileStamps.clear();
}

bool CTileStamps::serialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement tileStampsElement = addElement( doc, node, "tiles" );

   for (int i = 0; i < m_tileStamps.count(); i++)
   {
      if (!m_tileStamps.at(i)->serialize(doc, tileStampsElement))
      {
         return false;
      }
   }

   return true;
}

bool CTileStamps::deserialize(QDomDocument& doc, QDomNode& node, QString& errors)
{
   QDomNode childNode = node.firstChild();

   if (!childNode.isNull())
   {
      do
      {
         if (childNode.nodeName() == "tile")
         {
            CTileStamp* pNewTileStamp = new CTileStamp(this);
            m_tileStamps.append(pNewTileStamp);
            appendChild(pNewTileStamp);

            if (!pNewTileStamp->deserialize(doc,childNode,errors))
            {
               return false;
            }
         }
         else
         {
            return false;
         }
      }
      while (!(childNode = childNode.nextSibling()).isNull());
   }

   return true;
}

QString CTileStamps::caption() const
{
   return QString("Tiles & Screens");
}

void CTileStamps::contextMenuEvent(QContextMenuEvent* event, QTreeView* parent)
{
   const QString NEW_TILE_MENU_TEXT    = "New Tile";
   const QString NEW_SCREEN_MENU_TEXT    = "New Screen";
   const QString IMPORT_FROM_FILE_TEXT = "Import from file";
   const QString IMPORT_FROM_EMULATOR_TEXT = "Import from Emulator";

   QMenu menu(parent);

   menu.addAction(NEW_TILE_MENU_TEXT);
   menu.addAction(NEW_SCREEN_MENU_TEXT);

   QAction* ret = menu.exec(event->globalPos());

   if (ret)
   {
      if (ret->text() == NEW_TILE_MENU_TEXT)
      {
         QString name = QInputDialog::getText(parent, "New Tile",
                                              "What name would you like to use to identify this Tile?");

         if (!name.isEmpty())
         {
            CTileStamp* pTileStamp = new CTileStamp(this);
            pTileStamp->setName(name);
            m_tileStamps.append(pTileStamp);
            appendChild(pTileStamp);
            //((CProjectTreeViewModel*)parent->model())->layoutChangedEvent();
            nesicideProject->setDirty(true);
         }
      }
      else if (ret->text() == NEW_SCREEN_MENU_TEXT)
      {
         QString name = QInputDialog::getText(parent, "New Screen",
                                              "What name would you like to use to identify this Screen?");

         if (!name.isEmpty())
         {
            CTileStamp* pTileStamp = new CTileStamp(this);
            pTileStamp->setName(name);
            pTileStamp->setSize(256,240);
            m_tileStamps.append(pTileStamp);
            appendChild(pTileStamp);
            //((CProjectTreeViewModel*)parent->model())->layoutChangedEvent();
            nesicideProject->setDirty(true);
         }
      }
   }
}
