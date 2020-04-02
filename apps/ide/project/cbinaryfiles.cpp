#include "cbinaryfiles.h"

CBinaryFiles::CBinaryFiles(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem(":/resources/folder_closed.png",parent);
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
