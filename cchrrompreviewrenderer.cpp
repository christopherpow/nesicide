#include "cchrrompreviewrenderer.h"

void CCHRROMPreviewRenderer::initializeGL()
{
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

    resizeGL(this->width(), this->height());
}

void CCHRROMPreviewRenderer::resizeGL(int width, int height)
{

    // Zoom the width and height based on our view zoom. If zoom is 200% and our width is 100
    // then the renderer's width will be 50.
    int newWidth = (int)((float)width / ((float)zoom / 100.0f));
    int newHeight = (int)((float)height / ((float)zoom / 100.0f));

    // Width cannot be 0
    if (width == 0)
        width = 1;

    // Initialize our viewpoint using the actual size so 1 point should = 1 pixel.
    glViewport(0, 0, width, height);

    // We are using a projection matrix.
    glMatrixMode(GL_PROJECTION);

    // Load the default settings for the matrix.
    glLoadIdentity();

    // Set orthogonal mode (since we are doing 2D rendering) with the proper aspect ratio.
    glOrtho(0.0f, newWidth, newHeight, 0.0f, -1.0f, 1.0f);

    // Select and reset the ModelView matrix.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void CCHRROMPreviewRenderer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBegin(GL_QUADS);
        glVertex3f(050.0f, 100.0f, 0.0f);
        glVertex3f(100.0f, 100.0f, 0.0f);
        glVertex3f(100.0f, 050.0f, 0.0f);
        glVertex3f(050.0f, 050.0f, 0.0f);
    glEnd();
}

void CCHRROMPreviewRenderer::changeZoom(int newZoom)
{
    zoom = newZoom;
    resizeGL(this->width(), this->height());
    this->repaint();

}
