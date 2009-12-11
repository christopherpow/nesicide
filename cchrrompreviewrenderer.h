#ifndef CCHRROMPREVIEWRENDERER_H
#define CCHRROMPREVIEWRENDERER_H

#include <QWidget>
#include <QGLWidget>

class CCHRROMPreviewRenderer : public QGLWidget
{
public:
    CCHRROMPreviewRenderer(QWidget *parent, char *imgData);
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void changeZoom(int newZoom);
    int zoom;
    int scrollX;
    int scrollY;
    char* imageData;

};

#endif // CCHRROMPREVIEWRENDERER_H
