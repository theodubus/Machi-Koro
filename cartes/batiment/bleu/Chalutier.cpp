#include "Chalutier.h"
#include "Partie.h"

Chalutier::Chalutier() :
        Batiment("Chalutier",
        5,
        "Le joueur dont c'est le tour lance deux des. Si vous avez le Port, recevez de la banque autant de pieces que le total des deux des",
        "../assets/batiments/Bleu/Chalutier.png",
        Bleu,
        list<unsigned int>{12,13,14},
        "bateau"){}

///Constructeur de Chalutier

void Chalutier::declencher_effet(unsigned int possesseur, int bonus) const{

    //creation de l'instance de partie
    Partie * partie = Partie::get_instance();
    Joueur* joueur_actuel = partie->get_tab_joueurs()[possesseur];

    //on verifie si le joueur selectionne possede la carte Port
    if(joueur_actuel->possede_monument("Port")){
        partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet du Chalutier du joueur \"" + joueur_actuel->get_nom() + "\"");
        unsigned int result_des;
        unsigned int argent;
        partie->get_vue_partie()->get_vue_infos()->add_info("Tirage des deux des");
        result_des = Partie::lancer_de() + Partie::lancer_de();
        argent = joueur_actuel->get_argent();
        joueur_actuel->set_argent(argent + result_des);
        partie->get_vue_partie()->get_vue_infos()->add_info("Le joueur \"" + joueur_actuel->get_nom() + "\" a gagne " + to_string(result_des) + " pieces");
    }
}