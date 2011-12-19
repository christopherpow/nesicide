#ifndef CTILESTAMPRENDERER_H
#define CTILESTAMPRENDERER_H

#include <QWidget>
#include <QGLWidget>
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
   void setSize(int newX,int newY);
   bool pointToPixel(int ptx,int pty,int* pixx,int* pixy);
   void setBGColor(QColor clr);
   void setScrollX(int newScrollX) { scrollX = newScrollX; }
   void setScrollY(int newScrollY) { scrollY = newScrollY; }
   void setGrid(bool enabled) { gridEnabled = enabled; }
   void setBox(int x1=-1,int y1=-1,int x2=-1,int y2=-1) { boxX1 = x1; boxY1 = y1; boxX2 = x2; boxY2 = y2; }
   void getBox(int* x1,int* y1,int* x2,int* y2) { (*x1) = boxX1; (*y1) = boxY1; (*x2) = boxX2; (*y2) = boxY2; }
protected:
   int xSize;
   int ySize;
   int zoom;
   int scrollX;
   int scrollY;
   char* imageData;
   GLuint textureID;

   // Properties.
   bool gridEnabled;
   int boxX1;
   int boxY1;
   int boxX2;
   int boxY2;
};

#endif // CTILESTAMPRENDERER_H
