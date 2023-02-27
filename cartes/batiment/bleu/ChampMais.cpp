#include "ChampMais.h"
#include "Partie.h"

ChampMais::ChampMais() :
    Batiment("ChampMais",
         2,
        "Recevez une piece de la banque si vous avez moins de deux monuments construits",
        "../assets/batiments/Bleu/ChampMais.png",
        Bleu,
        list<unsigned int>{3,4},
        "champ"){}
///Constructeur de Ferme

void ChampMais::declencher_effet(unsigned int possesseur, int bonus) const{

    //creation de l'instance de partie
    Partie * partie = Partie::get_instance();
    Joueur* joueur_actuel = partie->get_tab_joueurs()[possesseur];
    unsigned int arg = joueur_actuel->get_argent();


    //On augmente de un l'argent du joueur pointe s'il a moins de deux monuments construits
    if((joueur_actuel->get_monument_jouables()).size() < 2) {
        partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet du ChampMais du joueur \"" + joueur_actuel->get_nom() + "\"");
        joueur_actuel->set_argent(arg + 1);
    }
}