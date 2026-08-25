#include "Chalutier.h"
#include "Partie.h"

Chalutier::Chalutier() :
        Batiment("Chalutier",
        5,
        "Le joueur dont c'est le tour lance deux des. Si vous avez le Port, recevez de la banque autant de pieces que le total des deux des",
        "../assets/batiments/Bleu/Chalutier.png",
        Bleu,
        list<unsigned int>{12,13,14},
        type_bat::Bateau){}

///Constructeur de Chalutier

void Chalutier::declencher_effet(const ContexteDeclenchement& ctx) const{

    //creation de l'instance de partie
    Partie * partie = Partie::get_instance();
    Joueur* joueur_actuel = partie->get_tab_joueurs()[ctx.possesseur];

    //on verifie si le joueur selectionne a bien CONSTRUIT le Port
    if(joueur_actuel->monument_construit("Port")){
        partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet du Chalutier du joueur \"" + joueur_actuel->get_nom() + "\"");
        // Les deux des ne sont lances qu'une fois par tour, par le joueur dont c'est le
        // tour : le meme resultat vaut pour tous les Chalutiers de tous les joueurs.
        unsigned int result_des = partie->get_de_chalutier();
        unsigned int argent = joueur_actuel->get_argent();
        joueur_actuel->set_argent(argent + result_des);
        partie->get_vue_partie()->get_vue_infos()->add_info("Le joueur \"" + joueur_actuel->get_nom() + "\" a gagne " + to_string(result_des) + " pieces");
    }
}