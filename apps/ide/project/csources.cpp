#include <QFileDialog>
#include <QFile>

#include "newfiledialog.h"

#include "csources.h"

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

CSourceItem *CSources::addSourceFile(QString fileName)
{
   QDir dir(QDir::currentPath());
   CSourceItem* pSourceItem = new CSourceItem(this);
   pSourceItem->setName(dir.fromNativeSeparators(dir.relativeFilePath(fileName)));

   pSourceItem->setPath(dir.fromNativeSeparators(dir.relativeFilePath(fileName)));

   pSourceItem->deserializeContent();

   m_sourceItems.append(pSourceItem);
   appendChild(pSourceItem);
   return pSourceItem;
   //((CProjectTreeViewModel*)parent->model())->layoutChangedEvent();
}

void CSources::removeSourceFile(CSourceItem *item)
{
   this->removeChild(item);
   m_sourceItems.removeAll(item);
   delete item;
}
