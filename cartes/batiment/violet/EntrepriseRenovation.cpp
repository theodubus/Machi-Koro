#include "EntrepriseRenovation.h"
#include "Partie.h"

EntrepriseRenovation::EntrepriseRenovation() :
        Batiment("EntrepriseRenovation",
                 4,
                 "Choississez un de vos etablissements qui ne soit pas de type special. Tous les etablissements identiques de tous les joueurs sont fermes. Puis recevez une piece de la banque pour chaque etablissement ferme.",
                 "../assets/batiments/Violet/Entreprise-de-renovation.png",
                 Violet,
                 list<unsigned int>{8},
                 type_bat::Special) {
    ///Constructeur de EntrepriseRenovation
}

void EntrepriseRenovation::declencher_effet(const ContexteDeclenchement& ctx) const{
    /// Effet de la classe Entreprise de Renovation
    Partie* partie = Partie::get_instance();
    const vector<Joueur *> &tab_joueurs = partie->get_tab_joueurs();
    Joueur *j_actuel = tab_joueurs[ctx.possesseur];

    // On verifie que le joueur possede au moins un batiment non violet
    map<Batiment*, unsigned int> liste_bat_bleu = j_actuel->get_liste_batiment(Bleu);
    map<Batiment*, unsigned int> liste_bat_vert = j_actuel->get_liste_batiment(Vert);
    map<Batiment*, unsigned int> liste_bat_rouge = j_actuel->get_liste_batiment(Rouge);

    if (liste_bat_bleu.empty() && liste_bat_vert.empty() && liste_bat_rouge.empty()) {
        partie->get_vue_partie()->get_vue_infos()->add_info("Vous ne possedez aucun batiment non special !");
        return;
    }

    partie->get_vue_partie()->get_vue_infos()->add_info("Activation de " + get_nom_affiche() + " chez " + j_actuel->get_nom());

    // Selection du batiment du joueur
    Batiment *batiment;
    batiment = j_actuel->selectionner_batiment();

    while (batiment == nullptr || batiment->get_couleur() == Violet) {
        partie->get_vue_partie()->get_vue_infos()->add_info("Vous ne pouvez pas selectionner un batiment violet !");
        batiment = j_actuel->selectionner_batiment();
    }

    int nb_fermes = 0;
    // Fermeture des batiments correspondants
    for (auto joueur : tab_joueurs){
        for (auto& couleur : joueur->get_liste_batiment()) {
            if (couleur.first == Violet)
                continue;
            for (auto bat: couleur.second) {
                if (bat.first->get_nom() == batiment->get_nom()) {
                    for (int nb = 0; nb < bat.second; nb++) {
                        // Fermeture des batiments
                        joueur->fermer_batiment(batiment);
                        nb_fermes++;
                    }
                }
            }
        }
    }
    // Affichage récapitulatif
    partie->get_vue_partie()->get_vue_infos()->add_info(std::to_string(nb_fermes) + " batiment(s) " + batiment->get_nom_affiche() + " ont ete fermes");
    partie->get_vue_partie()->get_vue_infos()->add_info(j_actuel->get_nom() + " va recevoir " + std::to_string(nb_fermes) + " piece(s)");
    j_actuel->set_argent(j_actuel->get_argent() + nb_fermes);
    partie->get_vue_partie()->get_vue_infos()->add_info(j_actuel->get_nom() + " possede maintenant " + std::to_string(j_actuel->get_argent()) + " pieces");
}
