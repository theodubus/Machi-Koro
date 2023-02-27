#include "Monument.h"

using namespace std;

Monument::Monument(const string& nom, unsigned int prix, const string& description_effet, const string& path_image, const std::string& path_actif) :
    Carte(nom, description_effet, prix, path_image), path_image_actif(path_actif){
    /// Constructeur de Monument

    // Gestion d'erreur nom, description_effet et path_image
    if (nom.empty() || description_effet.empty() || path_image.empty() || path_actif.empty()) {
        throw gameException("Erreur : nom, description_effet, path_image ou path_actif vide");
    }
}