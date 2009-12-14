#ifndef CNESEMULATORRENDERER_H
#define CNESEMULATORRENDERER_H

#include <QWidget>
#include <QGLWidget>

class CNESEmulatorRenderer : public QGLWidget
{
public:
    CNESEmulatorRenderer(QWidget *parent, char *imgData);
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void updateGL();
    void changeZoom(int newZoom);
    void setBGColor(QColor clr);
    int zoom;
    int scrollX;
    int scrollY;
    char* imageData;

};

#endif // CNESEMULATORRENDERER_H
