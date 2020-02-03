#ifndef PANZOOMRENDERER_H
#define PANZOOMRENDERER_H

#include <QWidget>
#include <QScrollArea>

#include "crendererbase.h"

namespace Ui {
    class PanZoomRenderer;
}

class PanZoomRenderer : public QWidget
{
    Q_OBJECT
public:
   PanZoomRenderer(int sizeX,int sizeY,int maxZoom,int8_t* imageData,bool showPalette,QWidget *parent = 0);
   PanZoomRenderer(int sizeX,int sizeY,int textureSizeXY,int maxZoom,int8_t* imageData,bool showPalette,QWidget *parent = 0);
   void commonConstructor(bool showPalette);
   virtual ~PanZoomRenderer();
   void reloadData(int8_t* imageData) { renderer->reloadData(imageData); }
   QColor getColor(int idx);
   void showPalette(bool show);
   bool pointToPixel(int ptx,int pty,int* pixx,int* pixy) { return renderer->pointToPixel(ptx,pty,pixx,pixy); }
   QWidget* getRenderer() { return renderer; }

protected:
   void showEvent(QShowEvent *event);
   void resizeEvent(QResizeEvent *event);
   bool eventFilter(QObject *object, QEvent *event);
   void renderer_mousePressEvent(QMouseEvent *event);
   void renderer_mouseMoveEvent(QMouseEvent *event);
   void renderer_wheelEvent(QWheelEvent *event);

signals:
   void repaintNeeded();

public slots:
   void on_zoomSlider_valueChanged(int value);

protected:
   CRendererBase* renderer;
   QScrollArea* scrollArea;
   QPoint pressPos;
   int _sizeX;
   int _sizeY;
   bool _paletteVisible;

   void updateScrollBars();

private:
   Ui::PanZoomRenderer *ui;

private slots:
    void on_horizontalScrollBar_valueChanged(int value);
    void on_verticalScrollBar_valueChanged(int value);
};

#endif // PANZOOMRENDERER_H
