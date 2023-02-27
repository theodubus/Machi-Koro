#include "Pioche.h"
#include "Partie.h"

Pioche::Pioche(const vector<Batiment*>& batiments) : taille_init(batiments.size()){
    /// Constructeur de la classe Pioche

    for (auto batiment : batiments) {
        contenu.push(batiment);
    }
}

Batiment* Pioche::get_carte() {
    /// Fonction qui retourne la premiere carte de la pioche
    // Si la pioche est non vide
    if (!contenu.empty()){
        // Recupere la tete et on la renvoie
        Batiment *bat = contenu.top();
        contenu.pop();
        // Retourne le batiment
        return bat;
    }
    // Sinon, on renvoie nullptr (erreur)
    else {
        throw gameException("La pioche est vide");
    }
}

