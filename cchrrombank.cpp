#include "cchrrombank.h"

CCHRROMBank::CCHRROMBank()
{
    editor = (CHRROMDisplayDialog *)NULL;
}

bool CCHRROMBank::serialize(QDomDocument &doc, QDomNode &node)
{
    return false;
}

QString CCHRROMBank::caption() const
{
    return "Bank " + QString::number(bankID, 10);
}

void CCHRROMBank::contextMenuEvent(QContextMenuEvent *event, QTreeView *parent)
{

}

void CCHRROMBank::openItemEvent(QTabWidget *tabWidget)
{
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
        editor = new CHRROMDisplayDialog();
        tabId = tabWidget->addTab(editor, this->caption());
    }


    tabWidget->setCurrentIndex(tabId);

}

