#include "cprgrombank.h"

CPRGROMBank::CPRGROMBank()
{
    editor = (PRGROMDisplayDialog *)0;
    data = new qint8[0x4000];
}


bool CPRGROMBank::serialize(QDomDocument &doc, QDomNode &node)
{
    // Create the root element for the CHR-ROM object
    QDomElement prgromElement = addElement( doc, node, "prgrom" );
    QDomCDATASection dataSect = doc.createCDATASection(QByteArray::fromRawData((const char *)data, 0x4000).toBase64());
    prgromElement.appendChild(dataSect);

    return true;
}

bool CPRGROMBank::deserialize(QDomDocument &doc, QDomNode &node)
{
    return false;
}

QString CPRGROMBank::caption() const
{
    return "Bank " + QString::number(bankID, 10);
}

void CPRGROMBank::contextMenuEvent(QContextMenuEvent *event, QTreeView *parent)
{

}

unsigned char c2a(unsigned char c)
{
    if(c<=9)
    {
        return c+0x30;
    }
    return c-0x0A+'A';
}


void CPRGROMBank::openItemEvent(QTabWidget *tabWidget)
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
        editor = new PRGROMDisplayDialog();
        tabId = tabWidget->addTab(editor, this->caption());
    }

    QString rt;
    for(int i=0; i<0x4000; i++)
    {
        char l = (data[i]>>4)&0x0F;
        char r= data[i]&0x0F;
        QChar c[2] = { c2a(l),c2a(r)};
        rt += QString(c,2);
        rt += " ";
    }

    tabWidget->setCurrentIndex(tabId);
    editor->setRomData(rt);
}


