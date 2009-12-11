#ifndef CCHRROMPREVIEWRENDERER_H
#define CCHRROMPREVIEWRENDERER_H

#include <QGLWidget>

class CCHRROMPreviewRenderer : public QGLWidget
{
public:
    CCHRROMPreviewRenderer(QWidget *parent) : QGLWidget(parent) {}
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void changeZoom(int newZoom);
    int zoom;
};

#endif // CCHRROMPREVIEWRENDERER_H
