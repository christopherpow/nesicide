#include "cchrrombank.h"

CCHRROMBank::CCHRROMBank()
{
    editor = (CHRROMDisplayDialog *)NULL;
    data = new qint8[0x2000];
}

CCHRROMBank::~ CCHRROMBank()
{
    delete data;
}

bool CCHRROMBank::serialize(QDomDocument &doc, QDomNode &node)
{
    // Create the root element for the CHR-ROM object
    QDomElement chrromElement = addElement( doc, node, "chrrom" );
    QDomCDATASection dataSect = doc.createCDATASection(QByteArray::fromRawData((const char *)data, 0x2000).toBase64());
    chrromElement.appendChild(dataSect);
    return true;
}

bool CCHRROMBank::deserialize(QDomDocument &, QDomNode &)
{
    return false;
}

QString CCHRROMBank::caption() const
{
    return "CHR Bank " + QString::number(bankID, 10);
}

void CCHRROMBank::contextMenuEvent(QContextMenuEvent *, QTreeView *)
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
        editor = new CHRROMDisplayDialog(0, false, data);
        tabId = tabWidget->addTab(editor, this->caption());
    }


    tabWidget->setCurrentIndex(tabId);
    editor->updateScrollbars();

}

