#ifndef CCODEDATALOGGERRENDERER_H
#define CCODEDATALOGGERRENDERER_H

#include <QWidget>
#include <QGLWidget>
#include "cgltexturemanager.h"
#if defined ( __APPLE__ )
#include <OpenGL/glext.h>
#else
#include <GL/glext.h>
#endif

class CCodeDataLoggerRenderer : public QGLWidget
{
public:
   CCodeDataLoggerRenderer(QWidget* parent, char* data);
   virtual ~CCodeDataLoggerRenderer();
   void initializeGL();
   void resizeGL(int width, int height);
   void paintGL();
   void changeZoom(int newZoom);
   void changeImage(char* data)
   {
      imageData = data;
   }
   void setBGColor(QColor clr);
   int zoom;
   int scrollX;
   int scrollY;
   char* imageData;
   int textureID;
};

#endif // CCODEDATALOGGERRENDERER_H
