#include "SushiBar.h"
#include "Partie.h"

SushiBar::SushiBar() :
            Batiment("SushiBar",
                     2 ,
                     "Si vous avez le port, recevez 3 pieces du joueur qui a lance les des.",
                     "../assets/batiments/Rouge/Sushi-bar.png",
                     Rouge ,
                     list<unsigned int>{1},
                     type_bat::Restaurant){}

void SushiBar::declencher_effet(const ContexteDeclenchement& ctx) const{
    Partie * partie = Partie::get_instance();
    Joueur* joueur_possesseur = partie->get_tab_joueurs()[ctx.possesseur];
    //Trouver un joueur qui a cette carte
    if(ctx.joueur_actuel != ctx.possesseur){
        if (partie->get_tab_joueurs()[ctx.possesseur]->monument_construit("Port")){
            partie->get_vue_partie()->get_vue_infos()->add_info("Activation de " + get_nom_affiche() + " chez " + joueur_possesseur->get_nom());
            partie->transfert_argent(ctx.joueur_actuel, ctx.possesseur, 3 + ctx.supplement) ;
        }
    }
    else{
        throw gameException("On ne peut pas se donner d'argent a soi meme");
    }
}