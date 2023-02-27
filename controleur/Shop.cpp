#include "Shop.h"

Shop::Shop(unsigned int nb_tas) : nb_tas_max(nb_tas), nb_tas_reel(0), contenu() {
    ///Constructeur de Shop
}

void Shop::completer_shop(Batiment *new_bat) {
    ///Complete le shop en ajoutant des cartes
    if (nb_tas_reel == nb_tas_max) {
        auto it = contenu.find(new_bat);
        if (it != contenu.end()) {
            it->second++;
        } else {
            throw gameException("Le shop est plein");
        }
    } else {
        auto it = contenu.find(new_bat);
        if (it != contenu.end()) {
            it->second++;
        } else {
            contenu.insert(pair<Batiment*, unsigned int>(new_bat, 1));
            nb_tas_reel++;
        }
    }
}

Shop::~Shop() {
    ///Destructeur de Shop
    contenu.clear();
}

Batiment * Shop::acheter_batiment(Batiment *bat) {
    ///Achete un batiment
    auto it = contenu.find(bat);
    if (it != contenu.end()) {
        if (it->second > 1) {
            it->second--;
            return bat;
        } else {
            contenu.erase(it);
            nb_tas_reel--;
            return bat;
        }
    } else {
        throw gameException("Le batiment n'est pas dans le shop");
    }
}

vector<Batiment*> Shop::get_contenu_v() const {
    ///Retourne le contenu du shop sous forme de vecteur

    vector<Batiment*> contenu_v;

    for (auto & it : contenu) {
        contenu_v.push_back(it.first);
    }

    return contenu_v;
}