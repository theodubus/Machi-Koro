#include "BanqueDeMinivilles.h"
#include "Partie.h"


BanqueDeMinivilles::BanqueDeMinivilles()
        : Batiment("BanqueDeMinivilles",
                   0,
                   "Lorsque vous achetez cet etablissement, recevez 5 pieces de la banque. Payez 2 pieces a la banque.",
                   "../assets/batiments/Vert/BanqueDeMinivilles.png",
                   Vert,
                   {5, 6},
                   // La carte porte l'icone valise, comme les deux « Entreprise »,
                   // et non l'icone boite du commerce. Le type conditionne ce que
                   // comptent les autres cartes : typee « commerce », elle etait
                   // ponctionnee a tort par la Maison d'edition.
                   "entreprise") {}

void BanqueDeMinivilles::declencher_effet(unsigned int possesseur, int bonus) const{
    /// Effet de l'BanqueDeMinivilles

    // Le don de 5 pieces est gere lors de l'achat de la carte

    /// TRANSACTION AVEC LA BANQUE
    // Le joueur actuel paye 2 pieces a la banque

    unsigned int j_act_index =  Partie::get_instance()->get_joueur_actuel();


    Partie * partie = Partie::get_instance();
    Joueur* j_actuel = partie->get_tab_joueurs()[j_act_index];
    partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet de la carte Banque de minivilles du joueur \""+ j_actuel->get_nom() + "\"");

    if (j_actuel->get_argent() >= 2) {
        j_actuel->set_argent(j_actuel->get_argent() - 2);
    }
    else {
        j_actuel->set_argent(0);
    }


}