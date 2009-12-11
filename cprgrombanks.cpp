#include "cprgrombanks.h"

CPRGROMBanks::CPRGROMBanks()
{
    banks.clear();
}

bool CPRGROMBanks::serialize(QDomDocument &doc, QDomNode &node)
{
    return false;
}

bool CPRGROMBanks::deserialize(QDomDocument &doc, QDomNode &node)
{
    return false;
}

QString CPRGROMBanks::caption() const
{
    return "PRG-ROM Banks";
}
