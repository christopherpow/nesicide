#include "cattributetable.h"
#include "cnesicideproject.h"

CAttributeTable::CAttributeTable(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem("",parent);

   // Allocate attributes
   m_palette.append(0x0D);
   m_palette.append(0x00);
   m_palette.append(0x10);
   m_palette.append(0x20);
   m_palette.append(0x0D);
   m_palette.append(0x00);
   m_palette.append(0x10);
   m_palette.append(0x20);
   m_palette.append(0x0D);
   m_palette.append(0x00);
   m_palette.append(0x10);
   m_palette.append(0x20);
   m_palette.append(0x0D);
   m_palette.append(0x00);
   m_palette.append(0x10);
   m_palette.append(0x20);
}

CAttributeTable::~CAttributeTable()
{
}

QList<uint8_t>& CAttributeTable::getPalette()
{
   return m_palette;
}

bool CAttributeTable::serialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement element = addElement( doc, node, "attributetable" );
   element.setAttribute("name", m_name);
   element.setAttribute("uuid", uuid());

   if ( m_editor && m_editor->isModified() )
   {
      editor()->onSave();
   }

   for (int i=0; i < m_palette.count(); i++)
   {
      QDomElement paletteItemElement = addElement( doc, element, "palette" );
      paletteItemElement.setAttribute("color",m_palette.at(i));
   }

   return true;
}

bool CAttributeTable::deserialize(QDomDocument&, QDomNode& node, QString& errors)
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

   m_name = element.attribute("name");

   setUuid(element.attribute("uuid"));

   m_palette.clear();

   QDomNode childNode = node.firstChild();

   if (!childNode.isNull())
   {
      do
      {
         if (childNode.nodeName() == "palette")
         {
            QDomElement paletteItem = childNode.toElement();
            uint8_t color = paletteItem.attribute("color","0").toInt();

            m_palette.append(color);
         }
         else
         {
            return false;
         }
      } while (!(childNode = childNode.nextSibling()).isNull());
   }

   return true;
}

void CAttributeTable::openItemEvent(CProjectTabWidget* tabWidget)
{
   if (m_editor)
   {
      tabWidget->setCurrentWidget(m_editor);
   }
   else
   {
      m_editor = new AttributeTableEditorForm(m_palette,this);
      tabWidget->addTab(m_editor, this->caption());
      tabWidget->setCurrentWidget(m_editor);
   }
}

void CAttributeTable::saveItemEvent()
{
   m_palette = editor()->attributeTable();

   if ( m_editor )
   {
      m_editor->setModified(false);
   }
}

bool CAttributeTable::onNameChanged(QString newName)
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
