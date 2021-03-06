#include "PlotScene.hpp"

PlotScene::PlotScene(QWidget* widgetParent, QObject *parent) :
    QGraphicsScene(parent), selectionLocked(0), pointsVisible(true),
    curvesVisible(true), curveLabelsVisible(false), selectedGroup(NULL),
    widgetParent(widgetParent)
{
}

void PlotScene::addCurve(PlotCurve *curve)
{
    // Create object used to color curve randomly
    ColorPicker picker(8);

    // Change the "pen" of each point and line of the curve
    curve->setPen(picker.color(this->curves.size() % 8));

    // Append the cuve to the scene
    this->curves.append(curve);
    this->addItem(curve);

    // Apply the visibility options
    curve->setPointsVisible(this->pointsVisible);
    curve->setCurveVisible(this->curvesVisible);

    // Create a label associated to the new curve
    QLabel* curveLabel = new QLabel(this->widgetParent);
    curveLabel->setVisible(this->curveLabelsVisible);
    this->curveLabels.append(curveLabel);
    //this->addWidget(curveLabel);
}

PlotCurve* PlotScene::addCurve(const QList<QPointF>& points,
                               const QVariant& curveId)
{
    // Create curve with points list
    PlotCurve* curve = new PlotCurve(points, curveId);

    // Add the curve to the scene
    this->addCurve(curve);

    return curve;
}

PlotCurve* PlotScene::addCurve(const QList<IndexedPosition>& points,
                               const QVariant& curveId)
{
    // Create curve with points list
    PlotCurve* curve = new PlotCurve(points, curveId);

    // Add the curve to the scene
    this->addCurve(curve);

    return curve;
}

bool PlotScene::removeCurves(const QVariant& idTrack)
{
    PlotCurve* targetCurve;
    for (int i(0); i < this->curves.count(); ++i)
    {
        targetCurve = this->curves.at(i);

        if(targetCurve->id() == idTrack)
        {
            this->curves.removeAt(i);
            this->removeItem(targetCurve);
            delete targetCurve;
            return true;
        }
    }

    return false;
}

CoordinateItem* PlotScene::nearestCoordinateitemOfX(qreal x) const // NOTE : p-e passe un QPointF à la place ?
{
    if (this->curves.isEmpty()) // NOTE : ajouter || !this->curvesAreVisible() ?
        return NULL;

    return this->curves.at(0)->nearestCoordinateitemsOfX(x);
}

bool PlotScene::curvesAreVisible(void) const
{
    return this->curvesVisible;
}

bool PlotScene::pointsAreVisible(void) const
{
    return this->pointsVisible;
}

bool PlotScene::curveLabelsAreVisible(void) const
{
    return this->curveLabelsVisible;
}

void PlotScene::setCurvesVisible(bool visible)
{
    this->curvesVisible = visible;

    // Change visibility of each curve of the scene
    foreach (PlotCurve* curve, this->curves)
        curve->setCurveVisible(visible);
}

void PlotScene::setPointsVisible(bool visible)
{
    this->pointsVisible = visible;

    // Change visibility of each point of the scene
    foreach (PlotCurve* curve, this->curves)
        curve->setPointsVisible(visible);
}

void PlotScene::setCurveLabelsVisible(bool visible)
{
    this->curveLabelsVisible = visible;

    // Change visibility of the label associate at each curve of the scene
    foreach (QLabel* curveLabel, this->curveLabels)
        curveLabel->setVisible(visible);
}

void PlotScene::clearPlotSelection(void)
{
    if (this->selectedGroup != NULL)
    {
        this->removeItem(this->selectedGroup);
        delete this->selectedGroup;
        this->selectedGroup = NULL;
    }
}

void PlotScene::clearCurves(void)
{
    this->clearPlotSelection();
    this->curves.clear();       // Clear the list of curves
    this->clear();              // Clear the scene

    foreach (QLabel* curveLabel, this->curveLabels)
        delete curveLabel;

    this->curveLabels.clear();  // Clear all the labels associated to the curves
}

