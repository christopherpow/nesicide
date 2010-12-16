#include "csourceitem.h"
#include "cnesicideproject.h"

#include "main.h"

CSourceItem::CSourceItem()
{
   m_indexOfTab = -1;
   m_name = "";
   m_codeEditorForm = (CodeEditorForm*)NULL;
}

CSourceItem::~CSourceItem()
{
   if (m_codeEditorForm)
   {
      delete m_codeEditorForm;
   }
}

QString CSourceItem::get_sourceCode()
{
   if (m_codeEditorForm)
   {
      m_sourceCode = m_codeEditorForm->get_sourceCode();
   }

   return m_sourceCode;
}

void CSourceItem::set_sourceCode(QString sourceCode)
{
   m_sourceCode = sourceCode;

   if (m_codeEditorForm)
   {
      m_codeEditorForm->set_sourceCode(sourceCode);
   }
}

int CSourceItem::getTabIndex()
{
   return m_indexOfTab;
}

bool CSourceItem::serialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement element = addElement( doc, node, "source" );
   element.setAttribute("name", m_name);
   element.setAttribute("uuid", getIdent());
   QDomCDATASection dataSect = doc.createCDATASection(get_sourceCode());
   element.appendChild(dataSect);
   return true;
}

bool CSourceItem::deserialize(QDomDocument&, QDomNode& node)
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

   QDomCDATASection cdata = element.firstChild().toCDATASection();

   if (cdata.isNull())
   {
      return false;
   }

   m_sourceCode = cdata.data();

   return true;
}

QString CSourceItem::caption() const
{
   return m_name;
}
void CSourceItem::contextMenuEvent(QContextMenuEvent* event, QTreeView* parent)
{
   QMenu menu(parent);
   menu.addAction("&Delete");

   QAction* ret = menu.exec(event->globalPos());

   if (ret)
   {
      if (ret->text() == "&Delete")
      {
         if (QMessageBox::question(parent, "Delete Source", "Are you sure you want to delete " + get_sourceName(),
                                   QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes)
         {
            return;
         }

         if (nesicideProject->getProject()->getMainSource() == this)
         {
            nesicideProject->getProject()->setMainSource((CSourceItem*)NULL);
         }

         if (this->m_codeEditorForm)
         {
            QTabWidget* tabWidget = (QTabWidget*)this->m_codeEditorForm->parentWidget()->parentWidget();
            tabWidget->removeTab(m_indexOfTab);
         }

         // TODO: Fix this logic so the memory doesn't get lost.
         nesicideProject->getProject()->getSources()->removeChild(this);
         nesicideProject->getProject()->getSources()->get_pointerToArrayOfSourceItems()->removeAll(this);
         ((CProjectTreeViewModel*)parent->model())->layoutChangedEvent();
      }
   }
}

void CSourceItem::openItemEvent(QTabWidget* tabWidget)
{
   if (m_codeEditorForm)
   {
      if (m_codeEditorForm->isVisible())
      {
         tabWidget->setCurrentIndex(m_indexOfTab);
      }
      else
      {
         m_indexOfTab = tabWidget->addTab(m_codeEditorForm, this->caption());
         tabWidget->setCurrentIndex(m_indexOfTab);
      }

      return;
   }
   else
   {
      m_codeEditorForm = new CodeEditorForm();
      m_codeEditorForm->set_sourceCode(m_sourceCode);
      m_indexOfTab = tabWidget->addTab(m_codeEditorForm, this->caption());
   }

   tabWidget->setCurrentIndex(m_indexOfTab);
}

QString CSourceItem::get_sourceName()
{
   return m_name;
}

void CSourceItem::set_sourceName(QString sourceName)
{
   m_name = sourceName;
}

bool CSourceItem::onCloseQuery()
{
   if (m_sourceCode != m_codeEditorForm->get_sourceCode())
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

void CSourceItem::onClose()
{
   if (m_codeEditorForm)
   {
      delete m_codeEditorForm;
      m_codeEditorForm = (CodeEditorForm*)NULL;
      m_indexOfTab = -1;
   }
}

bool CSourceItem::isDocumentSaveable()
{
   return true;
}

void CSourceItem::onSaveDocument()
{
   m_sourceCode = m_codeEditorForm->get_sourceCode();
}

bool CSourceItem::canChangeName()
{
   return true;
}

bool CSourceItem::onNameChanged(QString newName)
{
   if (m_name != newName)
   {
      m_name = newName;

      if (m_codeEditorForm && (m_indexOfTab != -1))
      {
         QTabWidget* tabWidget = (QTabWidget*)m_codeEditorForm->parentWidget()->parentWidget();
         tabWidget->setTabText(m_indexOfTab, newName);
      }
   }

   return true;
}
