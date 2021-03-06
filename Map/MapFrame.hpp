#ifndef __MAPFRAME_HPP__
#define __MAPFRAME_HPP__

#include "MapView.hpp"
#include "MapScene.hpp"
#include <QtGui>

namespace Ui {
class MapFrame;
}

class MapFrame : public QFrame
{
    Q_OBJECT

    public:

        explicit MapFrame(QWidget *parent = 0);
        ~MapFrame(void);

        // Getter
        MapView*  view(void) const;
        MapScene* scene(void) const;

    signals:

        void clearTracks(void);

    private slots:

        // Autoconnect
        void on_zoomSlider_valueChanged(int value); // updateScale
        void on_selectPointZoneButton_toggled(bool checked);
        void on_cutSectorButton_toggled(bool checked);
        void on_dragButton_toggled(bool checked);
        void on_clearToolButton_clicked(void);
        void on_eraseSelectionToolButton_clicked(void);

        // Personal slots
        void zoom(int level);
        void zoomAround(int level, QPointF origin);

    protected:

        // GUI
        Ui::MapFrame* ui;

        // Scene and view
        MapScene* mapScene;
        MapView*  mapView;

};

#endif /* __MAPFRAME_HPP__ */
