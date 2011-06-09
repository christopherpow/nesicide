#include "cgltexturemanager.h"

QList<int> CGLTextureManager::m_arrayOfTextureIndexes;
QMutex     CGLTextureManager::m_mutex;
unsigned int CGLTextureManager::m_textures[] = { 0, };
bool         CGLTextureManager::m_inited = false;

CGLTextureManager::CGLTextureManager()
{
   int idx;
   glGenTextures ( 10, (GLuint*)m_textures );

   for ( idx = 0; idx < 10; idx++ )
   {
      m_arrayOfTextureIndexes.append ( m_textures[idx] );
   }
}

int CGLTextureManager::getNewTextureID()
{
   m_mutex.lock ();

   if ( !m_inited )
   {
      CGLTextureManager ();
      m_inited = true;
   }

   int textureID = -1;

   if ( m_arrayOfTextureIndexes.size() )
   {
      textureID = m_arrayOfTextureIndexes.at(0);
      m_arrayOfTextureIndexes.removeAt(0);
   }
   else
   {
      // assert
   }

   m_mutex.unlock ();
   return textureID;
}

void CGLTextureManager::freeTextureID(int textureID)
{
   m_mutex.lock ();
   m_arrayOfTextureIndexes.append ( textureID );
   m_mutex.unlock ();
}

