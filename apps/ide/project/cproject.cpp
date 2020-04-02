#include <stdint.h>

#include "cproject.h"
#include "cnesicideproject.h"

CProject::CProject(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem(":/resources/folder_closed.png",parent);

   // Initialize this node's attributes

   // Allocate children
   m_pProjectPrimitives = new CProjectPrimitives(this);
   m_pSources = new CSources(this);
   m_pBinaryFiles = new CBinaryFiles(this);
   m_pGraphicsBanks = new CGraphicsBanks(this);
   m_pSounds = new CSounds(this);
}

CProject::~CProject()
{
   if (m_pProjectPrimitives)
   {
      delete m_pProjectPrimitives;
   }

   if (m_pSources)
   {
      delete m_pSources;
   }

   if (m_pBinaryFiles)
   {
      delete m_pBinaryFiles;
   }

   if (m_pGraphicsBanks)
   {
      delete m_pGraphicsBanks;
   }

   if (m_pSounds)
   {
      delete m_pSounds;
   }
}

void CProject::initializeProject()
{
   // Initialize this node's attributes

   // Initialize child nodes
   m_pProjectPrimitives->initializeProject();
   m_pSources->initializeProject();
   m_pBinaryFiles->initializeProject();
   m_pGraphicsBanks->initializeProject();
   m_pSounds->initializeProject();

   // Add child nodes to tree
   appendChild(m_pProjectPrimitives);
   appendChild(m_pSources);
   appendChild(m_pBinaryFiles);
   appendChild(m_pGraphicsBanks);
   appendChild(m_pSounds);
}

void CProject::terminateProject()
{
   // Terminate child nodes
   m_pProjectPrimitives->terminateProject();
   m_pSources->terminateProject();
   m_pBinaryFiles->terminateProject();
   m_pGraphicsBanks->terminateProject();
   m_pSounds->terminateProject();

   // Remove child nodes from tree
   removeChild(m_pProjectPrimitives);
   removeChild(m_pSources);
   removeChild(m_pBinaryFiles);
   removeChild(m_pGraphicsBanks);
   removeChild(m_pSounds);
}

bool CProject::serialize(QDomDocument& doc, QDomNode& node)
{
   // Create the root element for the CHR-ROM banks
   QDomElement projectElement = addElement( doc, node, "project" );

   if (m_pProjectPrimitives)
   {
      if (!m_pProjectPrimitives->serialize(doc, projectElement))
      {
         return false;
      }
   }
   else
   {
      return false;
   }

   if (m_pSources)
   {
      if (!m_pSources->serialize(doc, projectElement))
      {
         return false;
      }
   }
   else
   {
      return false;
   }

   if (m_pBinaryFiles)
   {
      if (!m_pBinaryFiles->serialize(doc, projectElement))
      {
         return false;
      }
   }
   else
   {
      return false;
   }

   if (m_pGraphicsBanks)
   {
      if (!m_pGraphicsBanks->serialize(doc, projectElement))
      {
         return false;
      }
   }
   else
   {
      return false;
   }

   if (m_pSounds)
   {
      if (!m_pSounds->serialize(doc, projectElement))
      {
         return false;
      }
   }
   else
   {
      return false;
   }

   return true;
}

bool CProject::deserialize(QDomDocument& doc, QDomNode& node, QString& errors)
{
   QDomNode childNode;

   // Deserialization order is important but file order is not,
   // so we must take care of any possible XML ordering of items
   // here.  First, look for primitives.
   childNode = node.firstChild();
   do
   {
      if (childNode.nodeName() == "primitives")
      {
         if (!m_pProjectPrimitives->deserialize(doc,childNode,errors))
         {
            return false;
         }
      }
   }
   while (!(childNode = childNode.nextSibling()).isNull());

   // Next, look for sources.
   childNode = node.firstChild();
   do
   {
      if (childNode.nodeName() == "sources")
      {
         if (!m_pSources->deserialize(doc,childNode,errors))
         {
            return false;
         }
      }
   }
   while (!(childNode = childNode.nextSibling()).isNull());

   // Next, look for binary files.
   childNode = node.firstChild();
   do
   {
      if (childNode.nodeName() == "binaryfiles")
      {
         if (!m_pBinaryFiles->deserialize(doc,childNode,errors))
         {
            return false;
         }
      }
   }
   while (!(childNode = childNode.nextSibling()).isNull());

   // Next, look for graphics banks.
   childNode = node.firstChild();
   do
   {
      if (childNode.nodeName() == "graphicsbanks")
      {
         if (!m_pGraphicsBanks->deserialize(doc,childNode,errors))
         {
            return false;
         }
      }
   }
   while (!(childNode = childNode.nextSibling()).isNull());

   // Next, look for sound effects and music.
   childNode = node.firstChild();
   do
   {
      if (childNode.nodeName() == "sounds")
      {
         if (!m_pSounds->deserialize(doc,childNode,errors))
         {
            return false;
         }
      }
   }
   while (!(childNode = childNode.nextSibling()).isNull());

   return true;
}

QString CProject::caption() const
{
   return CNesicideProject::instance()->getProjectTitle()+" Project";
}
