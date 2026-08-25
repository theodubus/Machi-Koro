#include "Startup.h"
#include "Partie.h"

Startup::Startup() :
        Batiment("Startup",
                 1,
                 "Recevez 1 piece de la banque pour chaque piece placee sur l'ensemble des Startups de tous les joueurs. A la fin de votre tour, placez une piece sur votre Startup.",
                 "../assets/batiments/Violet/Startup.png",
                 Violet,
                 list<unsigned int>{10},
                 "special") {
    ///Constructeur de Startup
}

void Startup::declencher_effet(unsigned int possesseur, int bonus) const {
    /// Effet de la Startup
    ///
    /// INTERPRETATION A CONFIRMER. La carte porte deux phrases sous l'en-tete
    /// « Pendant votre tour uniquement » : encaisser, puis « a la fin de votre tour,
    /// placez une piece sur votre Startup ». Le livret ne precise que l'origine de
    /// cette piece — « elle vient de la banque » — et non sa frequence.
    ///
    /// Deux lectures sont possibles :
    ///   1. on ne pose une piece que lorsque la carte s'active, sur un 10 ;
    ///   2. on en pose une a la fin de chacun de ses tours, l'activation ne servant
    ///      qu'a encaisser.
    ///
    /// C'est la lecture 1 qui est implementee ici : tout le texte est sous
    /// « Pendant votre tour uniquement », donc sous la condition d'activation. La
    /// lecture 2 rendrait la carte nettement plus forte, la cagnotte grossissant a
    /// chaque tour de chaque joueur. Passer a la lecture 2 demanderait de deplacer
    /// la pose du jeton dans Partie::terminer_tour().
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

    // Puis il pose une piece sur sa propre Startup. Le livret precise qu'elle vient
    // de la banque : elle ne coute rien au joueur.
    j_actuel->poser_jeton("Startup");
    partie->get_vue_partie()->get_vue_infos()->add_info(
            "Une piece est posee sur la Startup de \"" + j_actuel->get_nom() + "\", qui en porte maintenant " +
            to_string(j_actuel->get_jetons("Startup")));
}
