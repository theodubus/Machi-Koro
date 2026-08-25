#ifndef MACHI_KORO_CONTEXTE_DECLENCHEMENT_H
#define MACHI_KORO_CONTEXTE_DECLENCHEMENT_H

/// Ce qu'une carte a besoin de savoir au moment ou son effet se declenche.
///
/// Les 47 cartes recevaient le couple (possesseur, bonus). « bonus » etait un
/// entier nu dont seul le controleur connaissait le sens — le supplement d'une
/// piece du Centre commercial — et la moindre information supplementaire a
/// transmettre aurait oblige a rouvrir les 47 signatures. Les regrouper dans
/// une structure permet d'en ajouter une sans toucher a aucune d'elles.
///
/// Le joueur dont c'est le tour figure ici : les cartes allaient le chercher
/// dans le singleton Partie, alors que c'est une donnee du declenchement.
struct ContexteDeclenchement {
    /// Joueur dont la carte se declenche, et qui en encaisse l'effet.
    /// Pour un batiment bleu c'est n'importe quel joueur, pour un rouge celui
    /// qui est paye, pour un vert ou un violet le joueur dont c'est le tour.
    unsigned int possesseur;

    /// Joueur dont c'est le tour, celui qui a lance les des.
    unsigned int joueur_actuel;

    /// Supplement du Centre commercial : 1 piece par exemplaire active, 0 sinon.
    /// Le controleur le calcule en demandant a la carte si elle en beneficie.
    int supplement = 0;
};

#endif //MACHI_KORO_CONTEXTE_DECLENCHEMENT_H
