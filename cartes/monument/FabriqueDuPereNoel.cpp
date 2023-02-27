#include "FabriqueDuPereNoel.h"
#include "Partie.h"

FabriqueDuPereNoel::FabriqueDuPereNoel() :
        Monument("FabriqueDuPereNoel",
                 0,
                 "Si le jet de de est casse vous gagnez 3 pieces",
                 "../assets/monuments/FabriqueDuPereNoel.png",
                 "../assets/monuments/FabriqueDuPereNoel.png") {
    /// Constructeur de Centre commercial
}

void FabriqueDuPereNoel::declencher_effet(unsigned int possesseur, int bonus) const {
    Partie *partie = Partie::get_instance();
    Joueur *joueur = partie->get_tab_joueurs()[possesseur];

    partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet de la Fabrique du Pere Noel du joueur \"" + joueur->get_nom() + "\"");

    joueur->set_argent(joueur->get_argent() + 3);
    partie->set_de_1(Partie::lancer_de());
    if (partie->get_de_2() != 0) {
        partie->set_de_2(Partie::lancer_de());
    }
}
