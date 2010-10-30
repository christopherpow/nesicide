#include "cgraphics.h"

CGraphics::CGraphics()
{
    m_pGraphicsBanks = new CGraphicsBanks();
    m_pGraphicsBanks->InitTreeItem(this);
    this->appendChild(m_pGraphicsBanks);
}

CGraphics::~CGraphics()
{
    if (m_pGraphicsBanks)
        delete m_pGraphicsBanks;
}

bool CGraphics::serialize(QDomDocument &doc, QDomNode &node)
{
    QDomElement sourcesElement = addElement( doc, node, "graphics" );

    if (m_pGraphicsBanks) {
        if (!m_pGraphicsBanks->serialize(doc, node))
            return false;
    }
    return true;
}

bool CGraphics::deserialize(QDomDocument &, QDomNode &)
{
    return true;
}

CGraphicsBanks *CGraphics::getGraphicsBanks()
{
    return m_pGraphicsBanks;
}

void CGraphics::setGraphicsBanks(CGraphicsBanks *newGraphicsBanks)
{
    m_pGraphicsBanks = newGraphicsBanks;
}
