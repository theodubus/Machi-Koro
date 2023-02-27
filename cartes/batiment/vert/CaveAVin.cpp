#include "CaveAVin.h"
#include "Partie.h"

CaveAVin::CaveAVin()
        : Batiment("CaveAVin",
                   3,
                   "Recevez 6 pieces de la banque pour chaque Vignoble que vous possedez. Puis fermez cette cave a vin.",
                   "../assets/batiments/Vert/CaveAVin.png",
                   Vert,
                   {9},
                   "usine") {}

void CaveAVin::declencher_effet(unsigned int possesseur, int bonus) const {
    /// Effet de la CaveAVin

    unsigned int j_act_index =  Partie::get_instance()->get_joueur_actuel();
    Partie * partie = Partie::get_instance();
    Joueur* j_actuel = partie->get_tab_joueurs()[j_act_index];

    unsigned int nb_vignoble = 0;

    // liste des batiments bleus du joueur
    map<Batiment*, unsigned int> liste_bat_bleu = j_actuel->get_liste_batiment(Bleu);

    map<Batiment*, unsigned int>::iterator it;
    for (it=liste_bat_bleu.begin(); it!=liste_bat_bleu.end(); ++it) {
        if (it->first->get_nom() == "Vignoble") {
            nb_vignoble += it->second;
            break;
        }
    }



    Batiment * bat = j_actuel->possede_batiment("CaveAVin");

    if (bat != nullptr) {
            if (nb_vignoble > 0) {
                partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet de la carte Cave a vin du joueur \""+ j_actuel->get_nom() + "\"");
            }

        j_actuel->set_argent(j_actuel->get_argent() + 6 * nb_vignoble * (1 + bonus));

        j_actuel->fermer_batiment(bat);
    }

}
