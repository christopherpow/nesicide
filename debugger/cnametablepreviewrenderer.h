#ifndef CNAMETABLEPREVIEWRENDERER_H
#define CNAMETABLEPREVIEWRENDERER_H

#include <QWidget>
#include <QGLWidget>
#include "cgltexturemanager.h"

class CNameTablePreviewRenderer : public QGLWidget
{
public:
    CNameTablePreviewRenderer(QWidget *parent, char* data);
    ~CNameTablePreviewRenderer();
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
};

#endif // CNAMETABLEPREVIEWRENDERER_H
