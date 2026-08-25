#include "Startup.h"
#include "Partie.h"

Startup::Startup() :
        Batiment("Startup",
                 1,
                 "Recevez 1 piece de la banque pour chaque piece placee sur l'ensemble des Startups de tous les joueurs. A la fin de votre tour, placez une piece sur votre Startup.",
                 "../assets/batiments/Violet/Startup.png",
                 Violet,
                 list<unsigned int>{10},
                 type_bat::Special) {
    ///Constructeur de Startup
}

void Startup::declencher_effet(unsigned int possesseur, int bonus) const {
    /// Effet de la Startup, applique tel que la carte l'ecrit :
    /// « Recevez 1 piece de la banque pour chaque piece placee sur l'ensemble des
    ///   Startups de tous les joueurs. A la fin de votre tour, placez une piece sur
    ///   votre Startup. »
    /// Le livret precise que cette derniere piece vient de la banque : elle ne coute
    /// rien au joueur.
    Partie* partie = Partie::get_instance();
    const vector<Joueur*>& tab_joueurs = partie->get_tab_joueurs();
    Joueur* j_actuel = tab_joueurs[possesseur];

    partie->get_vue_partie()->get_vue_infos()->add_info(
            "Activation de l'effet de la Startup du joueur \"" + j_actuel->get_nom() + "\"");

    // On compte les pieces posees sur les Startups de TOUS les joueurs, y compris
    // celles du possesseur.
    unsigned int total_pieces = 0;
    for (const Joueur* joueur : tab_joueurs) {
        total_pieces += joueur->get_jetons("Startup");
    }

    j_actuel->set_argent(j_actuel->get_argent() + total_pieces);
    partie->get_vue_partie()->get_vue_infos()->add_info(
            "Le joueur \"" + j_actuel->get_nom() + "\" recoit " + to_string(total_pieces) +
            " piece(s) : autant que de pieces posees sur l'ensemble des Startups");

    // La piece se pose « a la fin de votre tour » : on la programme, Partie la posera
    // au moment voulu.
    j_actuel->programmer_jeton("Startup");
    partie->get_vue_partie()->get_vue_infos()->add_info(
            "Une piece sera posee sur la Startup de \"" + j_actuel->get_nom() + "\" a la fin du tour");
}
