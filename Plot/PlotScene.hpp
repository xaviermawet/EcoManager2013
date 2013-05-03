#ifndef __PLOTSCENE_HPP__
#define __PLOTSCENE_HPP__

#include "PlotCurve.hpp"
#include "../Utils/ColorPicker.hpp"
#include "../Map/TickItem.hpp"
#include <QtGui>

class PlotScene : public QGraphicsScene
{
    Q_OBJECT

    public:

        PlotScene(QWidget *widgetParent, QObject* parent = 0);

        void addCurve(PlotCurve* curve);
        PlotCurve* addCurve(const QList<QPointF>& points,
                            const QVariant& curveId = QVariant());
        PlotCurve* addCurve(const QList<IndexedPosition>& points,
                            const QVariant& curveId = QVariant());
        bool removeCurves(const QVariant& idTrack);

        CoordinateItem* nearestCoordinateitemOfX(qreal x) const; // Return NULL if no curve available

        bool curvesAreVisible(void) const;
        bool pointsAreVisible(void) const;
        bool curveLabelsAreVisible(void) const;

    signals:

        void pointSelected(float absciss, const QVariant& idTrack);
        void intervalSelected(float firstAbsciss, float secondAbsciss,
                              const QVariant& idTrack);

    public slots:

        void setCurvesVisible(bool visible);
        void setPointsVisible(bool visible);
        void setCurveLabelsVisible(bool visible);
        void clearPlotSelection(void);
        void clearCurves(void);
        void highlightPoint(float timeValue, const QVariant& trackId);
        void highlightPoints(float timeValue, const QVariant& trackId);
        void highlightSector(float t1, float t2, const QVariant& trackId);
        void highlightOnlySector(float t1, float t2, QVariant trackId);
        void displayLabels(const QPointF& mousePos, const QPointF& scenePos);
        void handleSelection(void);
        void lockSelectionAbility(void);
        void unlockSelectionAbility(void);

        void slotDeTest(const QPointF& scenePos);

    protected:

        int  selectionLocked;
        bool pointsVisible;
        bool curvesVisible;
        bool curveLabelsVisible;

        QList<PlotCurve*> curves;
        QList<QLabel*> curveLabels;
        QGraphicsItemGroup* selectedGroup;

        QWidget* widgetParent;
};

#endif /* __PLOTSCENE_HPP__ */
