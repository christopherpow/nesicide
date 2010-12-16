#include "cgraphicsbank.h"
#include "cnesicideproject.h"

#include "main.h"

CGraphicsBank::CGraphicsBank()
{
   m_isModified = false;
   m_tabIndex = -1;
   m_editor = (GraphicsBankEditorForm*)NULL;
   bankItems.clear();
}

CGraphicsBank::~CGraphicsBank()
{
   if (m_editor)
   {
      delete m_editor;
   }
}

bool CGraphicsBank::serialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement element = addElement( doc, node, "graphicsbank" );
   element.setAttribute("name", m_name);
   element.setAttribute("uuid", getIdent());

   for (int i=0; i < bankItems.count(); i++)
   {
      QDomElement graphicsItemElement = addElement( doc, element, "graphicitem" );
      IProjectTreeViewItem* projectItem = dynamic_cast<IProjectTreeViewItem*>(bankItems.at(i));
      graphicsItemElement.setAttribute("uuid", projectItem->getIdent() );
   }

   return true;
}

bool CGraphicsBank::deserialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement element = node.toElement();

   if (element.isNull())
   {
      return false;
   }

   if (!element.hasAttribute("name"))
   {
      return false;
   }

   if (!element.hasAttribute("uuid"))
   {
      return false;
   }

   m_name = element.attribute("name");

   setIdent(element.attribute("uuid"));

   bankItems.clear();

   QDomNode childNode = node.firstChild();

   if (!childNode.isNull()) do
      {
         if (childNode.nodeName() == "graphicitem")
         {
            QDomElement graphicItem = childNode.toElement();

            IProjectTreeViewItem* projectItem = findProjectItem(graphicItem.attribute("uuid"));
            IChrRomBankItem* pItem = dynamic_cast<IChrRomBankItem*>(projectItem);

            if ( pItem )
            {
               bankItems.append(pItem);
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

QString CGraphicsBank::caption() const
{
   return m_name;
}

void CGraphicsBank::contextMenuEvent(QContextMenuEvent* event, QTreeView* parent)
{
   QMenu menu(parent);
   menu.addAction("&Delete");

   QAction* ret = menu.exec(event->globalPos());

   if (ret)
   {
      if (ret->text() == "&Delete")
      {
         if (QMessageBox::question(parent, "Delete Source", "Are you sure you want to delete " + getBankName(),
                                   QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes)
         {
            return;
         }

         if (this->m_editor)
         {
            QTabWidget* tabWidget = (QTabWidget*)this->m_editor->parentWidget()->parentWidget();
            tabWidget->removeTab(m_tabIndex);
         }

         // TODO: Fix this logic so the memory doesn't get lost.
         nesicideProject->getProject()->getGraphicsBanks()->removeChild(this);
         nesicideProject->getProject()->getGraphicsBanks()->getGraphicsBankArray()->removeAll(this);
         ((CProjectTreeViewModel*)parent->model())->layoutChangedEvent();
      }
   }
}

void CGraphicsBank::openItemEvent(QTabWidget* tabWidget)
{
   if (m_editor)
   {
      if (m_editor->isVisible())
      {
         tabWidget->setCurrentIndex(m_tabIndex);
      }
      else
      {
         m_tabIndex = tabWidget->addTab(m_editor, this->caption());
         tabWidget->setCurrentIndex(m_tabIndex);
      }

      return;
   }
   else
   {
      m_editor = new GraphicsBankEditorForm();
      m_tabIndex = tabWidget->addTab(m_editor, this->caption());
   }

   m_editor->updateChrRomBankItemList(bankItems);

   tabWidget->setCurrentIndex(m_tabIndex);
}

bool CGraphicsBank::onCloseQuery()
{
   if (m_isModified)
   {
      return (QMessageBox::question(0, QString("Confirm Close"),
                                    QString("This file has unsaved changes that\n"
                                            "will be lost if closed. Close anyway?"),
                                    QMessageBox::Yes, QMessageBox::Cancel) == QMessageBox::Yes);
   }
   else
   {
      return true;
   }
}

void CGraphicsBank::onClose()
{
   m_isModified = false;
}

int CGraphicsBank::getTabIndex()
{
   return m_tabIndex;
}

void CGraphicsBank::onSaveDocument()
{
   bankItems.clear();

   for (int i=0; i < m_editor->chrRomBankItems.count(); i++)
   {
      bankItems.append(m_editor->chrRomBankItems.at(i));
   }

   m_isModified = false;
}

bool CGraphicsBank::onNameChanged(QString newName)
{
   if (m_name != newName)
   {
      m_name = newName;

      if (m_editor && (m_tabIndex != -1))
      {
         QTabWidget* tabWidget = (QTabWidget*)m_editor->parentWidget()->parentWidget();
         tabWidget->setTabText(m_tabIndex, newName);
      }
   }

   return true;
}

QString CGraphicsBank::getBankName()
{
   return m_name;
}

void CGraphicsBank::setBankName(QString newName)
{
   m_name = newName;
}
