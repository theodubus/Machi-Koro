#ifndef MACHI_KORO_STYLEJEU_H
#define MACHI_KORO_STYLEJEU_H

#include <QString>

/// Habillage des fenetres qui ne sont pas le plateau.
///
/// Le plateau est une scene graphique : il se dessine lui-meme. Restent les
/// menus de debut de partie et les fenetres de choix qu'ouvrent certaines cartes
/// — le Centre d'affaires, le Stade, l'Entreprise de renovation. Elles sont
/// faites de widgets ordinaires et portaient chacune ses propres couleurs, quand
/// elles en avaient. Une seule feuille de style, posee sur l'application, leur
/// donne l'aspect du plateau sans avoir a les reecrire.
namespace Style {
    /// Feuille de style a poser sur l'application entiere.
    const QString& feuille();
}

#endif //MACHI_KORO_STYLEJEU_H
