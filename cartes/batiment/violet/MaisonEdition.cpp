#include "MaisonEdition.h"
#include "Partie.h"

MaisonEdition::MaisonEdition():
        Batiment("MaisonEdition",
                 5,
                 "Recevez 1 piece de chaque joueur pour chaque etablissement de type restaurant et commerce qu'il possede.",
                 "../assets/batiments/Violet/Maison-d-edition.png",
                 Violet,
                 list<unsigned int>{7},
                 type_bat::Special) {
    ///Constructeur de Maison d'Edition
}



void MaisonEdition::declencher_effet(const ContexteDeclenchement& ctx) const{
    /// Effet de la maison d'edition
    // Variables utiles
    Partie *partie = Partie::get_instance();
    const vector<Joueur*> tab_joueurs = Partie::get_instance()->get_tab_joueurs();
    Joueur* j_actuel = tab_joueurs[ctx.possesseur];
    partie->get_vue_partie()->get_vue_infos()->add_info("Activation de " + get_nom_affiche() + " chez " + j_actuel->get_nom());

    unsigned int nb_bat;

    // Parcours du tableau de joueurs
    for (int i = 0; i < tab_joueurs.size(); i++){
        if (i != ctx.possesseur){
            Joueur* joueur = tab_joueurs[i];
            // On compte le nombre de batiments concernes
            nb_bat = joueur->count_type(type_bat::Restaurant) + joueur->count_type(type_bat::Commerce);
            // On fait le transfert d'argent
            Partie::get_instance()->transfert_argent(i, ctx.possesseur, nb_bat);
        }
    }
}
