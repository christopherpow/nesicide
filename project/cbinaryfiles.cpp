#include "cbinaryfiles.h"

#include "main.h"

CBinaryFiles::CBinaryFiles(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem(parent);
}

CBinaryFiles::~CBinaryFiles()
{
   // Remove any allocated children
   for ( int i = 0; i < m_binaryFiles.count(); i++ )
   {
      delete m_binaryFiles.at(i);
   }

   // Initialize this node's attributes
   m_binaryFiles.clear();
}

void CBinaryFiles::initializeProject()
{
   // Remove any allocated children
   for ( int i = 0; i < m_binaryFiles.count(); i++ )
   {
      removeChild(m_binaryFiles.at(i));
      delete m_binaryFiles.at(i);
   }

   // Initialize this node's attributes
   m_binaryFiles.clear();
}

void CBinaryFiles::terminateProject()
{
   // Remove any allocated children
   for ( int i = 0; i < m_binaryFiles.count(); i++ )
   {
      removeChild(m_binaryFiles.at(i));
      delete m_binaryFiles.at(i);
   }

   // Initialize this node's attributes
   m_binaryFiles.clear();
}

bool CBinaryFiles::serialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement binaryFilesElement = addElement( doc, node, "binaryfiles" );

   for (int i = 0; i < m_binaryFiles.count(); i++)
   {
      if (!m_binaryFiles.at(i)->serialize(doc, binaryFilesElement))
      {
         return false;
      }
   }

   return true;
}

bool CBinaryFiles::deserialize(QDomDocument& doc, QDomNode& node, QString& errors)
{
   QDomNode childNode = node.firstChild();

   if (!childNode.isNull()) do
      {
         if (childNode.nodeName() == "binaryfile")
         {

            CBinaryFile* pNewBinaryFile = new CBinaryFile(this);
            m_binaryFiles.append(pNewBinaryFile);
            appendChild(pNewBinaryFile);

            if (!pNewBinaryFile->deserialize(doc,childNode,errors))
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

QString CBinaryFiles::caption() const
{
   return QString("Binary Files");
}


void CBinaryFiles::contextMenuEvent(QContextMenuEvent* event, QTreeView* parent)
{
   // Project base directory (directory where the .nesproject file is)
   QDir dir( QDir::toNativeSeparators( QDir::currentPath() ) );

   const QString IMPORT_SOURCE_MENU_TEXT = "&Add an Existing File...";

   QMenu menu(parent);
   menu.addAction( IMPORT_SOURCE_MENU_TEXT );

   QAction* ret = menu.exec(event->globalPos());

   if (ret)
   {
      if (ret->text() == IMPORT_SOURCE_MENU_TEXT)
      {
         QString fileName = QFileDialog::getOpenFileName(NULL, "Add an Existing Binary File", QDir::currentPath(), "All Files (*.*)");

         if (!fileName.isEmpty())
         {
            CBinaryFile* pBinaryFile = new CBinaryFile(this);
            pBinaryFile->setName(dir.toNativeSeparators(dir.relativeFilePath(fileName)));

            pBinaryFile->setPath(dir.toNativeSeparators(dir.relativeFilePath(fileName)));

            pBinaryFile->deserializeContent();

            m_binaryFiles.append(pBinaryFile);
            appendChild(pBinaryFile);
            ((CProjectTreeViewModel*)parent->model())->layoutChangedEvent();
         }
      }
   }
}
