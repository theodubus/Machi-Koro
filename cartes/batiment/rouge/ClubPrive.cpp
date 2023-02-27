#include "ClubPrive.h"
#include "Partie.h"

ClubPrive::ClubPrive() :
            Batiment("ClubPrive",
                     4 ,
                     "Recevez toutes les pieces du joueur qui a lance les des si celui-ci possede au moins trois monuments construits.",
                     "../assets/batiments/Rouge/Club-prive.png",
                     Rouge ,
                     list<unsigned int>{12,13,14},
                     "restaurant"){}

void ClubPrive::declencher_effet(unsigned int possesseur, int bonus) const{
    Partie * partie = Partie::get_instance();
    Joueur* joueur_possesseur = partie->get_tab_joueurs()[possesseur];
    Joueur* joueur_actuel = partie->get_tab_joueurs()[partie->get_joueur_actuel()];

    if (partie->get_joueur_actuel() != possesseur){
        if(joueur_actuel->get_monument_jouables().size()>=3){
            partie->get_vue_partie()->get_vue_infos()->add_info( "Activation de l'effet du Club Prive du joueur \""+ joueur_possesseur->get_nom() + "\"");
            partie->transfert_argent(partie->get_joueur_actuel(), possesseur, joueur_actuel->get_argent());
            joueur_possesseur->set_argent(joueur_possesseur->get_argent() + 1);
        }
    }
    else{
        throw gameException("On ne peut pas se donner d'argent a soi meme");
    }
}