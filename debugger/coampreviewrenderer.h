#ifndef COAMPREVIEWRENDERER_H
#define COAMPREVIEWRENDERER_H

#include <QWidget>
#include <QGLWidget>
#include "cgltexturemanager.h"
#if defined ( __APPLE__ )
#include <OpenGL/glext.h>
#else
#include <GL/glext.h>
#endif

class COAMPreviewRenderer : public QGLWidget
{
public:
   COAMPreviewRenderer(QWidget* parent, char* data);
   virtual ~COAMPreviewRenderer();
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
};

#endif // COAMPREVIEWRENDERER_H
