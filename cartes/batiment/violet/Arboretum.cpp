#include "Arboretum.h"
#include "Partie.h"

Arboretum::Arboretum():
        Batiment("Arboretum",
                 3,
                 "Rassemblez les pieces de tous les joueurs puis redistribuez les de maniere egale entre tous. La banque completera s'il manque des pieces pour une redistribution egale.",
                 "../assets/batiments/Violet/Arboretum.png",
                 Violet,
                 list<unsigned int>{11, 12, 13},
                 type_bat::Special) {
    ///Constructeur de l'Arboretum
}



void Arboretum::declencher_effet(const ContexteDeclenchement& ctx) const{
    /// Effet de l'arboretum
    Partie *partie = Partie::get_instance();
    unsigned int somme_totale = 0;
    vector<Joueur*> tab_joueurs = partie->get_tab_joueurs();
    partie->get_vue_partie()->get_vue_infos()->add_info("Activation de " + get_nom_affiche() + " chez " + tab_joueurs[ctx.possesseur]->get_nom());

    // Calcul de l'argent total
    for (auto joueur : tab_joueurs){
        somme_totale += joueur->get_argent();
    }
    // On arrondit vers le HAUT, pas vers le bas : « la banque completera s'il
    // manque des pieces pour une redistribution egale ». Le complement vient donc
    // de la banque, et chaque joueur repart avec la meme somme.
    while (somme_totale%tab_joueurs.size() != 0){
        somme_totale++;
    }

    // Argent a repartir
    unsigned int montant_par_joueur = somme_totale / tab_joueurs.size();
    // Repartition
    for (auto joueur : tab_joueurs){
        joueur->set_argent(montant_par_joueur);
    }
    partie->get_vue_partie()->get_vue_infos()->add_info("Tous les joueurs ont maintenant " + std::to_string(montant_par_joueur) + " credits !");
}