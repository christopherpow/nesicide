#include "ctilestamp.h"
#include "cnesicideproject.h"

#include "cimageconverters.h"

#include "environmentsettingsdialog.h"

CTileStamp::CTileStamp(IProjectTreeViewItem* parent)
{
   int idx;

   // Add node to tree
   InitTreeItem("",parent);

   // Allocate attributes
   m_xSize = 8;
   m_ySize = 8;
   m_attrTblUUID = "";

   // Initialize tile data.
   for ( idx = 0; idx < 16; idx++ )
   {
      m_tile.append((char)0x00);
   }

   // Initialize attribute data.
   for ( idx = 0; idx < 1; idx++ )
   {
      m_attr.append((char)0x00);
   }

   // Initialize property data.
   m_tileProperties = CNesicideProject::instance()->getTileProperties();

   m_grid = false;
}

CTileStamp::~CTileStamp()
{
}

QByteArray CTileStamp::getTileData()
{
   return m_tile;
}

QByteArray CTileStamp::getAttributeData()
{
   return m_attr;
}

QImage CTileStamp::getTileImage()
{
   return CImageConverters::toIndexed8(getTileData(),m_xSize,m_ySize);
}

bool CTileStamp::serialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement element = addElement( doc, node, "tile" );
   char byte[3];
   int idx;

   element.setAttribute("name", m_name);
   element.setAttribute("uuid", uuid());

   if ( m_editor && m_editor->isModified() )
   {
      editor()->onSave();
   }

   element.setAttribute("x",m_xSize);
   element.setAttribute("y",m_ySize);
   element.setAttribute("attrtbl",m_attrTblUUID);
   element.setAttribute("grid",m_grid);

   // Serialize tile properties.
   QDomElement tilePropertiesElement = addElement(doc,element,"tileproperties");

   foreach ( PropertyItem item, m_tileProperties )
   {
      QDomElement elm = addElement(doc,tilePropertiesElement,"property");
      elm.setAttribute("name",item.name);
      elm.setAttribute("type",item.type);
      elm.setAttribute("value",item.value);
   }

   // Serialize the tile data.
   QDomElement tileElement = addElement(doc,element,"tile");
   QDomCDATASection tileDataSect;
   QString tileDataMem;

   for ( idx = 0; idx < m_tile.count(); idx++ )
   {
      sprintf(byte,"%02X",(unsigned char)m_tile.at(idx));
      tileDataMem += byte;
   }
   tileDataSect = doc.createCDATASection(tileDataMem);
   tileElement.appendChild(tileDataSect);

   // Serialize the attribute data.
   QDomElement attrElement = addElement(doc,element,"attr");
   QDomCDATASection attrDataSect;
   QString attrDataMem;

   for ( idx = 0; idx < m_attr.count(); idx++ )
   {
      sprintf(byte,"%02X",(unsigned char)m_attr.at(idx));
      attrDataMem += byte;
   }
   attrDataSect = doc.createCDATASection(attrDataMem);
   attrElement.appendChild(attrDataSect);

   return serializeContent();
}

bool CTileStamp::serializeContent()
{
   if ( m_editor && m_editor->isModified() )
   {
      QDir dir(QDir::currentPath());
      QFile fileOut(dir.relativeFilePath(m_path));

      if ( fileOut.open(QIODevice::ReadWrite|QIODevice::Truncate) )
      {
         QTextStream out(&fileOut);
         out.setCodec(QTextCodec::codecForName(EnvironmentSettingsDialog::textEncodingString().toUtf8()));
//         out << sourceCode();

         fileOut.close();
      }

      m_editor->setModified(false);
   }

   return true;
}

bool CTileStamp::deserialize(QDomDocument&, QDomNode& node, QString& errors)
{
   QDomElement element = node.toElement();
   QDomNode child = element.firstChild();
   QDomNode cdataNode;
   QDomElement childsElement;
   QDomCDATASection cdataSection;
   QDomNode propertyChild;
   QDomElement propertyChildsElement;
   QString cdataString;
   char byte;
   int idx;

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

   if (!element.hasAttribute("x"))
   {
      errors.append("Missing required attribute 'x' of element <source name='"+element.attribute("name")+"'>\n");
      return false;
   }

   if (!element.hasAttribute("y"))
   {
      errors.append("Missing required attribute 'x' of element <source name='"+element.attribute("name")+"'>\n");
      return false;
   }

   m_name = element.attribute("name");

   setUuid(element.attribute("uuid"));

   m_xSize = element.attribute("x").toInt();
   m_ySize = element.attribute("y").toInt();
   m_attrTblUUID = element.attribute("attrtbl","");
   m_grid = element.attribute("grid","1").toInt();

   do
   {
      if ( child.nodeName() == "tile" )
      {
         cdataNode = child.firstChild();
         cdataSection = cdataNode.toCDATASection();
         cdataString = cdataSection.data();
         m_tile.clear();
         while ( cdataString.length() )
         {
            byte = cdataString.left(2).toInt(0,16);
            cdataString = cdataString.right(cdataString.length()-2);
            m_tile.append(byte);
         }
      }
      else if ( child.nodeName() == "attr" )
      {
         cdataNode = child.firstChild();
         cdataSection = cdataNode.toCDATASection();
         cdataString = cdataSection.data();
         m_attr.clear();
         while ( cdataString.length() )
         {
            byte = cdataString.left(2).toInt(0,16);
            cdataString = cdataString.right(cdataString.length()-2);
            m_attr.append(byte);
         }
      }
      else if ( child.nodeName() == "tileproperties" )
      {
         propertyChild = child.firstChild();

         do
         {
            propertyChildsElement = propertyChild.toElement();
            idx = 0;

            // Find the property in the local properties.
            foreach ( PropertyItem item, m_tileProperties )
            {
               if ( item.name == propertyChildsElement.attribute("name") )
               {
                  item.type = (propertyTypeEnum)propertyChildsElement.attribute("type").toInt();
                  item.value = propertyChildsElement.attribute("value");
                  m_tileProperties.replace(idx,item);
               }
               idx++;
            }

         } while (!(propertyChild = propertyChild.nextSibling()).isNull());
      }
   }
   while (!(child = child.nextSibling()).isNull());

   return deserializeContent();
}

