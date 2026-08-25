#include "ChampFleur.h"
#include "Partie.h"

ChampFleur::ChampFleur() :
    Batiment("ChampFleur",
         2,
        "Recevez une piece de la banque",
        "../assets/batiments/Bleu/ChampFleur.png",
        Bleu,
        list<unsigned int>{4},
        type_bat::Champ){}
///Constructeur de ChampFleur

void ChampFleur::declencher_effet(const ContexteDeclenchement& ctx) const {

    //creation de l'instance de partie
    Partie * partie = Partie::get_instance();
    Joueur* joueur_actuel = partie->get_tab_joueurs()[ctx.possesseur];

    partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet du ChampFleur du joueur \"" + joueur_actuel->get_nom() + "\"");

    //On augmente de un l'argent du joueur actuel
    unsigned int argent = joueur_actuel->get_argent();
    joueur_actuel->set_argent(argent + 1);
}