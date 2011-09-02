#include "CTileStamprenderer.h"

CTileStampRenderer::CTileStampRenderer(QWidget* parent, char* data)
   : QGLWidget(parent)
{
   imageData = data;
   scrollX = 0;
   scrollY = 0;
   xSize = 8;
   ySize = 8;
}

CTileStampRenderer::~CTileStampRenderer()
{
   CGLTextureManager::freeTextureID(textureID);
}

void CTileStampRenderer::initializeGL()
{
   textureID = CGLTextureManager::getNewTextureID();
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
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_BGRA, GL_UNSIGNED_BYTE, imageData);
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

   actualSize.setWidth( 256*zf );
   actualSize.setHeight( 256*zf );

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
   glOrtho( 0.0, (float)size, (float)size, 0.0f, -1.0f, 1.0f);

   // Select and reset the ModelView matrix.
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // Scale.
   glScalef( actualSize.width() / 256, actualSize.height() / 256, 1 );

   // Slightly offset the view to ensure proper pixel alignment
//    glTranslatef(0.5,0.5,0);
}


void CTileStampRenderer::paintGL()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glBindTexture (GL_TEXTURE_2D, textureID);
   glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_BGRA, GL_UNSIGNED_BYTE, imageData);
   glBegin(GL_QUADS);
   glTexCoord2f (0.0, 0.0);
   glVertex3f(000.0f - scrollX, 000.0f - scrollY, 0.0f);
   glTexCoord2f ((float)xSize/256.0, 0.0);
   glVertex3f((float)xSize - scrollX, 000.0f - scrollY, 0.0f);
   glTexCoord2f ((float)xSize/256.0, (float)ySize/256.0);
   glVertex3f((float)xSize - scrollX, (float)ySize - scrollY, 0.0f);
   glTexCoord2f (0.0, (float)ySize/256.0);
   glVertex3f(000.0f - scrollX, (float)ySize - scrollY, 0.0f);
   glEnd();
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
   xSize = newX;
   ySize = newY;
   resizeGL(width(),height());
   repaint();
}
void CTileStampRenderer::pointToPixel(int ptx,int pty,int* pixx,int* pixy)
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
}
