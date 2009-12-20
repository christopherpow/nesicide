#include "cgltexturemanager.h"

QList<int> CGLTextureManager::m_arrayOfTextureIndexes;
QMutex     CGLTextureManager::m_mutex;
CGLTextureManager::CGLTextureManager()
{
}

int CGLTextureManager::getNewTextureID()
{
   m_mutex.lock ();
   int textureID = 0;
   while (m_arrayOfTextureIndexes.contains(textureID))
   {
      textureID++;
   }
   m_arrayOfTextureIndexes.append(textureID);
   m_mutex.unlock ();
   return textureID;
}

void CGLTextureManager::freeTextureID(int textureID)
{
   m_mutex.lock ();
   m_arrayOfTextureIndexes.removeAll(textureID);
   m_mutex.unlock ();
}

