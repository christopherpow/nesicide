#include "cbinaryfile.h"
#include "cnesicideproject.h"

#include "cimageconverters.h"

CBinaryFile::CBinaryFile(IProjectTreeViewItem* parent)
{
   // Add node to tree
   InitTreeItem("",parent);

   // Allocate attributes
   m_xSize = -1;
   m_ySize = -1;
}

CBinaryFile::~CBinaryFile()
{
}

QByteArray CBinaryFile::getBinaryData()
{
   return m_binaryData;
}

QImage CBinaryFile::getBinaryImage()
{
   return CImageConverters::toIndexed8(getBinaryData(),m_xSize,m_ySize);
}

void CBinaryFile::setBinaryData(const QByteArray& newBinaryData)
{
   QImage image;
   int tiles;
   int tilesX;
   int tilesY;

   image.loadFromData(newBinaryData);

   switch ( image.format() )
   {
   case QImage::Format_Indexed8:
      image.setColorCount(4);
      m_binaryData = CImageConverters::fromIndexed8(image);
      m_xSize = image.width();
      m_ySize = image.height();
      break;
   default:
      m_binaryData = newBinaryData;

      // Attempt to determine 'size' of binary data in tiles.
      tiles = newBinaryData.length()/16;
      tilesY = tiles/16;
      if ( tilesY >= 1 )
      {
         tilesX = 16;
      }
      else
      {
         tilesX = tiles%16;
      }
      if ( tilesY == 0 )
      {
         tilesY = 1;
      }
      if ( tilesY > 16 )
      {
         tilesY = 16;
         tilesX = 32;
      }
      m_xSize = tilesX*8;
      m_ySize = tilesY*8;
      break;
   }
}

bool CBinaryFile::serialize(QDomDocument& doc, QDomNode& node)
{
   QDomElement element = addElement( doc, node, "binaryfile" );

   element.setAttribute("name", m_name);
   element.setAttribute("path",m_path);
   element.setAttribute("uuid", uuid());

   // Serialize a size hint.
   element.setAttribute("xsize",m_xSize);
   element.setAttribute("ysize",m_ySize);

   // No need to serialize the content of binary files because there's
   // no way to modify their content within NESICIDE.

   return true;
}

bool CBinaryFile::deserialize(QDomDocument&, QDomNode& node, QString& errors)
{
   QDomElement element = node.toElement();

   if (element.isNull())
   {
      return false;
   }

   if (!element.hasAttribute("name"))
   {
      errors.append("Missing required attribute 'name' of element <binaryfile name='?'>\n");
      return false;
   }

   if (!element.hasAttribute("path"))
   {
      errors.append("Missing required attribute 'path' of element <binaryfile name='"+element.attribute("name")+"'>\n");
      return false;
   }

   if (!element.hasAttribute("uuid"))
   {
      errors.append("Missing required attribute 'uuid' of element <binaryfile name='"+element.attribute("name")+"'>\n");
      return false;
   }

   if (!element.hasAttribute("xsize"))
   {
      errors.append("Missing required attribute 'xsize' of element <binaryfile name='"+element.attribute("name")+"'>\n");
      return false;
   }

   if (!element.hasAttribute("ysize"))
   {
      errors.append("Missing required attribute 'ysize' of element <binaryfile name='"+element.attribute("name")+"'>\n");
      return false;
   }

   m_name = element.attribute("name");

   m_path = element.attribute("path");

   setUuid(element.attribute("uuid"));

   m_xSize = element.attribute("xsize").toInt();
   m_ySize = element.attribute("ysize").toInt();

   return deserializeContent();
}

bool CBinaryFile::deserializeContent()
{
   QDir dir(QDir::currentPath());
   QFile fileIn(dir.relativeFilePath(m_path));

   if ( fileIn.exists() && fileIn.open(QIODevice::ReadOnly) )
   {
      setBinaryData(fileIn.readAll());
      fileIn.close();
   }
   else
   {
      // CPTODO: provide a file dialog for finding the binary file
   }

   return true;
}

QString CBinaryFile::caption() const
{
   return m_name;
}

int CBinaryFile::getChrRomBankItemSize()
{
   return getBinaryData().size();
}

QByteArray CBinaryFile::getChrRomBankItemData()
{
   return getBinaryData();
}

QIcon CBinaryFile::getChrRomBankItemIcon()
{
   return QIcon(":/resources/22_binary_file.png");
}

QImage CBinaryFile::getChrRomBankItemImage()
{
   return getBinaryImage();
}
