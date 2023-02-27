#include "MarcheDeFruitsEtLegumes.h"
#include "Partie.h"

MarcheDeFruitsEtLegumes::MarcheDeFruitsEtLegumes()
        : Batiment("MarcheDeFruitsEtLegumes",
                   2,
                   "Recevez 2 pieces de la banque pour chaque etablissement de type champ que vous possedez.",
                   "../assets/batiments/Vert/MarcheDeFruitsEtLegumes.png",
                   Vert,
                   {11, 12},
                   "marche") {}

void MarcheDeFruitsEtLegumes::declencher_effet(unsigned int possesseur, int bonus) const{
    /// Effet du MarcheDeFruitsEtLegumes
    unsigned int j_act_index =  Partie::get_instance()->get_joueur_actuel();
    Partie * partie = Partie::get_instance();
    Joueur* j_actuel = partie->get_tab_joueurs()[j_act_index];

    // On compte le nombre d'etablissements de type champ
    unsigned int nb_champs = j_actuel->count_type("champ");

    if (nb_champs > 0) {
        partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet de la carte Marche de fruits et legumes du joueur \""+ j_actuel->get_nom() + "\"");
    }

    // On donne 2 pieces par champ
    j_actuel->set_argent(j_actuel->get_argent() + 2 * nb_champs * (1 + bonus));
}