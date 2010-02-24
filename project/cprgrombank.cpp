#include "cprgrombank.h"

CPRGROMBank::CPRGROMBank()
{
    m_indexOfPrgRomBank = -1;
    m_pointerToBankData = new quint8[0x4000];
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

quint8 *CPRGROMBank::get_pointerToBankData()
{
    return m_pointerToBankData;
}

void CPRGROMBank::set_pointerToBankData(quint8 *pointerToBankData)
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
    return "PRG Bank " + QString::number(m_indexOfPrgRomBank, 10);
}

void CPRGROMBank::contextMenuEvent(QContextMenuEvent *, QTreeView *)
{

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
        m_pointerToEditorDialog->setRomData(m_pointerToBankData);
        m_indexOfEditorTab = tabWidget->addTab(m_pointerToEditorDialog, this->caption());
    }

    tabWidget->setCurrentIndex(m_indexOfEditorTab);
}


