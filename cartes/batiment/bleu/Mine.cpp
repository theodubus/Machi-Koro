#include "Mine.h"
#include "Partie.h"

Mine::Mine() :
    Batiment("Mine",
         6,
        "Recevez cinq pieces de la banque",
        "../assets/batiments/Bleu/Mine.png",
        Bleu,
        list<unsigned int>{9},
        "engrenage"){}
///Constructeur de Mine

void Mine::declencher_effet(unsigned int possesseur, int bonus) const{

    //creation de l'instance de partie
    Partie * partie = Partie::get_instance();
    Joueur* joueur_actuel = partie->get_tab_joueurs()[possesseur];

    partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet de Mine du joueur \"" + joueur_actuel->get_nom() + "\"");

    //On augmente de cinq l'argent du joueur actuel
    joueur_actuel->set_argent(joueur_actuel->get_argent() + 5);
}