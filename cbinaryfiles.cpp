#include "cbinaryfiles.h"

CBinaryFiles::CBinaryFiles()
{
}

CBinaryFiles::~CBinaryFiles()
{
}

bool CBinaryFiles::serialize(QDomDocument &doc, QDomNode &node)
{
    return false;
}

bool CBinaryFiles::deserialize(QDomDocument &doc, QDomNode &node)
{
    return true;
}

QString CBinaryFiles::caption() const
{
    return QString("Binary Files");
}

void CBinaryFiles::contextMenuEvent(QContextMenuEvent*, QTreeView*)
{

}
