#include "Perspective.h"

#include <QtMath>
#include <algorithm>

namespace {
    // --- La table, dans son propre plan ---
    //
    // Elle n'est pas ronde : elle est **plus large que profonde**, comme une
    // vraie table de jeu. Vue de haut elle donnerait un ovale couche ; c'est ce
    // qui permet de la regarder en plongee sans qu'elle occupe tout l'ecran en
    // hauteur, et donc de garder aux cartes leurs proportions.
    const qreal RAYON_X = 636;
    const qreal RAYON_Y = 404;

    // --- L'inclinaison de la camera, le seul reglage qui compte ---
    //
    // C'est le rapport HAUTEUR / RECUL qui decide de tout. A 1200 / 900 la camera
    // regarde la table a cinquante-trois degres : assez haut pour qu'une carte
    // posee a plat garde plus de quatre cinquiemes de sa proportion et reste donc
    // lisible, assez bas pour qu'on voie une table devant soi et non un plan.
    //
    // Une carte couchee est ecrasee d'exactement le sinus de cet angle : baisser
    // la camera la rend illisible bien avant que la table ne paraisse plate.
    const qreal HAUTEUR_CAMERA = 1200;
    const qreal RECUL          =  900;

    // --- Ou et de quelle taille la table se pose a l'ecran ---
    //
    // La table est **plus grande que le cadre** : son bord proche tombe sous le
    // bas de l'ecran. C'est ce que fait le jeu de reference, et ce n'est pas une
    // coquetterie — un ovale se pince a ses deux pointes, et la pointe proche est
    // justement la ou le joueur pose ses propres cartes. En laissant cette pointe
    // sortir du cadre, la bande visible du bord proche reste large, et une carte y
    // gagne un huitieme de taille a l'ecran sans rien couter en profondeur.
    const qreal CX             = 800;
    const qreal LARGEUR_TAPIS  = 1600;   ///< largeur voulue du tapis projete
    const qreal CENTRE_TAPIS_Y = 548;    ///< milieu vertical du tapis projete

    /// Ce que le cadre laisse voir, en pixels. Sert a retenir les cartes dans
    /// l'ecran la ou le feutre, lui, deborde.
    const qreal LARGEUR_ECRAN  = 1600;

    const qreal L2 = HAUTEUR_CAMERA * HAUTEUR_CAMERA + RECUL * RECUL;
    const qreal L  = qSqrt(L2);

    /// Profondeur dans le plan : +RAYON_Y au fond, -RAYON_Y au bord proche.
    qreal profondeur(qreal v) { return RAYON_Y * (1.0 - 2.0 * std::clamp(v, -2.0, 2.0)); }

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
                const qreal x = RAYON_X * qCos(a), y = RAYON_Y * qSin(a);
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
    // convertir cette longueur en unites de `v`, qui couvrent la profondeur.
    return largeur * 1.55 / (2.0 * RAYON_Y);
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

qreal Perspective::place_ecran(qreal v, qreal marge) {
    // Combien d'unites de table tiennent dans le cadre a cette profondeur. Le
    // feutre depasse maintenant de l'ecran : s'en tenir a `demi_largeur` ferait
    // sortir les cartes par les cotes.
    return std::max(qreal(0.0), (LARGEUR_ECRAN / 2.0 - marge) / echelle(v));
}

qreal Perspective::demi_largeur(qreal v) {
    // La table est ronde : sa demi-largeur a une profondeur donnee est celle du
    // cercle, pas celle de l'ovale que l'on voit a l'ecran.
    const qreal y = profondeur(v);
    const qreal reste = 1.0 - (y * y) / (RAYON_Y * RAYON_Y);
    return reste <= 0 ? 0 : RAYON_X * qSqrt(reste);
}

QRectF Perspective::tapis() {
    // La projection d'un cercle est une ellipse, mais son centre n'est pas la
    // projection du centre du cercle : on en prend donc les extremes reels.
    qreal x0 = 1e9, x1 = -1e9, y0 = 1e9, y1 = -1e9;
    for (int i = 0; i < 720; i++) {
        const qreal a = 2 * M_PI * i / 720.0;
        const QPointF p = projeter_plan(RAYON_X * qCos(a), RAYON_Y * qSin(a));
        x0 = std::min(x0, p.x()); x1 = std::max(x1, p.x());
        y0 = std::min(y0, p.y()); y1 = std::max(y1, p.y());
    }
    return QRectF(x0, y0, x1 - x0, y1 - y0);
}
