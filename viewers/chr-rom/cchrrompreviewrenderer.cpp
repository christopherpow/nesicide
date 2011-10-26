#include "cchrrompreviewrenderer.h"

CCHRROMPreviewRenderer::CCHRROMPreviewRenderer(QWidget* parent, char* imgData)
   : QGLWidget(parent)
{
   imageData = imgData;
   scrollX = 0;
   scrollY = 0;
}

CCHRROMPreviewRenderer::~CCHRROMPreviewRenderer()
{
   glDeleteTextures(1,(GLuint*)&textureID);
}

void CCHRROMPreviewRenderer::initializeGL()
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

   resizeGL(this->width(), this->height());

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

void CCHRROMPreviewRenderer::reloadData(char* imgData)
{
   makeCurrent();

   glBindTexture(GL_TEXTURE_2D, textureID);
   glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_BGRA, GL_UNSIGNED_BYTE, imageData);
   update();
}

void CCHRROMPreviewRenderer::setBGColor(QColor clr)
{
   glClearColor((float)clr.red() / 255.0f, (float)clr.green() / 255.0f, (float)clr.blue() / 255.0f, 0.5f);
}

void CCHRROMPreviewRenderer::resizeGL(int width, int height)
{
   QSize actualSize;

   // Force integral scaling factors. TODO: Add to environment settings.
   int zf  = zoom / 100;

   actualSize.setWidth( 256*zf );
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

   // Set orthogonal mode (since we are doing 2D rendering).
   glOrtho( 0, 1, 1, 0.0f, -1.0f, 1.0f);

   // Select and reset the ModelView matrix.
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // Translate for letter-/pillarboxing
   glScalef( actualSize.width() / float( width )/256, actualSize.height() / float( height )/128, 1 );

   // Slightly offset the view to ensure proper pixel alignment
//    glTranslatef(0.5,0.5,0);
}


void CCHRROMPreviewRenderer::paintGL()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glBindTexture (GL_TEXTURE_2D, textureID);
   glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_BGRA, GL_UNSIGNED_BYTE, imageData);
   glBegin(GL_QUADS);
   glTexCoord2f (0.0, 0.0);
   glVertex3f(000.0f - scrollX, 000.0f - scrollY, 0.0f);
   glTexCoord2f (1.0, 0.0);
   glVertex3f(256.0f - scrollX, 000.0f - scrollY, 0.0f);
   glTexCoord2f (1.0, 0.5);
   glVertex3f(256.0f - scrollX, 128.0f - scrollY, 0.0f);
   glTexCoord2f (0.0, 0.5);
   glVertex3f(000.0f - scrollX, 128.0f - scrollY, 0.0f);
   glEnd();
}

void CCHRROMPreviewRenderer::changeZoom(int newZoom)
{
   makeCurrent();
   zoom = newZoom;
   resizeGL(this->width(), this->height());
   update();
}
