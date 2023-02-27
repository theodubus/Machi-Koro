#include "Carte.h"

using namespace std;

Carte::Carte(const string& name, const string& effet_description, unsigned int price, const string& path_picture) {
    /// Constructeur de la classe Carte

    // Gestion des erreurs
    if (name.empty()) {
        throw gameException("Le nom de la carte ne peut pas etre vide");
    }

    if (effet_description.empty()) {
        throw gameException("La description de l'effet de la carte ne peut pas etre vide");
    }

    if (path_picture.empty()) {
        throw gameException("Le chemin de l'image de la carte ne peut pas etre vide");
    }

    // Initialisation des attributs
    nom = name;
    description_effet = effet_description;
    prix = price;
    path_image = path_picture;
}

unsigned int Carte::argent_effet(int a) {
    /// Retourne le maximum entre a et 0

    if (a < 0)
        return 0;
    else
        return (unsigned int) a;
}