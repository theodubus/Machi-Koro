#include "ItemBouton.h"
#include "Decor.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>

ItemBouton::ItemBouton(const QString& texte, const QSizeF& taille, QGraphicsItem* parent)
        : QGraphicsObject(parent), le_texte(texte), la_taille(taille),
          la_teinte(Decor::Palette::encre()) {
    setAcceptHoverEvents(true);
}

QRectF ItemBouton::boundingRect() const {
    return QRectF(0, 0, la_taille.width(), la_taille.height());
}

void ItemBouton::set_texte(const QString& t) { le_texte = t; update(); }

void ItemBouton::set_actif(bool a) {
    if (est_actif == a) return;
    est_actif = a;
    // Un bouton qu'on vient de desactiver ne doit pas rester en surbrillance :
    // la souris peut le quitter sans qu'aucun evenement ne parvienne a l'item.
    if (!a) { survole = false; enfonce = false; }
    setCursor(QCursor(a ? Qt::PointingHandCursor : Qt::ArrowCursor));
    update();
}

void ItemBouton::set_teinte(const QColor& c) { la_teinte = c; update(); }

void ItemBouton::hoverEnterEvent(QGraphicsSceneHoverEvent*) {
    if (!est_actif) return;
    survole = true;
    update();
}

void ItemBouton::hoverLeaveEvent(QGraphicsSceneHoverEvent*) {
    survole = false;
    enfonce = false;
    update();
}

void ItemBouton::mousePressEvent(QGraphicsSceneMouseEvent* e) {
    if (!est_actif || e->button() != Qt::LeftButton) { e->ignore(); return; }
    enfonce = true;
    update();
    e->accept();
}

void ItemBouton::mouseReleaseEvent(QGraphicsSceneMouseEvent* e) {
    if (!enfonce) { e->ignore(); return; }
    enfonce = false;
    update();
    // Relacher hors du bouton annule le clic, comme partout ailleurs.
    if (boundingRect().contains(e->pos())) emit clique();
    e->accept();
}

void ItemBouton::paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*) {
    p->setRenderHint(QPainter::Antialiasing);
    const QRectF r = boundingRect();
    const qreal rayon = r.height() / 2.6;

    QColor fond = est_actif ? la_teinte : QColor(214, 210, 200);
    if (est_actif && survole) fond = fond.lighter(118);
    if (est_actif && enfonce) fond = fond.darker(112);

    // Un lisere clair sous le bouton lui donne du relief sans passer par un
    // effet graphique, qui serait recalcule a chaque repeint.
    p->setPen(Qt::NoPen);
    p->setBrush(fond.darker(125));
    p->drawRoundedRect(r.adjusted(0, 3, 0, 0), rayon, rayon);
    p->setBrush(fond);
    p->drawRoundedRect(enfonce ? r.adjusted(0, 2, 0, 2) : r, rayon, rayon);

    QFont f = p->font();
    f.setBold(true);
    f.setPixelSize(qMax(11, int(r.height() * 0.40)));
    p->setFont(f);
    p->setPen(est_actif ? Qt::white : QColor(150, 146, 138));
    p->drawText(enfonce ? r.adjusted(0, 2, 0, 2) : r, Qt::AlignCenter, le_texte);
}
