#ifndef CCHRROMPREVIEWRENDERER_H
#define CCHRROMPREVIEWRENDERER_H

#include <QWidget>
#include <QGLWidget>
#include "cgltexturemanager.h"
#if defined ( __APPLE__ )
#include <OpenGL/glext.h>
#else
#include <GL/glext.h>
#endif

class CCHRROMPreviewRenderer : public QGLWidget
{
public:
   CCHRROMPreviewRenderer(QWidget* parent, char* imgData);
   virtual ~CCHRROMPreviewRenderer();
   void initializeGL();
   void resizeGL(int width, int height);
   void paintGL();
   void changeZoom(int newZoom);
   void reloadData(char* imgData);
   void setBGColor(QColor clr);
   int zoom;
   int scrollX;
   int scrollY;
   char* imageData;
   int textureID;
};

#endif // CCHRROMPREVIEWRENDERER_H
