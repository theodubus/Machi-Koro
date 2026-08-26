#include "VuePlateau.h"
#include "ScenePlateau.h"
#include "Decor.h"

#include <QResizeEvent>

VuePlateau::VuePlateau(QWidget* parent) : QGraphicsView(parent) {
    la_scene = new ScenePlateau(this);
    setScene(la_scene);

    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform |
                   QPainter::TextAntialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameShape(QFrame::NoFrame);
    // Le ciel de la scene couvre tout le rectangle logique ; ce qui deborde autour
    // quand les proportions de la fenetre different reprend la meme couleur, sinon
    // deux bandes grises encadrent le plateau.
    setBackgroundBrush(Decor::Palette::ciel_bas());
    // Les cartes se redressent au survol : sans cette ancre, la vue recentrerait
    // la scene a chaque fois que la taille apparente d'un item change.
    setTransformationAnchor(QGraphicsView::NoAnchor);
    setResizeAnchor(QGraphicsView::NoAnchor);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setMinimumSize(880, 500);
}

void VuePlateau::ajuster() {
    fitInView(la_scene->sceneRect(), Qt::KeepAspectRatio);
}

void VuePlateau::resizeEvent(QResizeEvent* e) {
    QGraphicsView::resizeEvent(e);
    ajuster();
}

void VuePlateau::showEvent(QShowEvent* e) {
    QGraphicsView::showEvent(e);
    ajuster();
}
