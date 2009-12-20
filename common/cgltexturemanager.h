#ifndef CGLTEXTUREMANAGER_H
#define CGLTEXTUREMANAGER_H

#include <QList>
#include <QMutex>
#include <QGLWidget>

class CGLTextureManager
{
public:
    CGLTextureManager();
    static int getNewTextureID();
    static void freeTextureID(int textureID);

protected:
    static QList<int> m_arrayOfTextureIndexes;
    static QMutex m_mutex;
    static unsigned int m_textures [ 10 ];
    static bool m_inited;
};

#endif // CGLTEXTUREMANAGER_H
