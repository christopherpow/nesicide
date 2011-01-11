#include <QFileDialog>
#include <QFile>

#include "csources.h"

CSources::CSources(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem(parent);
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

bool CSources::deserialize(QDomDocument& doc, QDomNode& node)
{
   QDomNode childNode = node.firstChild();

   if (!childNode.isNull()) do
      {
         if (childNode.nodeName() == "source")
         {
            CSourceItem* pSourceItem = new CSourceItem(this);
            m_sourceItems.append(pSourceItem);
            appendChild(pSourceItem);

            if (!pSourceItem->deserialize(doc, childNode))
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

void CSources::contextMenuEvent(QContextMenuEvent* event, QTreeView* parent)
{
   QMenu menu(parent);
   menu.addAction("&New Source...");
   menu.addSeparator();
   menu.addAction("&Import Source from File...");

   QAction* ret = menu.exec(event->globalPos());

   if (ret)
   {
      if (ret->text() == "&New Source...")
      {
         QString sourceName = QInputDialog::getText(parent, "New Source",
                              "What name would you like to use to identify this source file?");

         if (!sourceName.isEmpty())
         {
            CSourceItem* pSourceItem = new CSourceItem(this);
            pSourceItem->setName(sourceName);
            m_sourceItems.append(pSourceItem);
            appendChild(pSourceItem);
            ((CProjectTreeViewModel*)parent->model())->layoutChangedEvent();
         }
      }
      else if (ret->text() == "&Import Source from File...")
      {
         QString fileName = QFileDialog::getOpenFileName(NULL, 0, 0);

         if (!fileName.isEmpty())
         {
            QFile fileIn (fileName);

            if (fileIn.exists() && fileIn.open(QIODevice::ReadOnly))
            {
               QDataStream fs(&fileIn);
               char* buffer = new char [ fileIn.size() ];

               fs.readRawData(buffer,fileIn.size());

               CSourceItem* pSourceItem = new CSourceItem(this);
               pSourceItem->setName(fileName);
               pSourceItem->set_sourceCode(buffer);
               m_sourceItems.append(pSourceItem);
               appendChild(pSourceItem);
               ((CProjectTreeViewModel*)parent->model())->layoutChangedEvent();

               delete [] buffer;
            }
         }
      }
   }
}
