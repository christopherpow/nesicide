#include "cgraphicsbanks.h"

CGraphicsBanks::CGraphicsBanks()
{
    m_pGraphicsBank = new QList<CGraphicsBank *>();
}

CGraphicsBanks::~CGraphicsBanks()
{
    if (m_pGraphicsBank)
    {
        for (int idx = 0; idx < m_pGraphicsBank->count(); idx++)
        {
            CGraphicsBank *bank = m_pGraphicsBank->at(idx);
            if (bank)
                delete bank;
        }
        delete m_pGraphicsBank;
    }

}

bool CGraphicsBanks::serialize(QDomDocument &doc, QDomNode &node)
{
    QDomElement graphicsBanksElement = addElement( doc, node, "graphicsbanks" );

    for (int sourceItemIdx = 0; sourceItemIdx < m_pGraphicsBank->count(); sourceItemIdx++)
    {
        if (!m_pGraphicsBank->at(sourceItemIdx))
            return false;
        if (!m_pGraphicsBank->at(sourceItemIdx)->serialize(doc, graphicsBanksElement))
            return false;
    }
    return true;
}

bool CGraphicsBanks::deserialize(QDomDocument &doc, QDomNode &node)
{
    if (m_pGraphicsBank)
    {
        for (int indexOfSourceItem=0; indexOfSourceItem<m_pGraphicsBank->count(); indexOfSourceItem++)
        {
            if (m_pGraphicsBank->at(indexOfSourceItem))
            {
                this->removeChild(m_pGraphicsBank->at(indexOfSourceItem));
                delete m_pGraphicsBank->at(indexOfSourceItem);
            }
        }
        delete m_pGraphicsBank;
    }

    m_pGraphicsBank = new QList<CGraphicsBank *>();

    QDomNode childNode = node.firstChild();
    if (!childNode.isNull()) do
    {
        if (childNode.nodeName() == "graphicsbank") {

            CGraphicsBank *pNewGraphicsBank = new CGraphicsBank();
            pNewGraphicsBank->InitTreeItem(this);
            m_pGraphicsBank->append(pNewGraphicsBank);
            this->appendChild(pNewGraphicsBank);
            if (!pNewGraphicsBank->deserialize(doc, childNode))
                return false;

        } else
            return false;
    } while (!(childNode = childNode.nextSibling()).isNull());

    return true;
}

void CGraphicsBanks::contextMenuEvent(QContextMenuEvent *event, QTreeView *parent)
{
    QMenu menu(parent);
    menu.addAction("&Add Bank");


    QAction *ret = menu.exec(event->globalPos());
    if (ret)
    {
        if (ret->text() == "&Add Bank")
        {
            QString bankName = QInputDialog::getText(parent, "New Bank",
                                                   "What name would you like to use to identify this bank?");
            if (!bankName.isEmpty())
            {
                CGraphicsBank *pBank = new CGraphicsBank();
                pBank->setBankName(bankName);
                pBank->InitTreeItem(this);
                m_pGraphicsBank->append(pBank);
                this->appendChild(pBank);
                ((CProjectTreeViewModel *)parent->model())->layoutChangedEvent();
            }
        }
    }
}

QList<CGraphicsBank *> *CGraphicsBanks::getGraphicsBankArray()
{
    return m_pGraphicsBank;
}

void CGraphicsBanks::setGraphicsBankArray(QList<CGraphicsBank *> *newGraphicsBankArray)
{
    m_pGraphicsBank = newGraphicsBankArray;
}
