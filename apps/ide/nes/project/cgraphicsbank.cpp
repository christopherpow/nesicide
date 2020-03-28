#include <QFileDialog>

#include "cgraphicsbank.h"
#include "cnesicideproject.h"

#include "cimageconverters.h"

#include "main.h"

CGraphicsBank::CGraphicsBank(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem("",parent);

   // Allocate attributes
   m_bankItems.clear();
   m_bankSize = MEM_8KB;
}

CGraphicsBank::~CGraphicsBank()
{
}

QList<IChrRomBankItem*> CGraphicsBank::getGraphics()
{
   return m_bankItems;
}

uint32_t CGraphicsBank::getSize()
{
   return m_bankSize;
}

bool CGraphicsBank::serialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement element = addElement( doc, node, "graphicsbank" );
   element.setAttribute("name", m_name);
   element.setAttribute("uuid", uuid());
   element.setAttribute("size", m_bankSize);

   if ( m_editor && m_editor->isModified() )
   {
      editor()->onSave();
   }

   for (int i=0; i < m_bankItems.count(); i++)
   {
      QDomElement graphicsItemElement = addElement( doc, element, "graphicitem" );
      IProjectTreeViewItem* projectItem = dynamic_cast<IProjectTreeViewItem*>(m_bankItems.at(i));
      graphicsItemElement.setAttribute("uuid", projectItem->uuid() );
   }

   return true;
}

void CGraphicsBank::exportAsPNG()
{
   QString fileName = QFileDialog::getSaveFileName(NULL,"Export Graphics Bank as PNG",QDir::currentPath());
   QByteArray chrData;
   QByteArray imgData;
   int idx;
   QImage imgOut;

   if ( !fileName.isEmpty() )
   {
      for ( idx = 0; idx < m_bankItems.count(); idx++ )
      {
         IChrRomBankItem* item = dynamic_cast<IChrRomBankItem*>(m_bankItems.at(idx));
         if ( item )
         {
            chrData += item->getChrRomBankItemData();
         }
      }
      imgOut = CImageConverters::toIndexed8(chrData);

      imgOut.save(fileName,"png");
   }
}

bool CGraphicsBank::deserialize(QDomDocument& /*doc*/, QDomNode& node, QString& errors)
{
   QDomElement element = node.toElement();

   if (element.isNull())
   {
      return false;
   }

   if (!element.hasAttribute("name"))
   {
      errors.append("Missing required attribute 'name' of element <source name='?'>\n");
      return false;
   }

   if (!element.hasAttribute("uuid"))
   {
      errors.append("Missing required attribute 'uuid' of element <source name='"+element.attribute("name")+"'>\n");
      return false;
   }

   // Default size is 8KB
   m_bankSize = element.attribute("size","8192").toInt();

   m_name = element.attribute("name");

   setUuid(element.attribute("uuid"));

   m_bankItems.clear();

   QDomNode childNode = node.firstChild();

   if (!childNode.isNull())
   {
      do
      {
         if (childNode.nodeName() == "graphicitem")
         {
            QDomElement graphicItem = childNode.toElement();

            IProjectTreeViewItem* projectItem = findProjectItem(graphicItem.attribute("uuid"));
            IChrRomBankItem* pItem = dynamic_cast<IChrRomBankItem*>(projectItem);

            if ( pItem )
            {
               m_bankItems.append(pItem);
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

QString CGraphicsBank::caption() const
{
   return m_name;
}

// CPTODO: CHECK TO MAKE SURE THIS IS IN NEW INFRASTRUCTURE JSOLO
//void CGraphicsBank::contextMenuEvent(QContextMenuEvent* event, QTreeView* parent)
//{
//   const QString EXPORT_PNG_TEXT    = "Export as PNG";
//   const QString DELETE_TEXT        = "&Delete";

//   QMenu menu(parent);
//   menu.addAction(EXPORT_PNG_TEXT);
//   menu.addSeparator();
//   menu.addAction(DELETE_TEXT);

//   QAction* ret = menu.exec(event->globalPos());

//   if (ret)
//   {
//      if (ret->text() == DELETE_TEXT)
//      {
//         if (QMessageBox::question(parent, "Delete Source", "Are you sure you want to delete " + caption(),
//                                   QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes)
//         {
//            return;
//         }

//         if (m_editor)
//         {
//            QTabWidget* tabWidget = (QTabWidget*)this->m_editor->parentWidget()->parentWidget();
//            tabWidget->removeTab(tabWidget->indexOf(m_editor));
//         }

//         // TODO: Fix this logic so the memory doesn't get lost.
//         nesicideProject->getProject()->getGraphicsBanks()->removeChild(this);
//         nesicideProject->getProject()->getGraphicsBanks()->getGraphicsBanks().removeAll(this);
//         //((CProjectTreeViewModel*)parent->model())->layoutChangedEvent();
//      }
//      else if (ret->text() == EXPORT_PNG_TEXT)
//      {
//         exportAsPNG();
//      }
//   }
//}

void CGraphicsBank::openItemEvent(CProjectTabWidget* tabWidget)
{
   if (m_editor)
   {
      tabWidget->setCurrentWidget(m_editor);
   }
   else
   {
      m_editor = new GraphicsBankEditorForm(m_bankSize,m_bankItems,this);
      tabWidget->addTab(m_editor, this->caption());
      tabWidget->setCurrentWidget(m_editor);
   }
}

void CGraphicsBank::saveItemEvent()
{
   m_bankItems = editor()->bankItems();
   m_bankSize = editor()->bankSize();

   if ( m_editor )
   {
      m_editor->setModified(false);
   }
}

bool CGraphicsBank::onNameChanged(QString newName)
{
   if (m_name != newName)
   {
      m_name = newName;

      if ( m_editor )
      {
         QTabWidget* tabWidget = (QTabWidget*)m_editor->parentWidget()->parentWidget();
         tabWidget->setTabText(tabWidget->indexOf(m_editor), newName);
      }
   }

   return true;
}
