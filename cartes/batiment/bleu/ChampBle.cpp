#include "ChampBle.h"
#include "Partie.h"

ChampBle::ChampBle() :
    Batiment("ChampBle",
        1,
        "Recevez une piece de la banque",
        "../assets/batiments/Bleu/ChampsBle.png",
        Bleu,
        list<unsigned int>{1},
        "champ"){}
///Constructeur de ChampBle

void ChampBle::declencher_effet(unsigned int possesseur, int bonus) const {

    //creation de l'instance de partie
    Partie * partie = Partie::get_instance();
    Joueur* joueur_actuel = partie->get_tab_joueurs()[possesseur];

    partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet du ChampBle du joueur \"" + joueur_actuel->get_nom() + "\"");

    //On augmente de un l'argent du joueur actuel
    joueur_actuel->set_argent(joueur_actuel->get_argent() + 1);
}