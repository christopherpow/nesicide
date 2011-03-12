#ifndef IXMLSERIALIZABLE_H
#define IXMLSERIALIZABLE_H

#include <QtXml>

#define IXMLEMPTYSPEC doc.firstChild(); node.firstChild();

class IXMLSerializable
{
public:
   QDomElement addElement( QDomDocument& doc,
                           QDomNode& node,
                           const QString& tag,
                           const QString& value = QString::null )
   {
      QDomElement el = doc.createElement( tag );
      node.appendChild( el );

      if ( !value.isNull() )
      {
         QDomText txt = doc.createTextNode( value );
         el.appendChild( txt );
      }

      return el;
   }

   virtual bool serialize(QDomDocument& doc, QDomNode& node) = 0;

   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors) = 0;
};

#endif // IXMLSERIALIZABLE_H
