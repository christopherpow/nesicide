#include "cproject.h"

CProject::CProject()
{
   m_pProjectPrimitives = new CProjectPrimitives();
   m_pProjectPrimitives->InitTreeItem(this);
   this->appendChild(m_pProjectPrimitives);

   m_mainSource = (CSourceItem*)NULL;
   m_pSources = new CSources();
   m_pSources->InitTreeItem(this);
   this->appendChild(m_pSources);

   m_pBinaryFiles = new CBinaryFiles();
   m_pBinaryFiles->InitTreeItem(this);
   this->appendChild(m_pBinaryFiles);

   m_pGraphicsBanks = new CGraphicsBanks();
   m_pGraphicsBanks->InitTreeItem(this);
   this->appendChild(m_pGraphicsBanks);
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

CProjectPrimitives* CProject::getProjectPrimitives()
{
   return m_pProjectPrimitives;
}

void CProject::setProjectPrimitives(CProjectPrimitives* newProjectPrimitives)
{
   m_pProjectPrimitives = newProjectPrimitives;
}

CGraphicsBanks* CProject::getGraphicsBanks()
{
   return m_pGraphicsBanks;
}

void CProject::setGraphicsBanks(CGraphicsBanks* newGraphicsBanks)
{
   m_pGraphicsBanks = newGraphicsBanks;
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

void CProject::setSources(CSources* newSources)
{
   m_pSources = newSources;
}

CBinaryFiles* CProject::getBinaryFiles()
{
   return m_pBinaryFiles;
}

void CProject::setBinaryFiles(CBinaryFiles* newBinaryFiles)
{
   m_pBinaryFiles = newBinaryFiles;
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

   if (m_pSources)
   {
      delete m_pSources;
   }

   m_pSources = new CSources();
   m_pSources->InitTreeItem(this);

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
