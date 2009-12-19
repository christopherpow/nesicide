#include "cproject.h"

CProject::CProject()
{
    m_mainSource = (CSourceItem *)NULL;
    m_pointerToSources = new CSources();
    m_pointerToSources->InitTreeItem(this);
    this->appendChild(m_pointerToSources);
}

CProject::~CProject()
{
    if (m_pointerToSources)
        delete m_pointerToSources;
}

CSourceItem *CProject::getMainSource()
{
    return m_mainSource;
}

void CProject::setMainSource(CSourceItem *newSource)
{
    m_mainSource = newSource;
}

CSources *CProject::getSources()
{
    return m_pointerToSources;
}

void CProject::setSources(CSources *newSources)
{
    m_pointerToSources = newSources;
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

    if (m_mainSource)
        projectElement.setAttribute("mainSource", m_mainSource->get_sourceName());

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

    QString mainSource = node.toElement().attribute("mainSource");
    m_mainSource = (CSourceItem *)NULL;
    for (int sourceIdx = 0; sourceIdx < m_pointerToSources->childCount(); sourceIdx++)
    {
        if (mainSource == (((CSourceItem *)m_pointerToSources->child(sourceIdx))->get_sourceName()))
        {
            m_mainSource = ((CSourceItem *)m_pointerToSources->child(sourceIdx));
            break;
        }
    }

    return true;
}

QString CProject::caption() const
{
    return "Project";
}
