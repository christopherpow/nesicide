#ifndef CNESEMULATORRENDERER_H
#define CNESEMULATORRENDERER_H

#include <QWidget>
#include <QGLWidget>
#include "cgltexturemanager.h"

class CNESEmulatorRenderer : public QGLWidget
{
   Q_OBJECT
public:
    CNESEmulatorRenderer(QWidget *parent, char *imgData);
    ~CNESEmulatorRenderer();
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
    int textureID;
    CGLTextureManager glTextureManager;
};

#endif // CNESEMULATORRENDERER_H
