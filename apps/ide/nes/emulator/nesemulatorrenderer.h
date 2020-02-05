#ifndef CNESEMULATORRENDERER_H
#define CNESEMULATORRENDERER_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class CNESEmulatorRenderer : public QOpenGLWidget
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
   bool initialized;
protected:
   void DeleteFunctions() {delete(m_pFunctions); m_pFunctions = nullptr;}

   QOpenGLFunctions * m_pFunctions = nullptr;
};

#endif // CNESEMULATORRENDERER_H
