#include "cchrrombank.h"

CCHRROMBank::CCHRROMBank()
{
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

}

