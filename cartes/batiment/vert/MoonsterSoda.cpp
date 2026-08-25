#include "MoonsterSoda.h"
#include "Partie.h"

MoonsterSoda::MoonsterSoda()
        : Batiment("MoonsterSoda",
                   5,
                   "Recevez 1 piece de la banque pour chaque etablissement de type restaurant que possedent tous les joueurs.",
                   "../assets/batiments/Vert/MoonsterSoda.png",
                   Vert,
                   {11},
                   type_bat::Usine) {}

void MoonsterSoda::declencher_effet(const ContexteDeclenchement& ctx) const {
    /// Effet du MoonsterSoda
    unsigned int index_possesseur =  ctx.possesseur;
    Partie * partie = Partie::get_instance();
    Joueur* j_actuel = partie->get_tab_joueurs()[index_possesseur];
    vector<Joueur*> tab_joueurs = partie->get_tab_joueurs();

    // On compte le nombre d'etablissements de type restaurant de tous les joueurs
    unsigned int nb_restaurants = 0;
    for (Joueur* j : tab_joueurs) {
        nb_restaurants += j->count_type(type_bat::Restaurant);
    }

    if (nb_restaurants > 0) {
        partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet de la carte Moonster Soda du joueur \""+ j_actuel->get_nom() + "\"");
    }

    // On donne 1 piece par restaurant
    j_actuel->set_argent(j_actuel->get_argent() + nb_restaurants * (1 + ctx.supplement));
}