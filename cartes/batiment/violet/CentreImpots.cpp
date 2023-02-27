#include "CentreImpots.h"
#include "Partie.h"

CentreImpots::CentreImpots():
        Batiment("CentreImpots",
                 4,
                 "Recevez la moitie (arrondie a l'inferieur) des pieces de chaque joueur qui en possede 10 ou plus.",
                 "../assets/batiments/Violet/Centre-des-impots.png",
                 Violet,
                 list<unsigned int>{8,9},
                 "special") {
    ///Constructeur du Centre des Impots
}



void CentreImpots::declencher_effet(unsigned int possesseur, int bonus) const {
    /// Effet du Centre des Impots
    // Variables utiles pour la fonction
    Partie *partie = Partie::get_instance();
    const vector<Joueur *> &tab_joueurs = Partie::get_instance()->get_tab_joueurs();
    Joueur *j_actuel = tab_joueurs[possesseur];
    partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet du Centre des Impots du joueur \"" + j_actuel->get_nom() + "\"");

    // Parcours du tableau de joueurs
    for (int i = 0; i < tab_joueurs.size(); i++) {
        if (j_actuel != tab_joueurs[i] && tab_joueurs[i]->get_argent() >= 10) {
            // Si le joueur d'echange a suffisamment d'argent
            Partie::get_instance()->transfert_argent(i, possesseur, tab_joueurs[i]->get_argent()/2);
        }
    }
}



