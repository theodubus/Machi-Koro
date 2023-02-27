#include "Verger.h"
#include "Partie.h"

Verger::Verger() :
    Batiment("Verger",
         3,
        "Recevez trois pieces de la banque",
        "../assets/batiments/Bleu/Verger.png",
        Bleu,
        list<unsigned int>{10},
        "champ"){}
///Constructeur de Verger

void Verger::declencher_effet(unsigned int possesseur, int bonus) const {

    //creation de l'instance de partie
    Partie * partie = Partie::get_instance();
    Joueur* joueur_actuel = partie->get_tab_joueurs()[possesseur];

    partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet du Verger du joueur \"" + joueur_actuel->get_nom() + "\"");

    //On augmente de trois l'argent du joueur actuel
    joueur_actuel->set_argent(joueur_actuel->get_argent() + 3);
}