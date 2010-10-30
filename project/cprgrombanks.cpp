#include "cprgrombanks.h"

CPRGROMBanks::CPRGROMBanks()
{
    m_pointerToArrayOfBanks = new QList<CPRGROMBank *>();
}

CPRGROMBanks::~CPRGROMBanks()
{
    if (m_pointerToArrayOfBanks)
    {
        for (int indexOfBank=0; indexOfBank<m_pointerToArrayOfBanks->count(); indexOfBank++)
            if (m_pointerToArrayOfBanks->at(indexOfBank))
                delete m_pointerToArrayOfBanks->at(indexOfBank);
        delete m_pointerToArrayOfBanks;
    }
}

bool CPRGROMBanks::serialize(QDomDocument &doc, QDomNode &node)
{
    // Create the root element for the CHR-ROM banks
    QDomElement prgromElement = addElement( doc, node, "prgrombanks" );

    for (int indexOfBank=0; indexOfBank<m_pointerToArrayOfBanks->count(); indexOfBank++)
        if (!m_pointerToArrayOfBanks->at(indexOfBank)->serialize(doc, prgromElement))
            return false;

    return true;
}

bool CPRGROMBanks::deserialize(QDomDocument &, QDomNode &)
{
    return true;
}

QString CPRGROMBanks::caption() const
{
    return "PRG-ROM Banks";
}

QList<CPRGROMBank *> *CPRGROMBanks::get_pointerToArrayOfBanks()
{
    return m_pointerToArrayOfBanks;
}

void CPRGROMBanks::set_pointerToArrayOfBanks(QList<CPRGROMBank *> *pointerToArrayOfBanks)
{
    m_pointerToArrayOfBanks = pointerToArrayOfBanks;
}
