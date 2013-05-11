#include "AdvancedPlot.hpp"

AdvancedPlot::AdvancedPlot(QString const& title, int nbColor, QWidget* parent) :
    AdvancedPlot(QwtText(title), nbColor, parent)
{
    // Delegating constructors only available with -std=c++11 or -std=gnu++11
}

AdvancedPlot::AdvancedPlot(QwtText const& title, int nbColor, QWidget* parent) :
    Plot(title, parent), colorPicker(nbColor)
{
    /* je ne pouvais pas utiliser le Zoomer comme picker car il ne génère pas
     * d'évènement lors d'un simple clic, donc dans tous les cas, je devais
     * ajouter un nouveau picker */

    // Picker with drag rect machine to provide multiple points selection
    QwtPlotPicker* rectPicker = new QwtPlotPicker(this->canvas());
    rectPicker->setStateMachine(new QwtPickerDragRectMachine);
    connect(rectPicker, SIGNAL(selected(QRectF)),
            this, SLOT(selectInterval(QRectF)));
}

AdvancedPlot::~AdvancedPlot(void)
{
    qDebug() << "AdvancedPlot (" << this->objectName() << ") Début destructeur";

    // Supprimer les courbes qui on des enfants
    foreach (QwtPlotItem* item, this->itemList(QwtPlotItem::Rtti_PlotCurve))
    {
        TrackPlotCurve* curve = (TrackPlotCurve*) item;

        if(curve == NULL)
            continue;

        if (curve->parent() == NULL)
            delete curve;
    }

    qDebug() << "AdvancedPlot (" << this->objectName() << ") fin destructeur";
}

QPlotCurve* AdvancedPlot::addCurve(const QString& title,
                                  const QVector<QPointF>& points)
{
    return Plot::addCurve(
               title, points,this->colorPicker.light(this->itemList().count()));
}

void AdvancedPlot::selectPoint(const QPointF &pos)
{
    qDebug() << "On cherche le point le plus proche du curseur ...";

    double minimalDistance(10); // Défini la distance limite à partir de laquelle on considère qi'on est proche de la courbe
    QPointF closestPoint;
    TrackPlotCurve* curve = NULL;

    foreach (QwtPlotItem* item, this->itemList(TrackPlotCurve::Rtti_TrackPlotCurve))
    {
        TrackPlotCurve* tmpCurve = (TrackPlotCurve*) item;

         // la seletion ne peut se faire que sur une courbe parente et visible
        if(tmpCurve == NULL || tmpCurve->parent() != NULL || !tmpCurve->isVisible())
            continue;

//        if (!tmpCurve->boundingRect().contains(pos))
//            continue;

        double tmpDist;
        QPointF tmpClosestPoint = tmpCurve->closestPointF(pos, &tmpDist);

        qDebug() << "Point temporaire le plus proche = " << tmpClosestPoint
                 << " distance = " << tmpDist;

        if (tmpDist < minimalDistance)
        {
            minimalDistance = tmpDist;
            closestPoint = tmpClosestPoint;
            curve = tmpCurve;
        }
    }

    // Si aucun point proche d'une des courbe n'a été trouvé
    if(!curve)
    {
        qDebug() << "Aucun point proche...";
        return;
    }

    qDebug() << "Point le plus proche = " << closestPoint << " distance = " << minimalDistance;
    emit this->pointSelected(closestPoint.x(), curve->trackIdentifier());
}

void AdvancedPlot::selectPoints(const QPointF &pos)
{
    qDebug() << "On cherche les points les plus proche de la valeur en abscisse de la position du curseur ...";

    foreach (QwtPlotItem* item, this->itemList(TrackPlotCurve::Rtti_TrackPlotCurve))
    {
        TrackPlotCurve* curve = (TrackPlotCurve*) item;

         // la seletion ne peut se faire que sur une courbe parente et visible
        if(curve == NULL || curve->parent() != NULL || !curve->isVisible())
            continue;

        double dist;
        QPointF closestPoint = curve->closestPointFOfX(pos.x(), &dist);

        if(dist < 10)
        {
            qDebug() << "Point = " << closestPoint;
            emit this->pointSelected(closestPoint.x(), curve->trackIdentifier());
        }
    }
}

void AdvancedPlot::selectInterval(const QRectF &selectedRect)
{
    if (selectedRect.left() == selectedRect.right() &&
        selectedRect.top()  == selectedRect.bottom())
    {
        if (this->isCrossLineVisible())
            this->selectPoints(selectedRect.topLeft()); // Top left au hasard, les 4 extrémités sont sur le meme point
        else
            this->selectPoint(selectedRect.topLeft());  // Top left au hasard, les 4 extrémités sont sur le meme point

        return;
    }

    /* ---------------------------------------------------------------------- *
     *                  Sélection d'une zone rectangulaire                    *
     * ---------------------------------------------------------------------- */

    qDebug() << "Rectangle de sélection : Left = " << selectedRect.left() << " right = " << selectedRect.right();

    foreach (QwtPlotItem* item, this->itemList(TrackPlotCurve::Rtti_TrackPlotCurve))
    {
        TrackPlotCurve* curve = (TrackPlotCurve*) item;

        // la seletion ne peut se faire que sur une courbe parente et visible
        if(curve == NULL || curve->parent() != NULL || !curve->isVisible())
            continue;

        if (!curve->boundingRect().intersects(selectedRect))
            continue;

        // Récupérer la liste des points de la courbe
        QwtSeriesData<QPointF>* curvePoints = curve->data();

        QPolygonF pointsSelected;

        qDebug() << "Nombre de points dans la courbe " << curve->title().text()
                 << " = " << curvePoints->size();

        unsigned int i;
        for(i = 0;i < curvePoints->size()
                   && curvePoints->sample(i).x() < selectedRect.left(); ++i);

        for(; i < curvePoints->size()
               && curvePoints->sample(i).x() < selectedRect.right(); ++i)
            if (selectedRect.contains(curvePoints->sample(i)))
                pointsSelected << curvePoints->sample(i);

        if (!pointsSelected.isEmpty())
        {
            qDebug() << "Premier = " << pointsSelected.first() << " dernier = " << pointsSelected.last();

            emit this->intervalSelected(pointsSelected.first().x(),
                                        pointsSelected.last().x(),
                                        curve->trackIdentifier());
        }
    }
}
