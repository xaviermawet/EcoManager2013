#include "TrackPlotCurve.hpp"

TrackPlotCurve::TrackPlotCurve(
        const QString &title, const TrackIdentifier &trackId,
        const QPen &pen, TrackPlotCurve *parent) :
    TrackPlotCurve(QwtText(title), trackId, pen, parent)
{
    // Delegating constructors only available with -std=c++11 or -std=gnu++11
}

TrackPlotCurve::TrackPlotCurve(
        QwtText const& title, TrackIdentifier const& trackId,
        QPen const& pen, TrackPlotCurve* parent) :
    QPlotCurve(title, pen), _trackIdentifier(trackId), parentCurve(NULL)
{
    this->attachTo(parent);

    this->setTitle(title.text() + QObject::tr(" (course ") +
                   trackId["race_num"].toString() + QObject::tr(" tour ") +
            trackId["lap"].toString() + ")");

    this->setSymbol(new QwtSymbol( QwtSymbol::Ellipse,
                                   Qt::gray, pen, QSize( 2, 2 ) ) );
}

TrackPlotCurve::~TrackPlotCurve(void)
{
    qDebug() << "TrackPlotCurve, suppression de : " << this->title().text();

    foreach (TrackPlotCurve* childCurve, this->children)
            delete childCurve;
}

TrackIdentifier TrackPlotCurve::trackIdentifier(void) const
{
    return this->_trackIdentifier;
}

TrackPlotCurve const* TrackPlotCurve::parent(void) const
{
    return this->parentCurve;
}

QRectF TrackPlotCurve::boundingRect(void) const
{
    QRectF rect = QwtPlotCurve::boundingRect();

    foreach (TrackPlotCurve* child, this->children) {
        rect = rect.united(child->boundingRect());
    }

    return rect;
}

void TrackPlotCurve::setVisible(bool visible)
{
    qDebug() << "On change la visibilité ...";

    QPlotCurve::setVisible(visible);

    // Hide or show child curves
    foreach (TrackPlotCurve* childCurve, this->children)
        childCurve->setVisible(visible);
}

void TrackPlotCurve::attachChild(TrackPlotCurve* child)
{
    if (child == NULL)
        return;

    // Detach the child from previous parent
    child->detachFromParentCurve();

    this->children.append(child);
    child->parentCurve = this;

    // Change legend item visibility to false (only the parent is visible)
    child->setItemAttribute(Legend, false);

    // Attach the child in the same plot as its new parent
    child->attach(this->plot());

    child->_trackIdentifier = this->_trackIdentifier;
}

void TrackPlotCurve::attachTo(TrackPlotCurve* parent)
{
    if(parent == NULL)
    {
        this->detachFromParentCurve();
        return;
    }

    parent->attachChild(this);
}

void TrackPlotCurve::detachFromParentCurve(void)
{
    if (parentCurve == NULL)
        return;

    this->parentCurve->removeChild(this);
}

bool TrackPlotCurve::removeChild(TrackPlotCurve * const &child)
{
    if(!this->children.contains(child))
        return false;

    if(!this->children.removeOne(child))
        return false;

    child->parentCurve = NULL;

    // Change legend item visibility
    child->setItemAttribute(Legend, true);

    return true;
}

void TrackPlotCurve::attach(QwtPlot *plot)
{
    qDebug() << "On attache la courbe courante et tous ses enfants ...";

    QPlotCurve::attach(plot);

    foreach (TrackPlotCurve* childCurve, this->children)
        childCurve->attach(plot);
}

void TrackPlotCurve::detach(void)
{
    qDebug() << "On détache la courbe courante et tous ses enfants ...";

    QPlotCurve::detach();

    foreach (TrackPlotCurve* childCurve, this->children)
        childCurve->detach();
}

QPointF TrackPlotCurve::closestPointF(QPointF const& pos, double* dist) const
{
    // Récupérer la liste des points
    const QwtSeriesData<QPointF>* curvePoints = this->data();

    // Effectuer une recherche dichotomique sur la valeur en x
    unsigned int indiceMin(0);
    unsigned int indiceMax(curvePoints->size() - 1);

    while (indiceMin <= indiceMax)
    {
        unsigned int mid = (indiceMin + indiceMax) / 2;

        if (curvePoints->sample(mid).x() < pos.x())
            indiceMin = mid + 1;
        else
            indiceMax = mid - 1;
    }

    // Ce point est le point juste à droite de pos mais celui de gauche est p-e plus pres
    if (indiceMin == 0)
    {
        QPointF pointF = curvePoints->sample(indiceMax);
        if (dist)
            *dist = qAbs(pointF.x() - pos.x()) + qAbs(pointF.y() - pos.y());
        return pointF;
    }

    qDebug() << "indiceMin = " << indiceMin << " indiceMax = " << indiceMax;
    QPointF pointF1 = curvePoints->sample(indiceMin); // le point après la pos
    QPointF pointF2 = curvePoints->sample(indiceMax); // Le point avant la pos

    double distF1 = qAbs(pointF1.x() - pos.x()) + qAbs(pointF1.y() - pos.y());
    double distF2 = qAbs(pointF2.x() - pos.x()) + qAbs(pointF2.y() - pos.y());

    if (distF1 < distF2)
    {
        if (dist)
            *dist = distF1;
        return pointF1;
    }

    if (dist)
        *dist = distF2;
    return pointF2;

//    if (qAbs(pointF1.x() - pos.x()) < qAbs(pointF2.x() - pos.x()))
//    {
//        if (dist != NULL)
//            *dist = qAbs(pointF1.x() - pos.x());
//        return pointF1;
//    }

//    if (dist!= NULL)
//        *dist = qAbs(pointF2.x() - pos.x());
//    return pointF2;
}
