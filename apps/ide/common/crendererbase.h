#ifndef CRENDERERBASE_H
#define CRENDERERBASE_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QGLWidget>
#if defined ( __APPLE__ )
#include <OpenGL/glext.h>
#else
#include <GL/glext.h>
#endif

class CRendererBase : public QOpenGLWidget
{
   Q_OBJECT
public:
   CRendererBase(int sizeX,int sizeY,int textureSizeXY,int maxZoom,int8_t* imageData,QWidget *parent = 0);
   CRendererBase(int sizeX,int sizeY,int maxZoom,int8_t* imageData,QWidget *parent = 0);
   virtual ~CRendererBase();

   void initializeGL();
   void resizeGL(int width, int height);
   void paintGL();
   void changeZoom(int newZoom);
   void reloadData(int8_t* imageData);
   void setBGColor(QColor clr);
   void setScrollX(int scrollX) { _scrollX = scrollX; }
   void setScrollY(int scrollY) { _scrollY = scrollY; }
   bool pointToPixel(int ptx,int pty,int* pixx,int* pixy);

public slots:

protected:
   int _sizeX;
   int _sizeY;
   int _scrollX;
   int _scrollY;
   int _textureSizeXY;
   int8_t* _imageData;
   int _zoomFactor;
   int _maxZoom;
   GLuint _textureID;
};

#endif // CRENDERERBASE_H
