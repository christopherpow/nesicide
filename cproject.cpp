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

    if (m_pointerToSources)
    {
        if (!m_pointerToSources->serialize(doc, projectElement))
            return false;
    } else
        return false;

    return true;
}

bool CProject::deserialize(QDomDocument &doc, QDomNode &node)
{
    if (m_pointerToSources)
        delete m_pointerToSources;

    m_pointerToSources = new CSources();
    m_pointerToSources->InitTreeItem(this);

    QDomNode childNode = node.firstChild();
    do
    {
        if (childNode.nodeName() == "sources") {
            if (!m_pointerToSources->deserialize(doc, childNode))
                return false;
        } else
            return false;
    } while (!(childNode = childNode.nextSibling()).isNull());

    return true;
}

QString CProject::caption() const
{
    return "Project";
}
