#ifndef CCHRROMPREVIEWRENDERER_H
#define CCHRROMPREVIEWRENDERER_H

#include <QWidget>
#include <QGLWidget>
#include "cgltexturemanager.h"

class CCHRROMPreviewRenderer : public QGLWidget
{
public:
    CCHRROMPreviewRenderer(QWidget *parent, char *imgData);
    ~CCHRROMPreviewRenderer();
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void changeZoom(int newZoom);
    void reloadData(char *imgData);
    void setBGColor(QColor clr);
    int zoom;
    int scrollX;
    int scrollY;
    char* imageData;
    int textureID;
    CGLTextureManager glTextureManager;
};

#endif // CCHRROMPREVIEWRENDERER_H
