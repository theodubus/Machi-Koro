#include "Epicerie.h"
#include "Partie.h"

Epicerie::Epicerie()
        : Batiment("Epicerie",
                   0,
                   "Si vous possedez moins de deux monuments, recevez 2 pieces de la banque.",
                   "../assets/batiments/Vert/Epicerie.png",
                   Vert,
                   {2},
                   type_bat::Commerce) {}

void Epicerie::declencher_effet(const ContexteDeclenchement& ctx) const{
    /// Effet de l'Epicerie
    unsigned int index_possesseur =  ctx.possesseur;
    Partie * partie = Partie::get_instance();
    Joueur* j_actuel = partie->get_tab_joueurs()[index_possesseur];


    if (j_actuel->nb_monuments_construits() < 2){
        partie->get_vue_partie()->get_vue_infos()->add_info("Activation de " + get_nom_affiche() + " chez " + j_actuel->get_nom());
        j_actuel->set_argent(j_actuel->get_argent() + 2 + ctx.supplement);
    }
}