void PlotScene::highlightPoint(float timeValue, const QVariant &trackId)
{
    PlotCurve* targetPlotCurve = NULL;

    // Find the curve associate to trackId of the point selected on the mapping
    for(int i(0); targetPlotCurve == NULL && i < this->curves.count(); i++)
        if (this->curves.at(i)->id() == trackId)
            targetPlotCurve = this->curves[i];

    if (targetPlotCurve == NULL)
    {
        qDebug() << "No PlotCurve associate to the trackId found";
        return;
    }

    CoordinateItem* nearestCoord = targetPlotCurve->nearestCoord(timeValue);
    if (nearestCoord == NULL)
        return;

    TickItem* tick = new TickItem(true);
    tick->setPos(nearestCoord->pos());

    /* Create the group that will contain all the GraphicsItem corresponding
     * to the selected zone or point */
    if (this->selectedGroup == NULL)
    {
        this->selectedGroup = new QGraphicsItemGroup;
        this->addItem(this->selectedGroup);
    }

    this->selectedGroup->addToGroup(tick);
    tick->launchAnimation();
}

void PlotScene::highlightPoints(float timeValue, const QVariant& trackId)
{
    PlotCurve* targetPlotCurve = NULL;

    // Find the curve associate to trackId of the point selected on the mapping
    for(int i(0); targetPlotCurve == NULL && i < this->curves.count(); i++)
        if (this->curves.at(i)->id() == trackId)
            targetPlotCurve = this->curves[i];

    if (targetPlotCurve == NULL)
    {
        qDebug() << "No PlotCurve associate to the trackId found";
        return;
    }

    /* Get the sector that contains all the points
     * corresponding to the timeValue */
    AnimateSectorItem* sect = targetPlotCurve->sectorOn(timeValue);
    if (sect == NULL)
    {
        qDebug() << "impossible de déterminer le secteur correspondant "
                 << "à la sélection de la vue mapping";
        return;
    }

    // The sector will have the same color that the curve
    sect->setBrush(targetPlotCurve->getPen().brush());

    /* Create the group that will contain all the GraphicsItem corresponding
     * to the selected zone or point */
    if (this->selectedGroup == NULL)
    {
        this->selectedGroup = new QGraphicsItemGroup;
        this->addItem(this->selectedGroup);
    }

    this->selectedGroup->addToGroup(sect);
    sect->launchAnimation();
}

void PlotScene::highlightSector(float t1, float t2, const QVariant& trackId)
{
    qDebug() << "sector délimité sur mapping. Bornes : " << t1 << t2;

    PlotCurve* targetPlotCurve = NULL;

    // Find the curve associate to trackId of the point selected on the mapping
    for(int i(0); targetPlotCurve == NULL && i < this->curves.count(); i++)
        if (this->curves.at(i)->id() == trackId)
            targetPlotCurve = this->curves[i];

    if (targetPlotCurve == NULL)
    {
        qDebug() << "No PlotCurve associate to the trackId found";
        return;
    }

    /* Get the sector that contains all the points
     * corresponding to the time interval t1 to t2 */
    AnimateSectorItem* sect = targetPlotCurve->sectorOn(t1, t2);
    if (sect == NULL)
    {
        qDebug() << "impossible de déterminer le secteur correspondant "
                 << "à la sélection de la vue mapping";
        return;
    }

    // The sector will have the same color that the curve
    sect->setBrush(targetPlotCurve->getPen().brush());

    /* Create the group that will contain all the GraphicsItem corresponding
     * to the selected zone or point */
    if (this->selectedGroup == NULL)
    {
        this->selectedGroup = new QGraphicsItemGroup;
        this->addItem(this->selectedGroup);
    }

    this->selectedGroup->addToGroup(sect);
    sect->launchAnimation();
}

