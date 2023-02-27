#include "Vignoble.h"
#include "Partie.h"

Vignoble::Vignoble() :
    Batiment("Vignoble",
         3,
        "Recevez trois pieces de la banque",
        "../assets/batiments/Bleu/Vignobles.png",
        Bleu,
        list<unsigned int>{7},
        "champ"){}
///Constructeur de Vignoble

void Vignoble::declencher_effet(unsigned int possesseur, int bonus) const {

    //creation de l'instance de partie
    Partie * partie = Partie::get_instance();
    Joueur* joueur_actuel = partie->get_tab_joueurs()[possesseur];

    partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet du Vignoble du joueur \"" + joueur_actuel->get_nom() + "\"");

    //On augmente de trois l'argent du joueur actuel
    joueur_actuel->set_argent(joueur_actuel->get_argent() + 3);
}