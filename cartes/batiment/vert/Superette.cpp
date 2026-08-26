#include "Superette.h"
#include "Partie.h"

Superette::Superette()
        : Batiment("Superette",
                   2,
                   "Recevez 3 piece de la banque.",
                   "../assets/batiments/Vert/Superette.png",
                   Vert,
                   {4},
                   type_bat::Commerce) {}

void Superette::declencher_effet(const ContexteDeclenchement& ctx) const{
    /// Effet de la Superette
    unsigned int index_possesseur =  ctx.possesseur;
    Partie * partie = Partie::get_instance();
    Joueur* j_actuel = partie->get_tab_joueurs()[index_possesseur];

    partie->get_vue_partie()->get_vue_infos()->add_info("Activation de " + get_nom_affiche() + " chez " + j_actuel->get_nom());

    // On donne 3 pieces
    unsigned int argent = j_actuel->get_argent();
    j_actuel->set_argent(argent + 3 + ctx.supplement);
}