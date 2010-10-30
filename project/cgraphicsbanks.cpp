#include "cgraphicsbanks.h"

CGraphicsBanks::CGraphicsBanks()
{
    m_paGraphicsBank = new QList<CGraphicsBank *>();
}

CGraphicsBanks::~CGraphicsBanks()
{
    if (m_paGraphicsBank)
    {
        for (int idx = 0; idx < m_paGraphicsBank->count(); idx++)
        {
            CGraphicsBank *bank = m_paGraphicsBank->at(idx);
            if (bank)
                delete bank;
        }
        delete m_paGraphicsBank;
    }

}

bool CGraphicsBanks::serialize(QDomDocument &doc, QDomNode &node)
{
    QDomElement graphicsBanksElement = addElement( doc, node, "graphicsbanks" );
    return true;
}

bool CGraphicsBanks::deserialize(QDomDocument &, QDomNode &)
{
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
                m_paGraphicsBank->append(pBank);
                this->appendChild(pBank);
                ((CProjectTreeViewModel *)parent->model())->layoutChangedEvent();
            }
        }
    }
}

QList<CGraphicsBank *> *CGraphicsBanks::getGraphicsBankArray()
{
    return m_paGraphicsBank;
}

void CGraphicsBanks::setGraphicsBankArray(QList<CGraphicsBank *> *newGraphicsBankArray)
{
    m_paGraphicsBank = newGraphicsBankArray;
}
