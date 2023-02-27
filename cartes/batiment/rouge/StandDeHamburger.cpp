#include "StandDeHamburger.h"
#include "Partie.h"

StandDeHamburger::StandDeHamburger() :
            Batiment("StandDeHamburger",
                     1 ,
                     "Recevez 1 piece du joueur qui a lance les des.",
                     "../assets/batiments/Rouge/Monster-burger.png",
                     Rouge,
                     list<unsigned int>{8},
                     "restaurant"){}

void StandDeHamburger::declencher_effet(unsigned int possesseur, int bonus) const{
    Partie * partie = Partie::get_instance();
    Joueur* joueur_possesseur = partie->get_tab_joueurs()[possesseur];
    //Trouver un joueur qui a cette carte
    if(partie->get_joueur_actuel() != possesseur){
        partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet du cafe du Moonster burger du joueur \""+ joueur_possesseur->get_nom() + "\"");
        partie->transfert_argent(partie->get_joueur_actuel(), possesseur, 1 + bonus);
    }
    else{
        throw gameException("On ne peut pas se donner d'argent a soi meme");
    }
}