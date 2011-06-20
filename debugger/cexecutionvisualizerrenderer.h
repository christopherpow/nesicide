#ifndef CEXECUTIONVISUALIZERRENDERER_H
#define CEXECUTIONVISUALIZERRENDERER_H

#include <QWidget>
#include <QGLWidget>
#include "cgltexturemanager.h"
#if defined ( __APPLE__ )
#include <OpenGL/glext.h>
#else
#include <GL/glext.h>
#endif

class CExecutionVisualizerRenderer : public QGLWidget
{
public:
   CExecutionVisualizerRenderer(QWidget* parent, char* data);
   virtual ~CExecutionVisualizerRenderer();
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

#endif // CEXECUTIONVISUALIZERRENDERER_H
