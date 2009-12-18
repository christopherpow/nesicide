#include "cgltexturemanager.h"

QList<int> CGLTextureManager::m_arrayOfTextureIndexes;

CGLTextureManager::CGLTextureManager()
{
}

int CGLTextureManager::getNewTextureID()
{
    int textureID = 0;
    while (m_arrayOfTextureIndexes.contains(textureID))
        textureID++;
    m_arrayOfTextureIndexes.append(textureID);
    return textureID;
}

void CGLTextureManager::freeTextureID(int textureID)
{
    m_arrayOfTextureIndexes.removeAll(textureID);
}

