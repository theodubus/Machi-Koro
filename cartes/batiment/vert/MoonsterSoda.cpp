#include "MoonsterSoda.h"
#include "Partie.h"

MoonsterSoda::MoonsterSoda()
        : Batiment("MoonsterSoda",
                   5,
                   "Recevez 1 piece de la banque pour chaque etablissement de type restaurant que possedent tous les joueurs.",
                   "../assets/batiments/Vert/MoonsterSoda.png",
                   Vert,
                   {11},
                   "usine") {}

void MoonsterSoda::declencher_effet(unsigned int possesseur, int bonus) const {
    /// Effet du MoonsterSoda
    unsigned int j_act_index =  Partie::get_instance()->get_joueur_actuel();
    Partie * partie = Partie::get_instance();
    Joueur* j_actuel = partie->get_tab_joueurs()[j_act_index];
    vector<Joueur*> tab_joueurs = partie->get_tab_joueurs();

    // On compte le nombre d'etablissements de type restaurant de tous les joueurs
    unsigned int nb_restaurants = 0;
    for (Joueur* j : tab_joueurs) {
        nb_restaurants += j->count_type("restaurant");
    }

    if (nb_restaurants > 0) {
        partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet de la carte Moonster Soda du joueur \""+ j_actuel->get_nom() + "\"");
    }

    // On donne 1 piece par restaurant
    j_actuel->set_argent(j_actuel->get_argent() + nb_restaurants * (1 + bonus));
}