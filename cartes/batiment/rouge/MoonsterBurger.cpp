#include "MoonsterBurger.h"
#include "Partie.h"

MoonsterBurger::MoonsterBurger() :
            Batiment("MoonsterBurger",
                     1 ,
                     "Recevez 1 piece du joueur qui a lance les des.",
                     "../assets/batiments/Rouge/Monster-burger.png",
                     Rouge,
                     list<unsigned int>{8},
                     type_bat::Restaurant){}

void MoonsterBurger::declencher_effet(const ContexteDeclenchement& ctx) const{
    Partie * partie = Partie::get_instance();
    Joueur* joueur_possesseur = partie->get_tab_joueurs()[ctx.possesseur];
    //Trouver un joueur qui a cette carte
    if(ctx.joueur_actuel != ctx.possesseur){
        partie->get_vue_partie()->get_vue_infos()->add_info("Activation de " + get_nom_affiche() + " chez " + joueur_possesseur->get_nom());
        partie->transfert_argent(ctx.joueur_actuel, ctx.possesseur, 1 + ctx.supplement);
    }
    else{
        throw gameException("On ne peut pas se donner d'argent a soi meme");
    }
}