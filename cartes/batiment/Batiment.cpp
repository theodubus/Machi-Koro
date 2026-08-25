#include "Batiment.h"

#include <map>

Batiment::Batiment(const string& nom, unsigned int prix, const string& description_effet, const string& path_image, couleur_bat coul,
                   const list<unsigned int>& num, type_bat type_batiment) : Carte(nom, description_effet,
                                                                          prix, path_image), couleur(coul),
                                                                          num_activation(num), type(type_batiment){
    ///Constructeur de Batiment
}

const string& nom_type(type_bat t) {
    /// Nom lisible d'une famille de batiment.
    static const map<type_bat, string> noms = {
            {type_bat::Champ,      "champ"},
            {type_bat::Betail,     "betail"},
            {type_bat::Engrenage,  "engrenage"},
            {type_bat::Bateau,     "bateau"},
            {type_bat::Restaurant, "restaurant"},
            {type_bat::Commerce,   "commerce"},
            {type_bat::Entreprise, "entreprise"},
            {type_bat::Usine,      "usine"},
            {type_bat::Marche,     "marche"},
            {type_bat::Special,    "special"},
    };
    static const string inconnu = "?";
    auto it = noms.find(t);
    return it == noms.end() ? inconnu : it->second;
}
