#include "ixmlserializable.h"

QDomElement addElement( QDomDocument& doc, QDomNode& node,
                        const QString& tag,
                        const QString& value )
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

