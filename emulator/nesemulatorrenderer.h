#ifndef CNESEMULATORRENDERER_H
#define CNESEMULATORRENDERER_H

#include <QWidget>
#include <QGLWidget>
#include "cgltexturemanager.h"
#if defined ( __APPLE__ )
#include <OpenGL/glext.h>
#else
#include <GL/glext.h>
#endif

class CNESEmulatorRenderer : public QGLWidget
{
public:
   CNESEmulatorRenderer(QWidget* parent, char* imgData);
   virtual ~CNESEmulatorRenderer();
   void initializeGL();
   void resizeGL(int width, int height);
   void paintGL();
   void changeZoom(int newZoom);
   void setBGColor(QColor clr);
   int zoom;
   int scrollX;
   int scrollY;
   char* imageData;
   int textureID;
   QRect renderRect;
};

#endif // CNESEMULATORRENDERER_H
