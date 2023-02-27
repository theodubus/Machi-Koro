#include "Boulangerie.h"
#include "Partie.h"

Boulangerie::Boulangerie()
        : Batiment("Boulangerie",
                   1,
                   "Recevez 1 piece de la banque.",
                   "../assets/batiments/Vert/Boulangerie.png",
                   Vert,
                   {2, 3},
                   "commerce") {}

void Boulangerie::declencher_effet(unsigned int possesseur, int bonus) const {
    /// Effet de la Boulangerie
    unsigned int j_act_index =  Partie::get_instance()->get_joueur_actuel();
    Partie * partie = Partie::get_instance();
    Joueur* j_actuel = partie->get_tab_joueurs()[j_act_index];
    partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet de la carte Boulangerie du joueur \""+ j_actuel->get_nom() + "\"");


    j_actuel->set_argent(j_actuel->get_argent() + 1 + bonus);
}