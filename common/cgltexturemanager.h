#ifndef CGLTEXTUREMANAGER_H
#define CGLTEXTUREMANAGER_H

#include <QList>
#include <QMutex>

class CGLTextureManager
{
public:
    CGLTextureManager();
    static int getNewTextureID();
    static void freeTextureID(int textureID);

protected:
    static QList<int> m_arrayOfTextureIndexes;
    static QMutex m_mutex;
};

#endif // CGLTEXTUREMANAGER_H
