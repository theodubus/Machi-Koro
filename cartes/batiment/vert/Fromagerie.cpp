#include "Fromagerie.h"
#include "Partie.h"

Fromagerie::Fromagerie()
        : Batiment("Fromagerie",
                   5,
                   "Recevez 3 pieces de la banque pour chaque etablissement de type betail que vous possedez.",
                   "../assets/batiments/Vert/Fromagerie.png",
                   Vert,
                   {7},
                   type_bat::Usine) {}

void Fromagerie::declencher_effet(const ContexteDeclenchement& ctx) const {
    /// Effet de la Fromagerie
    unsigned int index_possesseur =  ctx.possesseur;
    Partie * partie = Partie::get_instance();
    Joueur* j_actuel = partie->get_tab_joueurs()[index_possesseur];

    // On compte le nombre d'etablissements de type betail
    unsigned int nb_betail = j_actuel->count_type(type_bat::Betail);

    if (nb_betail > 0) {
        partie->get_vue_partie()->get_vue_infos()->add_info("Activation de " + get_nom_affiche() + " chez " + j_actuel->get_nom());
    }

    // On donne 3 pieces par betail
    j_actuel->set_argent(j_actuel->get_argent() + 3 * nb_betail * (1 + ctx.supplement));
}