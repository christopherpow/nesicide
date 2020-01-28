#include "ctilestamprenderer.h"

CTileStampRenderer::CTileStampRenderer(QWidget* parent, char* data)
   : QOpenGLWidget(parent),
     initialized(false)
{
   imageData = data;
   scrollX = 0;
   scrollY = 0;
   xSize = 8;
   ySize = 8;
   gridEnabled = false;
   boxX1 = -1;
   boxY1 = -1;
   boxX2 = -1;
   boxY2 = -1;
}

CTileStampRenderer::~CTileStampRenderer()
{
   glDeleteTextures(1,(GLuint*)&textureID);
}

void CTileStampRenderer::initializeGL()
{
   initializeOpenGLFunctions();

   if ( initialized )
   {
      glDeleteTextures(1,(GLuint*)&textureID);
   }

   glGenTextures(1,(GLuint*)&textureID);
   zoom = 100;

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
   glBindTexture(GL_TEXTURE_2D, textureID);

   // We want it to be RGBA formatted
   glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
   glPixelStorei(GL_PACK_ALIGNMENT, 4);
   glPixelStorei(GL_UNPACK_ROW_LENGTH, 256);
   glPixelStorei(GL_PACK_ROW_LENGTH, 256);

   // Set our texture parameters
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

   // Load the actual texture
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

   initialized = true;
}

void CTileStampRenderer::setBGColor(QColor clr)
{
   glClearColor((float)clr.red() / 255.0f, (float)clr.green() / 255.0f, (float)clr.blue() / 255.0f, 0.5f);
}

void CTileStampRenderer::resizeGL(int width, int height)
{
   QSize actualSize;

   QOpenGLWidget::resizeGL(width,height);

   // Width cannot be 0 or the system will freak out
   if (width == 0)
   {
      width = 1;
   }

   // Initialize our viewpoint using the actual size so 1 point should = 1 pixel.
   glViewport(0, 0, width, height);
}

void CTileStampRenderer::paintGL()
{
   int idxx;
   int idxy;
   QSize actualSize;

   // Force integral scaling factors. TODO: Add to environment settings.
   int zf  = zoom / 100;
   int size = (xSize>ySize)?xSize:ySize;

   actualSize.setWidth( 256*zf );
   actualSize.setHeight( 256*zf );

   // Select and reset the Projection matrix.
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   // Set orthogonal mode (since we are doing 2D rendering).
   glOrtho( 0.0f, (float)size, (float)size, 0.0f, -1.0f, 1.0f);

   // Select and reset the ModelView matrix.
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // Translate/scale.
   glTranslatef(-(scrollX*zf),-(scrollY*zf),0.0f);
   glScalef( actualSize.width() / 256, actualSize.height() / 256, 1 );

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glBindTexture (GL_TEXTURE_2D, textureID);
   glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
   glBegin(GL_QUADS);
   glTexCoord2f (0.0, 0.0);
   glVertex3f(000.0f, 000.0f, 0.0f);
   glTexCoord2f ((float)xSize/256.0, 0.0);
   glVertex3f((float)xSize, 000.0f, 0.0f);
   glTexCoord2f ((float)xSize/256.0, (float)ySize/256.0);
   glVertex3f((float)xSize, (float)ySize, 0.0f);
   glTexCoord2f (0.0, (float)ySize/256.0);
   glVertex3f(000.0f, (float)ySize, 0.0f);
   glEnd();
   if ( gridEnabled )
   {
      glDisable(GL_TEXTURE_2D);
      glColor3f(1.0,1.0,0.0);
      for (idxy = 0; idxy <= ySize; idxy++)
      {
         for (idxx = 0; idxx <= xSize; idxx++)
         {
            if ( (!(idxx%16)) && (!(idxy%16)) )
            {
               glPointSize(3.0);
               glBegin(GL_POINTS);
               glVertex3f(idxx, idxy, 0.0f);
               glEnd();
            }
            else if ( (!(idxx%8)) && (!(idxy%8)) )
            {
               glPointSize(2.0);
               glBegin(GL_POINTS);
               glVertex3f(idxx, idxy, 0.0f);
               glEnd();
            }
         }
      }
      glEnable(GL_TEXTURE_2D);
   }
   if ( (boxX1 >= 0) && (boxY1 >= 0) )
   {
      glDisable(GL_TEXTURE_2D);
      glColor3f(1.0,1.0,0.0);
      glBegin(GL_LINE_STRIP);
      glVertex3f(boxX1,boxY1,0.0f);
      glVertex3f(boxX2,boxY1,0.0f);
      glVertex3f(boxX2,boxY2,0.0f);
      glVertex3f(boxX1,boxY2,0.0f);
      glVertex3f(boxX1,boxY1,0.0f);
      glEnd();
      glEnable(GL_TEXTURE_2D);
   }
}

void CTileStampRenderer::changeZoom(int newZoom)
{
   makeCurrent();
   zoom = newZoom;
   resizeGL(width(), height());
   update();
}

void CTileStampRenderer::setSize(int newX,int newY)
{
   makeCurrent();
   xSize = newX;
   ySize = newY;
   resizeGL(width(),height());
   update();
}

bool CTileStampRenderer::pointToPixel(int ptx,int pty,int* pixx,int* pixy)
{
   int size = (xSize>ySize)?xSize:ySize;
   float xpixSize = (float)width()/(float)size;
   float ypixSize = (float)height()/(float)size;
   int zf = zoom/100;

   // Clip edges.
   if ( ptx < 0 )
   {
      ptx = 0;
   }
   if ( ptx > width()-1 )
   {
      ptx = width()-1;
   }
   if ( pty < 0 )
   {
      pty = 0;
   }
   if ( pty > height()-1 )
   {
      pty = height()-1;
   }

   ptx /= xpixSize;
   pty /= ypixSize;
   ptx /= zf;
   pty /= zf;
   (*pixx) = ptx;
   (*pixy) = pty;
   (*pixx) += scrollX;
   (*pixy) += scrollY;
   if ( ((*pixx) < xSize) && ((*pixy) < ySize) )
   {
      return true;
   }
   else
   {
      return false;
   }
}
