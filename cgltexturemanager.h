#ifndef CGLTEXTUREMANAGER_H
#define CGLTEXTUREMANAGER_H

#include <QList>

class CGLTextureManager
{
public:
    CGLTextureManager();
    int getNewTextureID();
    void freeTextureID(int textureID);

protected:
    static QList<int> m_arrayOfTextureIndexes;
};

#endif // CGLTEXTUREMANAGER_H
