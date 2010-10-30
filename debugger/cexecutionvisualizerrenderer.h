#ifndef CEXECUTIONVISUALIZERRENDERER_H
#define CEXECUTIONVISUALIZERRENDERER_H

#include <QWidget>
#include <QGLWidget>
#include "cgltexturemanager.h"

class CExecutionVisualizerRenderer : public QGLWidget
{
public:
    CExecutionVisualizerRenderer(QWidget *parent, char* data);
    ~CExecutionVisualizerRenderer();
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void updateGL();
    void changeZoom(int newZoom);
    void changeImage(char* data) { imageData = data; }
    void setBGColor(QColor clr);
    int zoom;
    int scrollX;
    int scrollY;
    char* imageData;
    int textureID;
};

#endif // CEXECUTIONVISUALIZERRENDERER_H
