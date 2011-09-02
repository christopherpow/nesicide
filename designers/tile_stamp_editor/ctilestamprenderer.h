#ifndef CTILESTAMPRENDERER_H
#define CTILESTAMPRENDERER_H

#include <QWidget>
#include <QGLWidget>
#include "cgltexturemanager.h"
#if defined ( __APPLE__ )
#include <OpenGL/glext.h>
#else
#include <GL/glext.h>
#endif

class CTileStampRenderer : public QGLWidget
{
public:
   CTileStampRenderer(QWidget* parent, char* data);
   virtual ~CTileStampRenderer();
   void initializeGL();
   void resizeGL(int width, int height);
   void paintGL();
   void changeZoom(int newZoom);
   void setSize(int newX,int newY) { xSize = newX; ySize = newY; }
   void pointToPixel(int ptx,int pty,int* pixx,int* pixy);
   void changeImage(char* data)
   {
      imageData = data;
   }
   void setBGColor(QColor clr);
   void setScrollX(int newScrollX) { scrollX = newScrollX; }
   void setScrollY(int newScrollY) { scrollY = newScrollY; }

protected:
   int xSize;
   int ySize;
   int zoom;
   int scrollX;
   int scrollY;
   char* imageData;
   int textureID;
};

#endif // CTILESTAMPRENDERER_H
