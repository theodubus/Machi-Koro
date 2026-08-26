#ifndef MACHI_KORO_ITEMBOUTON_H
#define MACHI_KORO_ITEMBOUTON_H

#include <QGraphicsObject>
#include <QColor>
#include <QString>

/// Un bouton pose dans la scene.
///
/// La scene remplace la disposition de widgets de l'ancienne interface : il n'y a
/// plus de QPushButton a y mettre. Passer par un QGraphicsProxyWidget serait
/// possible, mais un widget mandataire ne suit pas la mise a l'echelle de la vue
/// et se retrouve a la mauvaise taille des que la fenetre change. On dessine donc
/// le bouton comme le reste du plateau.
class ItemBouton : public QGraphicsObject {
    Q_OBJECT
public:
    ItemBouton(const QString& texte, const QSizeF& taille, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*) override;

    void set_texte(const QString& t);
    void set_actif(bool a);
    bool actif() const { return est_actif; }
    /// Couleur de fond quand le bouton est actif.
    void set_teinte(const QColor& c);

signals:
    void clique();

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent*) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent*) override;
    void mousePressEvent(QGraphicsSceneMouseEvent*) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*) override;

private:
    QString le_texte;
    QSizeF la_taille;
    QColor la_teinte;
    bool est_actif = true;
    bool survole = false;
    bool enfonce = false;
};

#endif //MACHI_KORO_ITEMBOUTON_H
