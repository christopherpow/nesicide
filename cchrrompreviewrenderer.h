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
};

#endif // CCHRROMPREVIEWRENDERER_H
