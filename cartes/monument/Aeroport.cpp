#include "Aeroport.h"
#include "Partie.h"

Aeroport::Aeroport() :
    Monument("Aeroport",
                30,
                "Si rien n'a ete construit durant le tour, la banque donne 10 au joueur",
                "../assets/monuments/Aeroport-travaux.png",
                "../assets/monuments/Aeroport-active.png") {
    /// Constructeur de Aeroport
}

void Aeroport::declencher_effet(unsigned int possesseur, int bonus) const {
    Partie * partie = Partie::get_instance();
    Joueur * joueur = partie->get_tab_joueurs()[possesseur];
    partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet de l'earoport du joueur \"" + joueur->get_nom() + "\"");

    joueur->set_argent(joueur->get_argent() + 10);
}

