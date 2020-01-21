#ifndef CNESEMULATORRENDERER_H
#define CNESEMULATORRENDERER_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class CNESEmulatorRenderer : public QOpenGLWidget, protected QOpenGLFunctions
{
   Q_OBJECT
public:
   CNESEmulatorRenderer(QWidget* parent, char* imgData);
   virtual ~CNESEmulatorRenderer();
   void initializeGL();
   void resizeGL(int width, int height);
   void paintGL();
   void changeZoom(int newZoom);
   void setBGColor(QColor clr);
   void setLinearInterpolation(bool enabled) { linearInterpolation = enabled; }
   void set43Aspect(bool enabled) { aspect43 = enabled; }
   int zoom;
   int scrollX;
   int scrollY;
   char* imageData;
   GLuint textureID;
   bool linearInterpolation;
   bool aspect43;
};

#endif // CNESEMULATORRENDERER_H
