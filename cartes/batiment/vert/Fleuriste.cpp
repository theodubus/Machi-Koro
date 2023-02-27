#include "Fleuriste.h"
#include "Partie.h"

Fleuriste::Fleuriste()
        : Batiment("Fleuriste",
                   1,
                   "Recevez 1 piece de la banque pour chaque Champ de fleurs que vous possedez.",
                   "../assets/batiments/Vert/Fleuriste.png",
                   Vert,
                   {6},
                   "commerce") {}

void Fleuriste::declencher_effet(unsigned int possesseur, int bonus) const {
    /// Effet de la FabriqueDeMeubles
    unsigned int j_act_index =  Partie::get_instance()->get_joueur_actuel();
    Partie * partie = Partie::get_instance();
    Joueur* j_actuel = partie->get_tab_joueurs()[j_act_index];

    unsigned int nb_champs_fleur = 0;

    // liste des batiments bleus du joueur
    map<Batiment*, unsigned int> liste_bat_bleu = j_actuel->get_liste_batiment(Bleu);

    // si le joueur possede des champs de fleurs
    map<Batiment*, unsigned int>::iterator it;
    for (it=liste_bat_bleu.begin(); it!=liste_bat_bleu.end(); ++it) {
        if (it->first->get_nom() == "ChampFleur") {
            nb_champs_fleur += it->second;
            break;
        }
    }

    if (nb_champs_fleur > 0) {
        partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet de la carte Fleuriste du joueur \""+ j_actuel->get_nom() + "\"");
    }

    // on donne 1 piece par champ de fleur
    j_actuel->set_argent(j_actuel->get_argent() + nb_champs_fleur * (1 + bonus));

}