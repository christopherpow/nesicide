#include <stdint.h>

#include "csounds.h"

CSounds::CSounds(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem(":/resources/folder_closed.png",parent);

   // Allocate children
   m_pMusics = new CMusics(this);
}

CSounds::~CSounds()
{
   if (m_pMusics)
   {
      delete m_pMusics;
   }
}

void CSounds::initializeProject()
{
   // Initialize child nodes
   m_pMusics->initializeProject();

   // Add child nodes to tree
   appendChild(m_pMusics);
}

void CSounds::terminateProject()
{
   // Terminate child nodes
   m_pMusics->terminateProject();

   // Remove child nodes from tree
   removeChild(m_pMusics);
}

bool CSounds::serialize(QDomDocument& doc, QDomNode& node)
{
   // Create the root element for the CHR-ROM banks
   QDomElement soundsElement = addElement( doc, node, "sounds" );

   if (m_pMusics)
   {
      if (!m_pMusics->serialize(doc, soundsElement))
      {
         return false;
      }
   }

   return true;
}

bool CSounds::deserialize(QDomDocument& doc, QDomNode& node, QString& errors)
{
   QDomNode childNode;

   // Deserialization order is important but file order is not,
   // so we must take care of any possible XML ordering of items
   // here.  First, look for primitives.
   childNode = node.firstChild();
   do
   {
      if (childNode.nodeName() == "musics")
      {
         if (!m_pMusics->deserialize(doc,childNode,errors))
         {
            return false;
         }
      }
   }
   while (!(childNode = childNode.nextSibling()).isNull());

   return true;
}

QString CSounds::caption() const
{
   return QString("Sounds");
}
