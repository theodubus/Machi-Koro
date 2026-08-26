#include "Restaurant5Etoiles.h"
#include "Partie.h"

Restaurant5Etoiles::Restaurant5Etoiles() :
            Batiment("Restaurant5Etoiles",
                     3 ,
                     "Recevez 5 pieces du joueur qui a lance les des si celui-ci possede au moins deux monuments construits.",
                     "../assets/batiments/Rouge/Restaurant-5-etoiles.png",
                     Rouge,
                     list<unsigned int>{5},
                     type_bat::Restaurant){}


void Restaurant5Etoiles::declencher_effet(const ContexteDeclenchement& ctx) const{
    Partie * partie = Partie::get_instance();
    Joueur* joueur_possesseur = partie->get_tab_joueurs()[ctx.possesseur];
    Joueur* joueur_actuel = partie->get_tab_joueurs()[ctx.joueur_actuel];

    if (ctx.joueur_actuel != ctx.possesseur){
        if(joueur_actuel->nb_monuments_construits() >= 2){
            partie->get_vue_partie()->get_vue_infos()->add_info("Activation de " + get_nom_affiche() + " chez " + joueur_possesseur->get_nom());
            partie->transfert_argent(ctx.joueur_actuel, ctx.possesseur, 5 + ctx.supplement);
        }
    }
    else{
        throw gameException("On ne peut pas se donner d'argent a soi meme");
    }
}