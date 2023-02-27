#include "ParcAttraction.h"
#include "Partie.h"

ParcAttraction::ParcAttraction() :
    Monument("ParcAttraction",
             16,
             "Si le jet de de est un double vous pouvez rejouer",
             "../assets/monuments/Parc-attractions-travaux.png",
             "../assets/monuments/Parc-attractions-active.png") {
    /// Constructeur de ParcAttraction
}

void ParcAttraction::declencher_effet(unsigned int possesseur, int bonus) const {
    Joueur *joueur = Partie::get_instance()->get_tab_joueurs()[possesseur];

    if (Partie::get_instance()->get_de_1() == Partie::get_instance()->get_de_2()) {
        Partie::get_instance()->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet du Parc d'attractions du joueur \"" + joueur->get_nom() + "\"");
        Partie::get_instance()->rejouer_tour();
    }
}
