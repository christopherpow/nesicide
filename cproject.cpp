#include "cproject.h"

CProject::CProject()
{
    m_pointerToSources = new CSources();
    m_pointerToSources->InitTreeItem(this);
    this->appendChild(m_pointerToSources);
}

CProject::~CProject()
{
    if (m_pointerToSources)
        delete m_pointerToSources;
}


bool CProject::serialize(QDomDocument &doc, QDomNode &node)
{
    // Create the root element for the CHR-ROM banks
    QDomElement projectElement = addElement( doc, node, "project" );

    if (!m_pointerToSources->serialize(doc, projectElement))
        return false;

    return true;
}

bool CProject::deserialize(QDomDocument &doc, QDomNode &node)
{
    return false;
}

QString CProject::caption() const
{
    return "Project";
}
