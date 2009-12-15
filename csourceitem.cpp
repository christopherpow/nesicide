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

bool CSourceItem::serialize(QDomDocument &doc, QDomNode &node)
{
    QDomElement sourcesElement = addElement( doc, node, "sourceitem" );
    return true;
}

bool CSourceItem::deserialize(QDomDocument &doc, QDomNode &node)
{
    return false;
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
