#include "cproject.h"

CProject::CProject()
{
}

CProject::~CProject()
{
}


bool CProject::serialize(QDomDocument &doc, QDomNode &node)
{

}

bool CProject::deserialize(QDomDocument &doc, QDomNode &node)
{
    return false;
}

QString CProject::caption() const
{
    return "Project";
}
