#include "csourceitem.h"

CSourceItem::CSourceItem()
{
    m_indexOfTab = -1;
    m_sourceName = "";
    m_codeEditorForm = (CodeEditorForm *)NULL;
}

CSourceItem::~CSourceItem()
{
}

QString CSourceItem::get_sourceCode()
{
    if (m_codeEditorForm)
        m_sourceCode = m_codeEditorForm->get_sourceCode();

    return m_sourceCode;
}

void CSourceItem::set_sourceCode(QString sourceCode)
{
    m_sourceCode = sourceCode;

    if (m_codeEditorForm)
        m_codeEditorForm->set_sourceCode(sourceCode);
}

int CSourceItem::getTabIndex()
{
    return m_indexOfTab;
}

bool CSourceItem::serialize(QDomDocument &doc, QDomNode &node)
{
    QDomElement sourcesElement = addElement( doc, node, "sourceitem" );
    sourcesElement.setAttribute("sourcename", m_sourceName);
    QDomCDATASection dataSect = doc.createCDATASection(get_sourceCode());
    sourcesElement.appendChild(dataSect);
    return true;
}

bool CSourceItem::deserialize(QDomDocument &doc, QDomNode &node)
{
    QDomElement element = node.toElement();

    if (element.isNull())
        return false;

    if (!element.hasAttribute("sourcename"))
        return false;

    m_sourceName = element.attribute("sourcename");
    QDomCDATASection cdata = element.firstChild().toCDATASection();
    if (cdata.isNull())
        return false;

    m_sourceCode = cdata.data();

    return true;
}

QString CSourceItem::caption() const
{
    return m_sourceName;
}
void CSourceItem::contextMenuEvent(QContextMenuEvent* event, QTreeView* parent)
{

}

void CSourceItem::openItemEvent(QTabWidget* tabWidget)
{
    if (m_codeEditorForm)
    {
        if (m_codeEditorForm->isVisible())
            tabWidget->setCurrentIndex(m_indexOfTab);
        else
        {
            m_indexOfTab = tabWidget->addTab(m_codeEditorForm, this->caption());
            tabWidget->setCurrentIndex(m_indexOfTab);
        }
        return;
    }
    else
    {
        m_codeEditorForm = new CodeEditorForm();
        m_codeEditorForm->set_sourceCode(m_sourceCode);
        m_indexOfTab = tabWidget->addTab(m_codeEditorForm, this->caption());
    }


    tabWidget->setCurrentIndex(m_indexOfTab);
}

QString CSourceItem::get_sourceName()
{
    return m_sourceName;
}

void CSourceItem::set_sourceName(QString sourceName)
{
    m_sourceName = sourceName;
}
