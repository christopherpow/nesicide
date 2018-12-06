#include "nesemulatorrenderer.h"

#include "main.h"

CNESEmulatorRenderer::CNESEmulatorRenderer(QWidget* parent, char* imgData)
   : QGLWidget(parent)
{
   imageData = imgData;
   scrollX = 0;
   scrollY = 0;
   zoom = 100;
}

CNESEmulatorRenderer::~CNESEmulatorRenderer()
{
   glDeleteTextures(1,&textureID);
}

void CNESEmulatorRenderer::initializeGL()
{
   glGenTextures(1,&textureID);

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

   resizeGL(this->width(), this->height());

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
   if ( linearInterpolation )
   {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   }
   else
   {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   }
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

   // Load the actual texture
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
}

void CNESEmulatorRenderer::setBGColor(QColor clr)
{
   glClearColor((float)clr.red() / 255.0f, (float)clr.green() / 255.0f, (float)clr.blue() / 255.0f, 0.5f);
}

void CNESEmulatorRenderer::resizeGL(int width, int height)
{
   QSize actualSize;
   QPoint offset;
   int realWidth;

   // Force .5x step scaling factors.
   if ( aspect43 )
   {
      realWidth = 292;
   }
   else
   {
      realWidth = 256;
   }

   int iwidth  = width   / (realWidth/2) * (realWidth/2);
   int iheight = height  / 120 * 120;

   if (width >= ((float)height * ((float)realWidth) / 240.0f))
   {
      actualSize.setHeight( iheight );
      actualSize.setWidth( (float)iheight * ((float)realWidth / 240.0f) );
   }
   else
   {
      actualSize.setWidth( iwidth );
      actualSize.setHeight( (float)iwidth * (240.0f / (float)realWidth) );
   }

   // Calculate the offset so that the quad is centered
   offset.setX( ( width  - actualSize.width() ) / 2 );
   offset.setY( ( height - actualSize.height() ) / 2 );

   // Width cannot be 0 or the system will freak out
   if (width == 0)
   {
      width = 1;
   }

   // Initialize our viewpoint using the actual size so 1 point should = 1 pixel.
   glViewport(0, 0, width, height);

   // clear the widget using the background color
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

   // We are using a projection matrix.
   glMatrixMode(GL_PROJECTION);

   // Load the default settings for the matrix.
   glLoadIdentity();

   // Set orthogonal mode (since we are doing 2D rendering).
   glOrtho( 0, 1, 0, 1, -1.0f, 1.0f);

   // Select and reset the ModelView matrix.
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // Translate for letter-/pillarboxing
   glTranslatef( offset.x() / float( width ), offset.y() / float( height ), 0 );
   glScalef( actualSize.width() / float( width ), actualSize.height() / float( height ), 1 );

   // Slightly offset the view to ensure proper pixel alignment
//    glTranslatef(0.5,0.5,0);
}


void CNESEmulatorRenderer::paintGL()
{
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

   if ( linearInterpolation )
   {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   }
   else
   {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   }
   glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
   glBegin(GL_QUADS);
   glTexCoord2f (0.0, 240.f/256);
   glVertex3f(0.0, 0.0, 0.0f);
   glTexCoord2f (1.0, 240.f/256);
   glVertex3f(1.0f, 0.0, 0.0f);
   glTexCoord2f (1.0, 0);
   glVertex3f(1.0f, 1.0f, 0.0f);
   glTexCoord2f (0.0, 0);
   glVertex3f(0.0, 1.0f, 0.0f);
   glEnd();
}

void CNESEmulatorRenderer::changeZoom(int /*newZoom*/)
{
//    zoom = newZoom;
//    resizeGL(this->width(), this->height());
//    this->repaint();
}
