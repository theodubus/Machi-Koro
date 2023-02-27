#include "MgaGameCenter.h"
#include "Partie.h"

MgaGameCenter::MgaGameCenter() :
        Batiment("MgaGameCenter",
                 7,
                 "Choississez un de vos etablissements qui ne soit pas de type special et activez son effet. Puis fermez le MGA game center.",
                 "../assets/batiments/Violet/MgaGameCenter.png",
                 Violet,
                 {10},
                 "special") {
    ///Constructeur de MgaGameCenter
}

void MgaGameCenter::declencher_effet(unsigned int possesseur, int bonus) const{
    /// Effet de la classe MGA Game Center
    Partie* instance = Partie::get_instance();
    const vector<Joueur *> &tab_joueurs = instance->get_tab_joueurs();
    Joueur *j_actuel = tab_joueurs[possesseur];

    // On verifie que le joueur possede au moins un batiment non violet
    map<Batiment*, unsigned int> liste_bat_bleu = j_actuel->get_liste_batiment(Bleu);
    map<Batiment*, unsigned int> liste_bat_vert = j_actuel->get_liste_batiment(Vert);
    map<Batiment*, unsigned int> liste_bat_rouge = j_actuel->get_liste_batiment(Rouge);

    if (liste_bat_bleu.empty() && liste_bat_vert.empty() && liste_bat_rouge.empty()) {
        instance->get_vue_partie()->get_vue_infos()->add_info("Vous ne possedez aucun batiment non special !");
        return;
    }
    // Récupération du pointeur sur le batiment (adresse)
    Batiment * bat = j_actuel->possede_batiment("MgaGameCenter");

    if (bat != nullptr) {
        instance->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet du MgaGameCenter du joueur \"" + j_actuel->get_nom() + "\"");

        // Selection du batiment du joueur
        Batiment *batiment = j_actuel->selectionner_batiment();
        // Tant qu'il choisi un batiment violet, on lui redemande
        while (batiment->get_couleur() == Violet) {
            instance->get_vue_partie()->get_vue_infos()->add_info("Vous ne pouvez pas selectionner un batiment violet !");
            batiment = j_actuel->selectionner_batiment();
        }
        // Déclenchement de l'effet du batiment choisi
        batiment->declencher_effet(possesseur, bonus);

        // Fermeture du batiment MGAGameCenter
        j_actuel->fermer_batiment(bat);
    }
}
