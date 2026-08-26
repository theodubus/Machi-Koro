#ifndef MACHI_KORO_DECOR_H
#define MACHI_KORO_DECOR_H

#include <QColor>
#include <QPixmap>
#include <QSize>

/// Fabrique les images qui n'existent pas dans assets/.
///
/// Le depot ne contient que les 39 cartes, les 8 monuments, six animations de des
/// et un logo. Tout le reste du plateau — le ciel, les montagnes, le tapis, les
/// avatars, les jetons — est dessine ici, en Qt, plutot que telecharge.
///
/// Trois raisons a ce choix : aucune question de licence sur des images tierces,
/// aucun binaire supplementaire a versionner, et un rendu net a n'importe quelle
/// taille puisque tout est vectoriel jusqu'au dernier moment. Les images
/// couteuses sont gardees en cache : le ciel et le tapis sont dessines une fois
/// pour toutes, la scene ayant une taille logique fixe.
namespace Decor {

    /// Couleurs communes a toute l'interface.
    namespace Palette {
        inline QColor ciel_haut()      { return QColor("#7fc9e3"); }
        inline QColor ciel_bas()       { return QColor("#d8eff2"); }
        inline QColor montagne_loin()  { return QColor("#b3d9e2"); }
        inline QColor montagne_pres()  { return QColor("#96c6d3"); }
        inline QColor neige()          { return QColor("#f0f7f9"); }
        inline QColor tapis_centre()   { return QColor("#41b884"); }
        inline QColor tapis_bord()     { return QColor("#1e8a63"); }
        inline QColor tapis_lisere()   { return QColor("#146b4e"); }
        inline QColor bois()           { return QColor("#c9a878"); }
        inline QColor panneau()        { return QColor("#fdfaf3"); }
        inline QColor encre()          { return QColor("#2f3b46"); }
        inline QColor encre_pale()     { return QColor("#7b8a97"); }
        inline QColor or_piece()       { return QColor("#f2b134"); }
        inline QColor or_sombre()      { return QColor("#c98a1f"); }

        /// Couleur d'identite d'un joueur, dans l'ordre de la table.
        QColor joueur(unsigned int indice);
    }

    /// Fond complet : degrade de ciel, montagnes et silhouette de ville.
    const QPixmap& ciel(const QSize& taille);

    /// Le tapis de jeu, une ellipse avec son lisere. Transparent autour.
    const QPixmap& tapis(const QSize& taille);

    /// Avatar abstrait. Quatre silhouettes distinctes pour qu'on les differencie
    /// meme en noir et blanc, chacune dans la couleur de son joueur.
    QPixmap avatar(unsigned int indice, int taille);

    /// Piece d'or, avec son relief.
    const QPixmap& jeton(int taille);

    /// Emplacement vide, en pointilles, la ou une carte peut venir se poser.
    QPixmap emplacement(const QSize& taille);

    /// Ombre portee douce, calculee une fois et collee sous les cartes plutot
    /// que recalculee par un QGraphicsDropShadowEffect a chaque image : l'effet
    /// est refait a chaque repeint et s'effondre au-dela d'une vingtaine
    /// d'elements, ce qui est vite atteint avec quinze piles et quatre villes.
    QPixmap ombre_carte(const QSize& taille, int flou = 12);

    /// Icone d'une famille d'etablissement, dessinee au trait.
    QPixmap icone_famille(int type, int taille, const QColor& couleur);
}

#endif //MACHI_KORO_DECOR_H
