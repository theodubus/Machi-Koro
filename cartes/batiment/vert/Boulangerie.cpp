#include "Boulangerie.h"
#include "Partie.h"

Boulangerie::Boulangerie()
        : Batiment("Boulangerie",
                   1,
                   "Recevez 1 piece de la banque.",
                   "../assets/batiments/Vert/Boulangerie.png",
                   Vert,
                   {2, 3},
                   type_bat::Commerce) {}

void Boulangerie::declencher_effet(const ContexteDeclenchement& ctx) const {
    /// Effet de la Boulangerie
    unsigned int index_possesseur =  ctx.possesseur;
    Partie * partie = Partie::get_instance();
    Joueur* j_actuel = partie->get_tab_joueurs()[index_possesseur];
    partie->get_vue_partie()->get_vue_infos()->add_info("Activation de " + get_nom_affiche() + " chez " + j_actuel->get_nom());


    j_actuel->set_argent(j_actuel->get_argent() + 1 + ctx.supplement);
}