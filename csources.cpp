#include "csources.h"

CSources::CSources()
{
    m_pointerToArrayOfSourceItems = new QList<CSourceItem *>();
}

CSources::~CSources()
{
    if (m_pointerToArrayOfSourceItems)
    {
        for (int indexOfSourceItem=0; indexOfSourceItem<m_pointerToArrayOfSourceItems->count(); indexOfSourceItem++)
            if (m_pointerToArrayOfSourceItems->at(indexOfSourceItem))
                delete m_pointerToArrayOfSourceItems->at(indexOfSourceItem);
        delete m_pointerToArrayOfSourceItems;
    }
}

bool CSources::serialize(QDomDocument &doc, QDomNode &node)
{
    QDomElement sourcesElement = addElement( doc, node, "sources" );

    for (int indexOfSourceItem = 0; indexOfSourceItem < m_pointerToArrayOfSourceItems->count(); indexOfSourceItem++)
    {
        if (!m_pointerToArrayOfSourceItems->at(indexOfSourceItem))
            return false;
        if (!m_pointerToArrayOfSourceItems->at(indexOfSourceItem)->serialize(doc, sourcesElement))
            return false;
    }
    return true;
}

bool CSources::deserialize(QDomDocument &doc, QDomNode &node)
{
    return false;
}

QString CSources::caption() const
{
    return "Source Code";
}

void CSources::contextMenuEvent(QContextMenuEvent* event, QTreeView* parent)
{
    QMenu menu(parent);
    menu.addAction("&New Source...");
    menu.addSeparator();
    menu.addAction("&Import Source from File...");

    QAction *ret = menu.exec(event->globalPos());
    if (ret)
    {
        if (ret->text() == "&New Source...")
        {
            QString sourceName = QInputDialog::getText(parent, "New Source",
                                                   "What name would you like to use to identify this source file?");
            if (!sourceName.isEmpty())
            {
                CSourceItem *pointerToSourceItem = new CSourceItem();
                pointerToSourceItem->set_sourceName(sourceName);
                pointerToSourceItem->InitTreeItem(this);
                m_pointerToArrayOfSourceItems->append(pointerToSourceItem);
                this->appendChild(pointerToSourceItem);
                ((CProjectTreeViewModel *)parent->model())->layoutChangedEvent();
            }
        } else if (ret->text() == "&Import Source from File...") {


        }
    }
}

QList<CSourceItem *> *CSources::get_pointerToArrayOfSourceItems()
{
    return m_pointerToArrayOfSourceItems;
}

void CSources::set_pointerToArrayOfSourceItems(QList<CSourceItem *> *pointerToArrayOfSourceItems)
{
    m_pointerToArrayOfSourceItems = pointerToArrayOfSourceItems;
}
