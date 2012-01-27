#include <QFileDialog>
#include <QFile>

#include "newprojectdialog.h"

#include "csources.h"

#include "main.h"

CSources::CSources(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem(":/resources/folder_closed.png",parent);
}

CSources::~CSources()
{
   // Remove any allocated children
   for ( int i = 0; i < m_sourceItems.count(); i++ )
   {
      delete m_sourceItems.at(i);
   }

   // Initialize this node's attributes
   m_sourceItems.clear();
}

void CSources::initializeProject()
{
   // Remove any allocated children
   for ( int i = 0; i < m_sourceItems.count(); i++ )
   {
      removeChild(m_sourceItems.at(i));
      delete m_sourceItems.at(i);
   }

   // Initialize this node's attributes
   m_sourceItems.clear();
}

void CSources::terminateProject()
{
   // Remove any allocated children
   for ( int i = 0; i < m_sourceItems.count(); i++ )
   {
      removeChild(m_sourceItems.at(i));
      delete m_sourceItems.at(i);
   }

   // Initialize this node's attributes
   m_sourceItems.clear();
}

bool CSources::serialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement sourcesElement = addElement( doc, node, "sources" );

   for (int i = 0; i < m_sourceItems.count(); i++)
   {
      if (!m_sourceItems.at(i)->serialize(doc, sourcesElement))
      {
         return false;
      }
   }

   return true;
}

bool CSources::deserialize(QDomDocument& doc, QDomNode& node, QString& errors)
{
   QDomNode childNode = node.firstChild();

   if (!childNode.isNull()) do
      {
         if (childNode.nodeName() == "source")
         {
            CSourceItem* pSourceItem = new CSourceItem(this);
            m_sourceItems.append(pSourceItem);
            appendChild(pSourceItem);

            if (!pSourceItem->deserialize(doc,childNode,errors))
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

   return true;
}

QString CSources::caption() const
{
   return "Source Code";
}

void CSources::addSourceFile(QString fileName)
{
   QDir dir(QDir::currentPath());
   CSourceItem* pSourceItem = new CSourceItem(this);
   pSourceItem->setName(dir.fromNativeSeparators(dir.relativeFilePath(fileName)));

   pSourceItem->setPath(dir.fromNativeSeparators(dir.relativeFilePath(fileName)));

   pSourceItem->deserializeContent();

   m_sourceItems.append(pSourceItem);
   appendChild(pSourceItem);
   //((CProjectTreeViewModel*)parent->model())->layoutChangedEvent();
}

void CSources::contextMenuEvent(QContextMenuEvent* event, QTreeView* parent)
{
   // Project base directory (directory where the .nesproject file is)
   QDir dir( QDir::fromNativeSeparators( QDir::currentPath() ) );

   const QString NEW_SOURCE_MENU_TEXT    = "New Source";
   const QString IMPORT_SOURCE_MENU_TEXT = "Add Existing File(s)";

   QMenu menu(parent);
   menu.addAction( NEW_SOURCE_MENU_TEXT );
   menu.addSeparator();
   menu.addAction( IMPORT_SOURCE_MENU_TEXT );

   QAction* ret = menu.exec(event->globalPos());

   if (ret)
   {
      if (ret->text() == NEW_SOURCE_MENU_TEXT)
      {
         NewProjectDialog dlg(0,NEW_SOURCE_MENU_TEXT,"",QDir::currentPath());

         int result = dlg.exec();

         if ( result )
         {
            QString fileName = dlg.getName();
            QDir newDir(dlg.getPath());

            if ( !fileName.isEmpty() )
            {
               QString fullPath = dir.relativeFilePath( newDir.absoluteFilePath( fileName ) );
               QFile fileIn( fullPath );

               if ( fileIn.open(QIODevice::ReadWrite|QIODevice::Truncate|QIODevice::Text) )
               {
                  CSourceItem* pSourceItem = new CSourceItem(this);
                  pSourceItem->setName(fullPath);

                  pSourceItem->setPath(fullPath);

                  pSourceItem->serializeContent();

                  m_sourceItems.append(pSourceItem);
                  appendChild(pSourceItem);
                  ((CProjectTreeViewModel*)parent->model())->layoutChangedEvent();
               }
            }
         }
      }
      else if (ret->text() == IMPORT_SOURCE_MENU_TEXT)
      {
         QStringList fileNames = QFileDialog::getOpenFileNames(NULL, IMPORT_SOURCE_MENU_TEXT, QDir::currentPath(), "All Files (*.*)");

         foreach ( QString fileName, fileNames )
         {
            if (!fileName.isEmpty())
            {
               CSourceItem* pSourceItem = new CSourceItem(this);
               pSourceItem->setName(dir.fromNativeSeparators(dir.relativeFilePath(fileName)));

               pSourceItem->setPath(dir.fromNativeSeparators(dir.relativeFilePath(fileName)));

               pSourceItem->deserializeContent();

               m_sourceItems.append(pSourceItem);
               appendChild(pSourceItem);
               ((CProjectTreeViewModel*)parent->model())->layoutChangedEvent();
            }
         }
      }
   }
}
