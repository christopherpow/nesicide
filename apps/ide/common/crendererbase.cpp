#include "crendererbase.h"

CRendererBase::CRendererBase(int sizeX,int sizeY,int textureSizeXY,int maxZoom,int8_t* imageData,QWidget *parent) :
      QOpenGLWidget(parent),
      _sizeX(sizeX),
      _trueSizeX(textureSizeXY),
      _sizeY(sizeY),
      _scrollX(0),
      _scrollY(0),
      _zoomFactor(100),
      _maxZoom(maxZoom),
      _initialized(false)
{
   setBackgroundRole(QPalette::Dark);
   setCursor(QCursor(Qt::CrossCursor));

   _imageData = imageData;
}

CRendererBase::CRendererBase(int sizeX,int sizeY,int maxZoom,int8_t* imageData,QWidget *parent) :
    QOpenGLWidget(parent),
    _sizeX(sizeX),
    _trueSizeX(sizeX),
    _sizeY(sizeY),
    _scrollX(0),
    _scrollY(0),
    _zoomFactor(100),
    _maxZoom(maxZoom),
    _initialized(false)
{
   setBackgroundRole(QPalette::Dark);
   setCursor(QCursor(Qt::CrossCursor));

   _imageData = imageData;
}

CRendererBase::~CRendererBase()
{
   if ( _initialized )
   {
      glDeleteTextures(1,(GLuint*)&_textureID);
   }
}

void CRendererBase::initializeGL()
{
   initializeOpenGLFunctions();

   if ( _initialized )
   {
      glDeleteTextures(1,(GLuint*)&_textureID);
   }

   glGenTextures(1,(GLuint*)&_textureID);

   // Enable flat shading
   glShadeModel(GL_FLAT);

   // Black background color
   glClearColor(0.0f, 0.0f, 0.0f, 0.5f);

   // Depth buffer setup
   glClearDepth(1.0f);

   // Use the fastest rendering possible
   glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
   glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);
   glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
   glHint(GL_TEXTURE_COMPRESSION_HINT,GL_FASTEST);
   glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_FASTEST);

   // Disable stuff
   glDisable(GL_BLEND);
   glDisable(GL_LIGHTING);
   glDisable(GL_DEPTH_TEST);
   glDisable(GL_DITHER);

   // Enable textures
   glEnable(GL_TEXTURE_2D);

   // Create the texture we will be rendering onto
   glBindTexture(GL_TEXTURE_2D, _textureID);

   // We want it to be RGBA formatted
   glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
   glPixelStorei(GL_PACK_ALIGNMENT, 4);
   glPixelStorei(GL_UNPACK_ROW_LENGTH, _trueSizeX);
   glPixelStorei(GL_PACK_ROW_LENGTH, _trueSizeX);

   // Set our texture parameters
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

   // Load the actual texture
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _sizeX, _sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, _imageData);

   _initialized = true;
}

void CRendererBase::reloadData(int8_t* imageData)
{
   _imageData = imageData;

   update();
}

void CRendererBase::setBGColor(QColor clr)
{
   glClearColor((float)clr.red() / 255.0f, (float)clr.green() / 255.0f, (float)clr.blue() / 255.0f, 0.5f);
}

void CRendererBase::resizeGL(int width, int height)
{
   QSize actualSize;

   QOpenGLWidget::resizeGL(width,height);

   // Force integral scaling factors. TODO: Add to environment settings.
   int zf  = _zoomFactor / 100;

   actualSize.setWidth( _sizeX*zf );
   actualSize.setHeight( _sizeY*zf );

   // Width cannot be 0 or the system will freak out
   if (width == 0)
   {
      width = 1;
   }

   // Initialize our viewpoint using the actual size so 1 point should = 1 pixel.
   glViewport(0, 0, width, height);

   // We are using a projection matrix.
   glMatrixMode(GL_PROJECTION);

   // Load the default settings for the matrix.
   glLoadIdentity();

   // Set orthogonal mode (since we are doing 2D rendering).
   glOrtho( 0, 1, 1, 0.0f, -1.0f, 1.0f);

   // Select and reset the ModelView matrix.
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // Translate for letter-/pillarboxing
   glScalef( actualSize.width() / float( width )/_sizeX, actualSize.height() / float( height )/_sizeY, 1 );

   // Slightly offset the view to ensure proper pixel alignment
//    glTranslatef(0.5,0.5,0);
}


void CRendererBase::paintGL()
{
   glBindTexture (GL_TEXTURE_2D, _textureID);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _sizeX, _sizeY, GL_RGBA, GL_UNSIGNED_BYTE, _imageData);
   glBegin(GL_QUADS);
   glTexCoord2f (0.0, 0.0);
   glVertex3f(000.0f - _scrollX, 000.0f - _scrollY, 0.0f);
   glTexCoord2f (1.0, 0.0);
   glVertex3f(_sizeX - _scrollX, 000.0f - _scrollY, 0.0f);
   glTexCoord2f (1.0, 1.0);
   glVertex3f(_sizeX - _scrollX, _sizeY - _scrollY, 0.0f);
   glTexCoord2f (0.0, 1.0);
   glVertex3f(000.0f - _scrollX, _sizeY - _scrollY, 0.0f);
   glEnd();
}

void CRendererBase::changeZoom(int newZoom)
{
   makeCurrent();
   _zoomFactor = newZoom;
   resizeGL(width(),height());
   doneCurrent();
   update();
}

bool CRendererBase::pointToPixel(int ptx,int pty,int* pixx,int* pixy)
{
   int zf = _zoomFactor/100;

   // Get to widget-coords.
   QPoint myPoint = mapFromGlobal(QPoint(ptx,pty));
   ptx = myPoint.x();
   pty = myPoint.y();

   ptx /= zf;
   pty /= zf;
   (*pixx) = ptx;
   (*pixy) = pty;
   (*pixx) += _scrollX;
   (*pixy) += _scrollY;
   if ( ((*pixx) < _sizeX) && ((*pixy) < _sizeY) )
   {
      return true;
   }
   else
   {
      return false;
   }
}
