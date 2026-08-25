#include "PetitBateauDePeche.h"
#include "Partie.h"

PetitBateauDePeche::PetitBateauDePeche() :
        Batiment("PetitBateauDePeche",
                 2,
                 "Si vous avez le port, recevez 3 pieces",
                 "../assets/batiments/Bleu/Petit-bateau-peche.png",
                 Bleu,
                 list<unsigned int>{8},
                 type_bat::Bateau){}
///Constructeur de BateauPeche


void PetitBateauDePeche::declencher_effet(const ContexteDeclenchement& ctx) const{

    //creation de l'instance de partie
    Partie * partie = Partie::get_instance();
    Joueur* joueur_actuel = partie->get_tab_joueurs()[ctx.possesseur];

    if(joueur_actuel->monument_construit("Port")){
        partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet du PetitBateauDePeche du joueur \"" + joueur_actuel->get_nom() + "\"");
        unsigned int argent = joueur_actuel->get_argent();
        argent += 3;
        joueur_actuel->set_argent(argent);
    }
}