bool CTileStamp::deserializeContent()
{
   QDir dir(QDir::currentPath());
   QFile fileIn(dir.relativeFilePath(m_path));

   if ( fileIn.exists() && fileIn.open(QIODevice::ReadOnly) )
   {
      QTextStream in(&fileIn);
      in.setCodec(QTextCodec::codecForName(EnvironmentSettingsDialog::textEncodingString().toUtf8()));
//      setSourceCode(in.readAll());
      fileIn.close();
   }
   else
   {
      // CPTODO: provide a file dialog for finding the source
   }

   return true;
}

// CPTODO: IS COPY ACTION IN NEW INFRASTRUCTURE JSOLO
//void CTileStamp::contextMenuEvent(QContextMenuEvent* event, QTreeView* parent)
//{
//   const QString DELETE_MENU_ACTION_TEXT = "Delete";
//   const QString COPY_MENU_ACTION_TEXT = "Copy";

//   QMenu menu(parent);
//   menu.addAction(DELETE_MENU_ACTION_TEXT);
//   menu.addAction(COPY_MENU_ACTION_TEXT);

//   QAction* ret = menu.exec(event->globalPos());

//   if (ret)
//   {
//      if (ret->text() == DELETE_MENU_ACTION_TEXT)
//      {
//         if (QMessageBox::question(parent, "Delete Tile", "Are you sure you want to delete " + caption(),
//                                   QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes)
//         {
//            return;
//         }

//         if (m_editor)
//         {
//            QTabWidget* tabWidget = (QTabWidget*)m_editor->parentWidget()->parentWidget();
//            tabWidget->removeTab(tabWidget->indexOf(m_editor));
//         }

//         CNesicideProject::instance()->getProject()->getProjectPrimitives()->getTileStamps()->removeChild(this);
//         //((CProjectTreeViewModel*)parent->model())->layoutChangedEvent();
//         markForDeletion();
//         CNesicideProject::instance()->setDirty(true);
//      }
//      else if (ret->text() == COPY_MENU_ACTION_TEXT)
//      {
//#if 0
//         QString name = QInputDialog::getText(parent, "New Tile",
//                                              "What name would you like to use to identify this Tile?");

//         if (!name.isEmpty())
//         {
//            CTileStamp* pTileStamp = new CTileStamp(this);
//            pTileStamp->setName(name);
//            m_tileStamps.append(pTileStamp);
//            appendChild(pTileStamp);
//            ((CProjectTreeViewModel*)parent->model())->layoutChangedEvent();
//            CNesicideProject::instance()->setDirty(true);
//         }
//#endif
//      }
//   }
//}

void CTileStamp::openItemEvent(CProjectTabWidget* tabWidget)
{
   if (m_editor)
   {
      tabWidget->setCurrentWidget(m_editor);
   }
   else
   {
      m_editor = new TileStampEditorForm(m_tile,m_attr,m_attrTblUUID,m_tileProperties,m_xSize,m_ySize,m_grid,this);
      tabWidget->addTab(m_editor, this->caption());
      tabWidget->setCurrentWidget(m_editor);
   }
}

void CTileStamp::saveItemEvent()
{
   m_tile = editor()->tileData();
   m_attr = editor()->attributeData();

   editor()->currentSize(&m_xSize,&m_ySize);
   m_attrTblUUID = editor()->currentAttributeTable().toString();
   m_grid = editor()->isGridEnabled();
   m_tileProperties = editor()->tileProperties();

   if ( m_editor )
   {
      m_editor->setModified(false);
   }
}

bool CTileStamp::onNameChanged(QString newName)
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

int CTileStamp::getChrRomBankItemSize()
{
   return getTileData().size();
}

QByteArray CTileStamp::getChrRomBankItemData()
{
   return getTileData();
}

QIcon CTileStamp::getChrRomBankItemIcon()
{
   return QIcon(":/resources/22_binary_file.png");
}

QImage CTileStamp::getChrRomBankItemImage()
{
   return getTileImage();
}
