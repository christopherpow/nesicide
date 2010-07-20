#include "cgraphicsbank.h"
#include "cnesicideproject.h"
CGraphicsBank::CGraphicsBank()
{
   m_isModified = false;
   m_tabIndex = -1;
   m_editor = (GraphicsBankEditorForm *)NULL;
   bankItems.clear();
}

CGraphicsBank::~CGraphicsBank()
{
   if (m_editor)
      delete m_editor;
}

bool CGraphicsBank::serialize(QDomDocument &doc, QDomNode &node)
{
   QDomElement graphicsBankElement = addElement( doc, node, "graphicsbank" );

   return true;
}

bool CGraphicsBank::deserialize(QDomDocument &doc, QDomNode &node)
{
   return true;
}

QString CGraphicsBank::caption() const
{
   return m_bankName;
}

void CGraphicsBank::contextMenuEvent(QContextMenuEvent *event, QTreeView *parent)
{
   QMenu menu(parent);
   menu.addAction("&Delete");

   QAction *ret = menu.exec(event->globalPos());
   if (ret)
   {
      if (ret->text() == "&Delete")
      {
         if (QMessageBox::question(parent, "Delete Source", "Are you sure you want to delete " + getBankName(),
                                   QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes)
            return;

         if (this->m_editor)
         {
            QTabWidget *tabWidget = (QTabWidget *)this->m_editor->parentWidget()->parentWidget();
            tabWidget->removeTab(m_tabIndex);
         }

         // TODO: Fix this logic so the memory doesn't get lost.
         nesicideProject->getProject()->getGraphics()->getGraphicsBanks()->removeChild(this);
         nesicideProject->getProject()->getGraphics()->getGraphicsBanks()->getGraphicsBankArray()->removeAll(this);
         ((CProjectTreeViewModel *)parent->model())->layoutChangedEvent();
      }
   }
}

void CGraphicsBank::openItemEvent(QTabWidget *tabWidget)
{
   if (m_editor)
   {
      if (m_editor->isVisible())
         tabWidget->setCurrentIndex(m_tabIndex);
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


   tabWidget->setCurrentIndex(m_tabIndex);
}

bool CGraphicsBank::onCloseQuery()
{
   if (m_isModified) {
      return (QMessageBox::question(0, QString("Confirm Close"),
                                    QString("This file has unsaved changes that\n"
                                            "will be lost if closed. Close anyway?"),
                                    QMessageBox::Yes, QMessageBox::Cancel) == QMessageBox::Yes);
   } else
      return true;
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
      bankItems.append(m_editor->chrRomBankItems.at(i));
   m_isModified = false;
}

bool CGraphicsBank::onNameChanged(QString newName)
{
   if (m_bankName != newName)
   {
      m_bankName = newName;
      if (m_editor && (m_tabIndex != -1))
      {
         QTabWidget * tabWidget = (QTabWidget *)m_editor->parentWidget()->parentWidget();
         tabWidget->setTabText(m_tabIndex, newName);
      }
   }
   return true;
}

QString CGraphicsBank::getBankName()
{
   return m_bankName;
}

void CGraphicsBank::setBankName(QString newName)
{
   m_bankName = newName;
}
