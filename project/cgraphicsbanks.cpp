#include "cgraphicsbanks.h"

CGraphicsBanks::CGraphicsBanks(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem("",parent);
}

CGraphicsBanks::~CGraphicsBanks()
{
   // Remove any allocated children
   for ( int i = 0; i < m_graphicsBanks.count(); i++ )
   {
      delete m_graphicsBanks.at(i);
   }

   // Initialize this node's attributes
   m_graphicsBanks.clear();
}

void CGraphicsBanks::initializeProject()
{
   // Remove any allocated children
   for ( int i = 0; i < m_graphicsBanks.count(); i++ )
   {
      removeChild(m_graphicsBanks.at(i));
      delete m_graphicsBanks.at(i);
   }

   // Initialize this node's attributes
   m_graphicsBanks.clear();
}

void CGraphicsBanks::terminateProject()
{
   // Remove any allocated children
   for ( int i = 0; i < m_graphicsBanks.count(); i++ )
   {
      removeChild(m_graphicsBanks.at(i));
      delete m_graphicsBanks.at(i);
   }

   // Initialize this node's attributes
   m_graphicsBanks.clear();
}

bool CGraphicsBanks::serialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement graphicsBanksElement = addElement( doc, node, "graphicsbanks" );

   for (int i = 0; i < m_graphicsBanks.count(); i++)
   {
      if (!m_graphicsBanks.at(i)->serialize(doc, graphicsBanksElement))
      {
         return false;
      }
   }

   return true;
}

bool CGraphicsBanks::deserialize(QDomDocument& doc, QDomNode& node, QString& errors)
{
   QDomNode childNode = node.firstChild();

   if (!childNode.isNull())
   {
      do
      {
         if (childNode.nodeName() == "graphicsbank")
         {
            CGraphicsBank* pGraphicsBank = new CGraphicsBank(this);
            m_graphicsBanks.append(pGraphicsBank);
            appendChild(pGraphicsBank);

            if (!pGraphicsBank->deserialize(doc,childNode,errors))
            {
               return false;
            }

         }
         else
         {
            return false;
         }
      } while (!(childNode = childNode.nextSibling()).isNull());
   }

   return true;
}

void CGraphicsBanks::contextMenuEvent(QContextMenuEvent* event, QTreeView* parent)
{
   QMenu menu(parent);
   menu.addAction("&Add Bank");


   QAction* ret = menu.exec(event->globalPos());

   if (ret)
   {
      if (ret->text() == "&Add Bank")
      {
         QString bankName = QInputDialog::getText(parent, "New Bank",
                            "What name would you like to use to identify this bank?");

         if (!bankName.isEmpty())
         {
            CGraphicsBank* pGraphicsBank = new CGraphicsBank(this);
            pGraphicsBank->setName(bankName);
            m_graphicsBanks.append(pGraphicsBank);
            appendChild(pGraphicsBank);
            ((CProjectTreeViewModel*)parent->model())->layoutChangedEvent();
         }
      }
   }
}