void PlotScene::highlightOnlySector(float t1, float t2, QVariant trackId)
{
    this->clearPlotSelection();
    this->highlightSector(t1, t2, trackId);
}

void PlotScene::displayLabels(const QPointF& mousePos, const QPointF& scenePos)
{
    QPalette palette;
    PlotCurve* currentCurve = NULL;

    for (int i(0); i < this->curves.count(); i++)
    {
        currentCurve = this->curves.at(i);

        /* Get the CoordinateItem which abscisse is the nearest
         * to the mouse position abscisse */
        CoordinateItem* itemAtMousePos =
                currentCurve->nearestCoordinateitemsOfX(scenePos.x());

        if (itemAtMousePos == NULL) continue;

        // Get the label associate to the curve
        QLabel* curveLabel = this->curveLabels.at(i);
        if (curveLabel == NULL) continue;

        // Change the text color of the label associate to the curve
        palette.setColor(QPalette::WindowText, currentCurve->getPen().color());
        curveLabel->setPalette(palette);

        // Change the text of the label associate to the curve
        curveLabel->setText(QString("%1, %2").arg(
                                itemAtMousePos->x(), 6, 'f', 2).arg(
                                itemAtMousePos->y(), 6, 'f', 2));
        curveLabel->adjustSize();

        // Move the label
        curveLabel->move(mousePos.x(), mousePos.y() - (i * 12));
    }
}

void PlotScene::handleSelection(void)
{
    if (this->selectionLocked > 0)
        return;

    QList<QGraphicsItem*> items = this->selectedItems();

    /* On n'a sélectionné qu'un élément, ça n'est donc pas une selection
     * rectangulaire --> on a sélectionné un point
     */
    if (items.count() == 1)
    {
        CoordinateItem* coord = qgraphicsitem_cast<CoordinateItem*>(items.first());

        if (coord != NULL)
        {
            PlotCurve* parent = qgraphicsitem_cast<PlotCurve*>(coord->parentItem());

            if (parent != NULL)
                emit this->pointSelected(coord->index().toFloat(), parent->id());
        }
    }
    else if (items.count() > 1)
    {
        QMap< PlotCurve*, QPair<float, float> > intervalsBounds;

        foreach (QGraphicsItem* item, items)
        {
            CoordinateItem* coord = qgraphicsitem_cast<CoordinateItem*>(item);

            if (coord != NULL)
            {
                PlotCurve* parent = qgraphicsitem_cast<PlotCurve*>(coord->parentItem());

                if (parent != NULL)
                {
                    if (! intervalsBounds.contains(parent))
                    {
                        intervalsBounds[parent].first = coord->index().toFloat();
                        intervalsBounds[parent].second = coord->index().toFloat();
                    }
                    else
                    {
                        if (coord->index().toFloat() < intervalsBounds[parent].first)
                            intervalsBounds[parent].first = coord->index().toFloat();
                        else if (coord->index().toFloat() > intervalsBounds[parent].second)
                            intervalsBounds[parent].second = coord->index().toFloat();
                    }
                }
            }
        }

        foreach (PlotCurve* curve, intervalsBounds.keys())
        {
            QPair<float, float> bounds = intervalsBounds[curve];
            emit this->intervalSelected(bounds.first, bounds.second, curve->id());
        }
    }
}

void PlotScene::lockSelectionAbility(void)
{
    this->selectionLocked++;
}

void PlotScene::unlockSelectionAbility(void)
{
    this->selectionLocked--;

    if (this->selectionLocked <= 0)
    {
        this->selectionLocked = 0;
        this->handleSelection();
    }
}

void PlotScene::slotDeTest(const QPointF &scenePos)
{
    foreach (PlotCurve* curve, this->curves)
    {
        CoordinateItem* itemAtMousePos =
                curve->nearestCoordinateitemsOfX(scenePos.x());

        if (itemAtMousePos)
            emit this->pointSelected(itemAtMousePos->index().toFloat(),
                                     curve->id());
    }
}
