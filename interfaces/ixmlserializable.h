#ifndef IXMLSERIALIZABLE_H
#define IXMLSERIALIZABLE_H

#include <QtXml>

#define IXMLEMPTYSPEC doc.firstChild(); node.firstChild();

QDomElement addElement( QDomDocument& doc, QDomNode& node,
                        const QString& tag,
                        const QString& value = QString::null );

class IXMLSerializable
{
public:
   virtual bool serialize(QDomDocument& doc, QDomNode& node) = 0;

   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors) = 0;
};

#endif // IXMLSERIALIZABLE_H
