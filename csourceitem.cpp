#include "csourceitem.h"

CSourceItem::CSourceItem()
{
    m_indexOfTab = -1;
    m_sourceName = "";
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

void CSourceItem::openItemEvent(QTabWidget* parent)
{/*
    if (editor)
    {
        if (editor->isVisible())
            tabWidget->setCurrentIndex(tabId);
        else
        {
            tabId = tabWidget->addTab(editor, this->caption());
            tabWidget->setCurrentIndex(tabId);
        }
        return;
    }
    else
    {
        editor = new CHRROMDisplayDialog(0, data);
        tabId = tabWidget->addTab(editor, this->caption());
    }


    tabWidget->setCurrentIndex(tabId);
    editor->updateScrollbars();*/
}

QString CSourceItem::get_sourceName()
{
    return m_sourceName;
}

void CSourceItem::set_sourceName(QString sourceName)
{
    m_sourceName = sourceName;
}
