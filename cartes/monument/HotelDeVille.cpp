#include "HotelDeVille.h"
#include "Partie.h"

HotelDeVille::HotelDeVille() :
    Monument("HotelDeVille",
             0,
             "Avant de construire, si vous n'avez pas d'argent vous recevez 1 piece de la banque.",
             "../assets/monuments/HotelDeVille.png",
             "../assets/monuments/HotelDeVille.png") {
    /// Constructeur de HotelDeVille
}

void HotelDeVille::declencher_effet(unsigned int possesseur, int bonus) const {
    Partie * partie = Partie::get_instance();
    Joueur * joueur = partie->get_tab_joueurs()[possesseur];

    if (joueur->get_argent() == 0) {
        partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet de l'Hotel de ville du joueur \"" + joueur->get_nom() + "\"");
        joueur->set_argent(1);
    }
}

