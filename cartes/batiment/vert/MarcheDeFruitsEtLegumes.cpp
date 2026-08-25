#include "MarcheDeFruitsEtLegumes.h"
#include "Partie.h"

MarcheDeFruitsEtLegumes::MarcheDeFruitsEtLegumes()
        : Batiment("MarcheDeFruitsEtLegumes",
                   2,
                   "Recevez 2 pieces de la banque pour chaque etablissement de type champ que vous possedez.",
                   "../assets/batiments/Vert/MarcheDeFruitsEtLegumes.png",
                   Vert,
                   {11, 12},
                   type_bat::Marche) {}

void MarcheDeFruitsEtLegumes::declencher_effet(const ContexteDeclenchement& ctx) const{
    /// Effet du MarcheDeFruitsEtLegumes
    unsigned int index_possesseur =  ctx.possesseur;
    Partie * partie = Partie::get_instance();
    Joueur* j_actuel = partie->get_tab_joueurs()[index_possesseur];

    // On compte le nombre d'etablissements de type champ
    unsigned int nb_champs = j_actuel->count_type(type_bat::Champ);

    if (nb_champs > 0) {
        partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet de la carte Marche de fruits et legumes du joueur \""+ j_actuel->get_nom() + "\"");
    }

    // On donne 2 pieces par champ
    j_actuel->set_argent(j_actuel->get_argent() + 2 * nb_champs * (1 + ctx.supplement));
}