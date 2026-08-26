#include "Perspective.h"

#include <QtMath>
#include <algorithm>

namespace {
    // --- La table, dans son propre plan ---
    const qreal RAYON = 620;          ///< rayon du tapis, en unites de table

    // --- L'inclinaison de la camera, le seul reglage qui compte ---
    //
    // C'est le rapport HAUTEUR / RECUL qui decide de tout : plus la camera est
    // haute, plus on voit la table de dessus et moins les cartes couchees sont
    // ecrasees. A ces valeurs on la regarde d'environ trente-trois degres — assez
    // haut pour qu'une carte posee reste identifiable, assez bas pour qu'on voie
    // bien une table devant soi et non un plan vu a la verticale.
    const qreal HAUTEUR_CAMERA = 1100;
    const qreal RECUL          = 1670;

    // --- Ou et de quelle taille la table se pose a l'ecran ---
    const qreal CX             = 800;
    const qreal LARGEUR_TAPIS  = 1330;   ///< largeur voulue du tapis projete
    const qreal CENTRE_TAPIS_Y = 496;    ///< milieu vertical du tapis projete

    const qreal L2 = HAUTEUR_CAMERA * HAUTEUR_CAMERA + RECUL * RECUL;
    const qreal L  = qSqrt(L2);

    /// Profondeur dans le plan : +RAYON au fond, -RAYON au bord proche.
    qreal profondeur(qreal v) { return RAYON * (1.0 - 2.0 * std::clamp(v, -2.0, 2.0)); }

    /// Denominateur de la projection : la distance a la camera, au facteur pres.
    qreal denominateur(qreal y) { return y * RECUL + L2; }

    /// Focale et centre vertical, calcules une fois pour que le tapis projete
    /// tombe exactement a la taille et a la place voulues. Les deviner a la main
    /// obligeait a reprendre tous les autres nombres a chaque fois qu'on touchait
    /// a l'inclinaison.
    struct Cadrage {
        qreal focale;
        qreal cy;
        Cadrage() {
            qreal x0 = 1e9, x1 = -1e9, y0 = 1e9, y1 = -1e9;
            for (int i = 0; i < 720; i++) {
                const qreal a = 2 * M_PI * i / 720.0;
                const qreal x = RAYON * qCos(a), y = RAYON * qSin(a);
                const qreal den = std::max(qreal(1.0), denominateur(y));
                const qreal px = L * x / den;              // focale 1
                const qreal py = -HAUTEUR_CAMERA * y / den;
                x0 = std::min(x0, px); x1 = std::max(x1, px);
                y0 = std::min(y0, py); y1 = std::max(y1, py);
            }
            focale = LARGEUR_TAPIS / (x1 - x0);
            cy = CENTRE_TAPIS_Y - focale * (y0 + y1) / 2.0;
        }
    };
    const Cadrage& cadrage() { static Cadrage c; return c; }

    QPointF projeter_plan(qreal x, qreal y) {
        const qreal den = std::max(qreal(1.0), denominateur(y));
        return QPointF(CX + cadrage().focale * L * x / den,
                       cadrage().cy - cadrage().focale * HAUTEUR_CAMERA * y / den);
    }
}

QPointF Perspective::projeter(qreal u, qreal v) {
    return projeter_plan(u, profondeur(v));
}

qreal Perspective::echelle(qreal v) {
    return cadrage().focale * L / std::max(qreal(1.0), denominateur(profondeur(v)));
}

qreal Perspective::profondeur_carte(qreal largeur) {
    // Une carte mesure 1,55 fois sa largeur dans le plan de la table ; il reste a
    // convertir cette longueur en unites de `v`, qui couvrent le diametre.
    return largeur * 1.55 / (2.0 * RAYON);
}

QPolygonF Perspective::quad_carte(qreal u, qreal v, qreal largeur) {
    const qreal v_loin = v - profondeur_carte(largeur);
    const qreal demi = largeur / 2.0;
    QPolygonF q;
    q << projeter(u - demi, v_loin)   // haut-gauche
      << projeter(u + demi, v_loin)   // haut-droit
      << projeter(u + demi, v)        // bas-droit
      << projeter(u - demi, v);       // bas-gauche
    return q;
}

qreal Perspective::demi_largeur(qreal v) {
    // La table est ronde : sa demi-largeur a une profondeur donnee est celle du
    // cercle, pas celle de l'ovale que l'on voit a l'ecran.
    const qreal y = profondeur(v);
    const qreal reste = RAYON * RAYON - y * y;
    return reste <= 0 ? 0 : qSqrt(reste);
}

QRectF Perspective::tapis() {
    // La projection d'un cercle est une ellipse, mais son centre n'est pas la
    // projection du centre du cercle : on en prend donc les extremes reels.
    qreal x0 = 1e9, x1 = -1e9, y0 = 1e9, y1 = -1e9;
    for (int i = 0; i < 720; i++) {
        const qreal a = 2 * M_PI * i / 720.0;
        const QPointF p = projeter_plan(RAYON * qCos(a), RAYON * qSin(a));
        x0 = std::min(x0, p.x()); x1 = std::max(x1, p.x());
        y0 = std::min(y0, p.y()); y1 = std::max(y1, p.y());
    }
    return QRectF(x0, y0, x1 - x0, y1 - y0);
}
