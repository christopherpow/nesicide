#include "cprgrombank.h"

CPRGROMBank::CPRGROMBank()
{
    m_indexOfPrgRomBank = -1;
    m_pointerToBankData = new qint8[0x4000];
    m_pointerToEditorDialog = (PRGROMDisplayDialog *)0;
}

CPRGROMBank::~CPRGROMBank()
{
    if (m_indexOfPrgRomBank)
        delete[] m_pointerToBankData ;

    if (m_pointerToEditorDialog)
        delete m_pointerToEditorDialog;
}

qint8 CPRGROMBank::get_indexOfPrgRomBank()
{
    return m_indexOfPrgRomBank;
}

void CPRGROMBank::set_indexOfPrgRomBank(qint8 indexOfPrgRomBank)
{
    m_indexOfPrgRomBank = indexOfPrgRomBank;
}

qint8 *CPRGROMBank::get_pointerToBankData()
{
    return m_pointerToBankData;
}

void CPRGROMBank::set_pointerToBankData(qint8 *pointerToBankData)
{
    m_pointerToBankData = pointerToBankData;
}

PRGROMDisplayDialog *CPRGROMBank::get_pointerToEditorDialog()
{
    return m_pointerToEditorDialog;
}

void CPRGROMBank::set_pointerToEditorDialog(PRGROMDisplayDialog *pointerToEditorDialog)
{
    m_pointerToEditorDialog = pointerToEditorDialog;
}

int CPRGROMBank::get_indexOfEditorTab()
{
    return m_indexOfEditorTab;
}

void CPRGROMBank::set_indexOfEditorTab(int indexOfEditorTab)
{
    m_indexOfEditorTab = indexOfEditorTab;
}

bool CPRGROMBank::serialize(QDomDocument &doc, QDomNode &node)
{
    // Create the root element for the CHR-ROM object
    QDomElement prgromElement = addElement( doc, node, "prgrom" );
    QDomCDATASection dataSect = doc.createCDATASection(QByteArray::fromRawData((const char *)m_pointerToBankData,
                                                                               0x4000).toBase64());
    prgromElement.appendChild(dataSect);

    return true;
}

bool CPRGROMBank::deserialize(QDomDocument &, QDomNode &)
{
    return false;
}

QString CPRGROMBank::caption() const
{
    return "Bank " + QString::number(m_indexOfPrgRomBank, 10);
}

void CPRGROMBank::contextMenuEvent(QContextMenuEvent *, QTreeView *)
{

}

unsigned char c2a(unsigned char c)
{
   static unsigned char hex_char [ 16 ] = { '0', '1', '2', '3', '4', '5', '6', '7',
                                            '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
   return hex_char [ c ];
}

void CPRGROMBank::openItemEvent(QTabWidget *tabWidget)
{

    if (m_pointerToEditorDialog)
    {
        if (m_pointerToEditorDialog->isVisible())
            tabWidget->setCurrentIndex(m_indexOfEditorTab);
        else
        {
            m_indexOfEditorTab = tabWidget->addTab(m_pointerToEditorDialog, this->caption());
            tabWidget->setCurrentIndex(m_indexOfEditorTab);
        }
        return;
    }
    else
    {
        m_pointerToEditorDialog = new PRGROMDisplayDialog();
        m_indexOfEditorTab = tabWidget->addTab(m_pointerToEditorDialog, this->caption());
    }

    QString rt;
    for(int i=0; i<0x4000; i++)
    {
        char l = (m_pointerToBankData[i]>>4)&0x0F;
        char r = m_pointerToBankData[i]&0x0F;
        QChar c[2] = { c2a(l), c2a(r) };
        rt += QString(c,2);
        rt += " ";
    }

    tabWidget->setCurrentIndex(m_indexOfEditorTab);
    m_pointerToEditorDialog->setRomData(rt);
}


