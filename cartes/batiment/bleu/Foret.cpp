#include "Foret.h"
#include "Partie.h"

Foret::Foret() :
    Batiment("Foret",
         3,
        "Recevez une piece de la banque",
        "../assets/batiments/Bleu/Foret.png",
        Bleu,
        list<unsigned int>{5},
        "engrenage"){}
///Constructeur de Foret

void Foret::declencher_effet(unsigned int possesseur, int bonus) const {

    //creation de l'instance de partie
    Partie * partie = Partie::get_instance();
    Joueur* joueur_actuel = partie->get_tab_joueurs()[possesseur];

    partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet de Foret du joueur \"" + joueur_actuel->get_nom() + "\"");

    //On augmente de un l'argent du joueur pointe
    joueur_actuel->set_argent(joueur_actuel->get_argent() + 1);
}