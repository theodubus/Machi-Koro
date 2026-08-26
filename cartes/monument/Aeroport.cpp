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

void Aeroport::declencher_effet(const ContexteDeclenchement& ctx) const {
    Partie * partie = Partie::get_instance();
    Joueur * joueur = partie->get_tab_joueurs()[ctx.possesseur];
    partie->get_vue_partie()->get_vue_infos()->add_info("Activation de " + get_nom_affiche() + " chez " + joueur->get_nom());

    joueur->set_argent(joueur->get_argent() + 10);
}

