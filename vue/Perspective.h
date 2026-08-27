#ifndef MACHI_KORO_PERSPECTIVE_H
#define MACHI_KORO_PERSPECTIVE_H

#include <QPointF>
#include <QRectF>
#include <QPolygonF>

/// La table, et la camera qui la regarde.
///
/// Les cartes ne sont pas alignees dans un rectangle ni dressees face a nous :
/// elles sont **posees a plat sur une table ronde**, filmee d'au-dessus et de
/// trois quarts. Une carte du fond est donc plus petite ET plus ecrasee qu'une
/// carte du bord proche, et le tapis rond se projette en ovale.
///
/// Le modele est une camera reelle : placee a la hauteur `HAUTEUR_CAMERA`
/// au-dessus du plan et reculee de `RECUL`, elle vise le centre de la table. Le
/// rapport entre les deux fixe l'**inclinaison** — c'est le seul reglage qui
/// compte, et le seul a toucher si le rendu penche trop ou pas assez :
///
///   - camera basse  : la table s'aplatit, les cartes deviennent des traits ;
///   - camera haute  : on tombe sur une vue de dessus, la table redevient ronde.
///
/// Deux coordonnees decrivent une place sur la table, dans le plan lui-meme :
///
///   `u` — l'ecart lateral au centre, en unites de table ;
///   `v` — la profondeur, de 0 au fond de la table a 1 au bord proche.
namespace Perspective {

    /// Rectangle ecran ou s'inscrit le tapis projete.
    QRectF tapis();

    /// Projection d'un point du plan de la table.
    QPointF projeter(qreal u, qreal v);

    /// Facteur de reduction a la profondeur `v`. 1 au centre de la table.
    qreal echelle(qreal v);

    /// Les quatre coins ecran d'une carte **couchee** sur la table, de largeur
    /// `largeur` mesuree dans le plan, centree en `u`, son bord proche en `v`.
    /// Dans l'ordre attendu par QTransform::quadToQuad : haut-gauche, haut-droit,
    /// bas-droit, bas-gauche.
    QPolygonF quad_carte(qreal u, qreal v, qreal largeur);

    /// Demi-largeur utilisable a la profondeur `v` : au-dela, on quitte le feutre.
    qreal demi_largeur(qreal v);

    /// Demi-largeur que le **cadre** laisse voir a la profondeur `v`, en unites
    /// de table, une fois retiree `marge` pixels de chaque cote. La table etant
    /// plus large que l'ecran, c'est souvent elle qui limite, pas le feutre.
    qreal place_ecran(qreal v, qreal marge = 0);

    /// Profondeur qu'occupe une carte de cette largeur, en unites de `v`.
    qreal profondeur_carte(qreal largeur);
}

#endif //MACHI_KORO_PERSPECTIVE_H
