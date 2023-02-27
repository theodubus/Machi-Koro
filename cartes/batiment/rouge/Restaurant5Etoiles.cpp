#include "Restaurant5Etoiles.h"
#include "Partie.h"

Restaurant5Etoiles::Restaurant5Etoiles() :
            Batiment("Restaurant5Etoiles",
                     3 ,
                     "Recevez 5 pieces du joueur qui a lance les des si celui-ci possede au moins deux monuments construits.",
                     "../assets/batiments/Rouge/Restaurant-5-etoiles.png",
                     Rouge,
                     list<unsigned int>{5},
                     "restaurant"){}


void Restaurant5Etoiles::declencher_effet(unsigned int possesseur, int bonus) const{
    Partie * partie = Partie::get_instance();
    Joueur* joueur_possesseur = partie->get_tab_joueurs()[possesseur];
    Joueur* joueur_actuel = partie->get_tab_joueurs()[partie->get_joueur_actuel()];

    if (partie->get_joueur_actuel() != possesseur){
        if(joueur_actuel->get_monument_jouables().size() >= 2){
            partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet du Restaurant 5 Etoiles du joueur \""+ joueur_possesseur->get_nom() + "\"");
            partie->transfert_argent(partie->get_joueur_actuel(), possesseur, 5 + bonus);
        }
    }
    else{
        throw gameException("On ne peut pas se donner d'argent a soi meme");
    }
}