#include "CTileStamprenderer.h"

CTileStampRenderer::CTileStampRenderer(QWidget* parent, char* data)
   : QGLWidget(parent)
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

   // Disable Blending
   glDisable(GL_BLEND);

   // Enable textures
   glEnable(GL_TEXTURE_2D);

   resizeGL(width(),height());

   // Create the texture we will be rendering onto
   glBindTexture(GL_TEXTURE_2D, textureID);

   // We want it to be RGBRGB(etc) formatted
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   // Set our texture parameters
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

   // Load the actual texture
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_BGRA, GL_UNSIGNED_BYTE, imageData);
}

void CTileStampRenderer::setBGColor(QColor clr)
{
   glClearColor((float)clr.red() / 255.0f, (float)clr.green() / 255.0f, (float)clr.blue() / 255.0f, 0.5f);
}

void CTileStampRenderer::resizeGL(int width, int height)
{
   QSize actualSize;

   // Force integral scaling factors. TODO: Add to environment settings.
   int zf  = zoom / 100;
   int size = (xSize>ySize)?xSize:ySize;

   actualSize.setWidth( 128*zf );
   actualSize.setHeight( 128*zf );

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

#if 0
   implementation from ReaperSMS for square tiles.
   For projection:
   float waspect = (float)width / (float)height
   float saspect = (float)xSize / (float)ySize
   float shift;

   if (waspect > saspect)
   {
         shift = (waspect - saspect) * 0.5f;
         glOrtho(-shift*xSize, (1.0 + shift) * xSize, ySize, 0.0, -1.0, 1.0);
   }
   else
   {
         shift = (saspect - waspect) * 0.5f;
         glOrtho(0.0, xSize, (1.0+shift)*ySize, -shift*ySize, -1.0, 1.0);
   }

   For rendering, ignore scroll until you know how it's supposed to work on the QT side:
   Main box:
   glVertex3f(0.0f, 0.0f, 0.0f);
   glVertex3f(xSize, 0.0f, 0.0f);
   glVertex3f(xSize, ySize, 0.0f);
   glVertex3f(0.0f, ySize, 0.0f);

   Grid:
   Either set glPointSize() to something reasonable, and render with

   glBegin(GL_POINTS);
   glColor3f(1.0,1.0,0.0);
   for (i = 0; i < xSize; i++)
      for (j = 0; j < ySize; j++)
         glVertex3f(i, j, 0.0f);
   glEnd();

   Or use the current loop, with idxx/idxy as your coordinates, +/- a percentage of a texel size.
#endif
//   <ReaperSMS> glOrtho(0.0, (1.0 + 2 * shift) * xSize, ySize, 0.0, -1.0, 1.0);
//   <ReaperSMS> for the other one, glOrtho(0.0, xSize, (1.0 + 2 * shift) * ySize, 0.0, -1.0, 1.0)

   // Set orthogonal mode (since we are doing 2D rendering).
   glOrtho( 0.0f, (float)size, (float)size, 0.0f, -1.0f, 1.0f);

   // Select and reset the ModelView matrix.
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // Scale.
   glScalef( actualSize.width() / 128, actualSize.height() / 128, 1 );

   // Slightly offset the view to ensure proper pixel alignment
//    glTranslatef(0.5,0.5,0);
}

void CTileStampRenderer::paintGL()
{
   int idxx;
   int idxy;
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glBindTexture (GL_TEXTURE_2D, textureID);
   glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 128, 128, GL_BGRA, GL_UNSIGNED_BYTE, imageData);
   glBegin(GL_QUADS);
   glTexCoord2f (0.0, 0.0);
   glVertex3f(000.0f - scrollX, 000.0f - scrollY, 0.0f);
   glTexCoord2f ((float)xSize/128.0, 0.0);
   glVertex3f((float)xSize - scrollX, 000.0f - scrollY, 0.0f);
   glTexCoord2f ((float)xSize/128.0, (float)ySize/128.0);
   glVertex3f((float)xSize - scrollX, (float)ySize - scrollY, 0.0f);
   glTexCoord2f (0.0, (float)ySize/128.0);
   glVertex3f(000.0f - scrollX, (float)ySize - scrollY, 0.0f);
   glEnd();
   if ( gridEnabled )
   {
      glDisable(GL_TEXTURE_2D);
      glColor3f(1.0,1.0,0.0);
      for (idxy = -scrollY; idxy <= ySize-scrollY; idxy++)
      {
         for (idxx = -scrollX; idxx <= xSize-scrollX; idxx++)
         {
            if ( (!(idxx%16)) && (!(idxy%16)) )
            {
               glPointSize(3.0);
            }
            else if ( (!(idxx%8)) && (!(idxy%8)) )
            {
               glPointSize(2.0);
            }
            else
            {
               glPointSize(1.0);
            }
            glBegin(GL_POINTS);
            glVertex3f(idxx, idxy, 0.0f);
            glEnd();
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
   repaint();
}

void CTileStampRenderer::setSize(int newX,int newY)
{
   makeCurrent();
   xSize = newX;
   ySize = newY;
   resizeGL(width(),height());
   repaint();
}

bool CTileStampRenderer::pointToPixel(int ptx,int pty,int* pixx,int* pixy)
{
   int size = (xSize>ySize)?xSize:ySize;
   float xpixSize = (float)width()/(float)size;
   float ypixSize = (float)height()/(float)size;
   int zf = zoom/100;
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
