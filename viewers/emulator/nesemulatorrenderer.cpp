#include "nesemulatorrenderer.h"

#include "main.h"

CNESEmulatorRenderer::CNESEmulatorRenderer(QWidget *parent, char *imgData)
    : QGLWidget(parent)
{
   imageData = imgData;
   scrollX = 0;
   scrollY = 0;
   zoom = 100;
}

CNESEmulatorRenderer::~CNESEmulatorRenderer()
{
   CGLTextureManager::freeTextureID(textureID);
}

void CNESEmulatorRenderer::initializeGL()
{
   textureID = CGLTextureManager::getNewTextureID();
   zoom = 100;

   makeCurrent();

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
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
}

void CNESEmulatorRenderer::updateGL()
{
   makeCurrent();

   glBindTexture(GL_TEXTURE_2D, textureID);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
   repaint();
}

void CNESEmulatorRenderer::setBGColor(QColor clr)
{
   glClearColor((float)clr.red() / 255.0f, (float)clr.green() / 255.0f, (float)clr.blue() / 255.0f, 0.5f);
}

void CNESEmulatorRenderer::resizeGL(int width, int height)
{
   // First determine the scale factor (ratio), and the zoom factor (reverseRatio)
   float ratio, zoomFactor;
   int padding;
   QRect actualSize;
   if (width >= ((float)height * (256.0f / 240.0f)))
   {
      // The screen size is determined by the max height
      ratio = ((float)height / 240.0f);
      zoomFactor = (240.0f / (float)height);

      // The top and bottom are known since it fills the screen
      actualSize.setTop(0);
      actualSize.setBottom(240); // We clip the last 16 pixels as they don't matter

      // Determine the left offset
      actualSize.setLeft(-(((240.0f * ((float)width / (float)height)) - 256.0f) / 2.0f));

      // Scale up 256 by the ratio to get the correct aspect ratio
      actualSize.setWidth(240.0f * ((float)width / (float)height));

   } else {
      // The screen size is determined by the max width
      ratio = 1;
      zoomFactor = (256.0f / (float)height);

      // The left and right are known since it fills the screen
      actualSize.setLeft(0);
      actualSize.setRight(256);

      // Determine the top offset
      actualSize.setTop(-(((256.0f * ((float)height / (float)width)) - 240.0f) / 2.0f));

      // Scale up 240 by the ratio to get the correct aspect ratio
      actualSize.setHeight(256.0f * ((float)height / (float)width));
   }


   // Let opengl know which surface we are working with
   makeCurrent();

   // Width cannot be 0 or the system will freak out
   if (width == 0)
     width = 1;

   // Initialize our viewpoint using the actual size so 1 point should = 1 pixel.
   glViewport(0, 0, width, height);

   // We are using a projection matrix.
   glMatrixMode(GL_PROJECTION);

   // Load the default settings for the matrix.
   glLoadIdentity();

   // Set orthogonal mode (since we are doing 2D rendering) with the proper aspect ratio.
   glOrtho(actualSize.left(), actualSize.right(), actualSize.bottom(), actualSize.top(), -1.0f, 1.0f);

   // Select and reset the ModelView matrix.
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // Slightly offset the view to ensure proper pixel alignment
//    glTranslatef(0.5,0.5,0);
}


void CNESEmulatorRenderer::paintGL()
{
   // Select this renderer as the current so opengl paints to the right surface
   makeCurrent();

   float x = 0;
   float y = 0;

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glBindTexture (GL_TEXTURE_2D, textureID);
   glBegin(GL_QUADS);
      glTexCoord2f (0.0, 0.0);
      glVertex3f(x, y, 0.0f);
      glTexCoord2f (1.0, 0.0);
      glVertex3f(x+256.0f, y, 0.0f);
      glTexCoord2f (1.0, 1.0);
      glVertex3f(x+256.0f, y+256.0f, 0.0f);
      glTexCoord2f (0.0, 1.0);
      glVertex3f(x, y+256.0f, 0.0f);
   glEnd();

}

void CNESEmulatorRenderer::changeZoom(int newZoom)
{
//    zoom = newZoom;
//    resizeGL(this->width(), this->height());
//    this->repaint();
}
