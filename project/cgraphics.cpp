#include "cgraphics.h"

CGraphics::CGraphics()
{
}

CGraphics::~CGraphics()
{
}

bool CGraphics::serialize(QDomDocument &doc, QDomNode &node)
{
    QDomElement sourcesElement = addElement( doc, node, "graphics" );
    return true;
}

bool CGraphics::deserialize(QDomDocument &, QDomNode &)
{
    return true;
}

