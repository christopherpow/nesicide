#include "cproject.h"

CProject::CProject(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem(parent);

   // Initialize this node's attributes
   m_mainSource = NULL;
   
   // Allocate children
   m_pProjectPrimitives = new CProjectPrimitives(this);
   m_mainSource = (CSourceItem*)NULL;
   m_pSources = new CSources(this);
   m_pBinaryFiles = new CBinaryFiles(this);
   m_pGraphicsBanks = new CGraphicsBanks(this);
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
}

void CProject::initializeProject()
{
   // Initialize this node's attributes
   m_mainSource = NULL;
   
   // Initialize child nodes
   m_pProjectPrimitives->initializeProject();
   m_pSources->initializeProject();
   m_pBinaryFiles->initializeProject();
   m_pGraphicsBanks->initializeProject();
   
   // Add child nodes to tree
   appendChild(m_pProjectPrimitives);
   appendChild(m_pSources);
   appendChild(m_pBinaryFiles);
   appendChild(m_pGraphicsBanks);
}

void CProject::terminateProject()
{
   // Terminate child nodes
   m_pProjectPrimitives->terminateProject();
   m_pSources->terminateProject();
   m_pBinaryFiles->terminateProject();
   m_pGraphicsBanks->terminateProject();
   
   // Remove child nodes from tree
   removeChild(m_pProjectPrimitives);
   removeChild(m_pSources);
   removeChild(m_pBinaryFiles);
   removeChild(m_pGraphicsBanks);
}

CProjectPrimitives* CProject::getProjectPrimitives()
{
   return m_pProjectPrimitives;
}

CGraphicsBanks* CProject::getGraphicsBanks()
{
   return m_pGraphicsBanks;
}

CSourceItem* CProject::getMainSource()
{
   return m_mainSource;
}

void CProject::setMainSource(CSourceItem* newSource)
{
   m_mainSource = newSource;
}

CSources* CProject::getSources()
{
   return m_pSources;
}

CBinaryFiles* CProject::getBinaryFiles()
{
   return m_pBinaryFiles;
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

   if (m_mainSource)
   {
      projectElement.setAttribute("mainsourceuuid", m_mainSource->getIdent());
   }

   return true;
}

bool CProject::deserialize(QDomDocument& doc, QDomNode& node)
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
         if (!m_pProjectPrimitives->deserialize(doc, childNode))
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
         if (!m_pSources->deserialize(doc, childNode))
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
         if (!m_pBinaryFiles->deserialize(doc, childNode))
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
         if (!m_pGraphicsBanks->deserialize(doc, childNode))
         {
            return false;
         }
      }
   }
   while (!(childNode = childNode.nextSibling()).isNull());

   QString mainSource = node.toElement().attribute("mainsourceuuid");
   m_mainSource = (CSourceItem*)NULL;

   for (int sourceIdx = 0; sourceIdx < m_pSources->childCount(); sourceIdx++)
   {
      if (mainSource == (((CSourceItem*)m_pSources->child(sourceIdx))->getIdent()))
      {
         m_mainSource = ((CSourceItem*)m_pSources->child(sourceIdx));
         break;
      }
   }

   return true;
}

QString CProject::caption() const
{
   return "Project";
}
