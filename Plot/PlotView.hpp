#ifndef __PLOTVIEW_HPP__
#define __PLOTVIEW_HPP__

#include <QtGui>

class PlotView : public QGraphicsView
{
    Q_OBJECT

    public:

        PlotView(QWidget* parent = 0);
        PlotView(QGraphicsScene* scene, QWidget* parent = 0);
        virtual ~PlotView(void);

    signals:

        void positionChange(QPoint);
        void rectChange(QRectF);
        void beginSelection(void);
        void finishSelection(void);

    public slots:

       void updateSceneRect(const QRectF &rect);
       void selectionChanged(void);
       void zoomOut(void);
       void toggleSelectionMode(void);

    protected:

       bool clicked;
       bool delimiting;
       bool moving;
       QStack<QRectF> sceneStack;

       virtual void drawForeground(QPainter *painter, const QRectF &rect);
       virtual void mouseMoveEvent(QMouseEvent*);
       virtual void mousePressEvent(QMouseEvent*);
       virtual void mouseReleaseEvent(QMouseEvent*);
       virtual void resizeEvent(QResizeEvent*);
       //virtual void wheelEvent(QWheelEvent*);

    private:

       void init(void);
       QRectF globalRect(void) const;

       QLabel* posLabel;
};

#endif // PLOTVIEW_HPP