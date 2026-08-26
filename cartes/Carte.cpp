#include "Carte.h"

#include <map>

using namespace std;

const string& nom_lisible(const string& nom_interne) {
    /// Table des quarante-sept noms, releves sur les visuels du dossier assets/.
    ///
    /// Elle vit ici, dans le modele, et non dans la vue : le journal de partie et
    /// les messages construits par les cartes elles-memes en ont besoin autant que
    /// le plateau.
    static const map<string, string> noms = {
            {"Aeroport",                   "Aéroport"},
            {"Arboretum",                  "Arboretum"},
            {"BanqueDeMinivilles",         "Banque de Minivilles"},
            {"Boulangerie",                "Boulangerie"},
            {"Cafe",                       "Café"},
            {"CaveAVin",                   "Cave à vin"},
            {"CentreAffaires",             "Centre d'affaires"},
            {"CentreCommercial",           "Centre commercial"},
            {"CentreImpots",               "Centre des impôts"},
            {"ChaineTelevision",           "Chaîne de télévision"},
            {"Chalutier",                  "Chalutier"},
            {"ChampBle",                   "Champs de blé"},
            {"ChampFleur",                 "Champ de fleurs"},
            {"ChampMais",                  "Champ de maïs"},
            {"ClubPrive",                  "Club privé"},
            {"EntrepriseDeDemenagement",   "Entreprise de déménagement"},
            {"EntrepriseDeTravauxPublics", "Entreprise de travaux publics"},
            {"EntrepriseRenovation",       "Entreprise de rénovation"},
            {"Epicerie",                   "Épicerie"},
            {"FabriqueDeMeubles",          "Fabrique de meubles"},
            {"FabriqueDuPereNoel",         "Fabrique du Père Noël"},
            {"Ferme",                      "Ferme"},
            {"Fleuriste",                  "Fleuriste"},
            {"Foret",                      "Forêt"},
            {"Fromagerie",                 "Fromagerie"},
            {"Gare",                       "Gare"},
            {"HalleDeMarche",              "Halle de marché"},
            {"HotelDeVille",               "Hôtel de ville"},
            {"MaisonEdition",              "Maison d'édition"},
            {"MarcheDeFruitsEtLegumes",    "Marché de fruits et légumes"},
            {"MgaGameCenter",              "MGA Game Center"},
            {"Mine",                       "Mine"},
            {"MoonsterBurger",             "Moonster burger"},
            {"MoonsterSoda",               "Moonster soda"},
            {"ParcAttraction",             "Parc d'attractions"},
            {"PetitBateauDePeche",         "Petit bateau de pêche"},
            {"Pizzeria",                   "Pizzeria"},
            {"Port",                       "Port"},
            {"Restaurant",                 "Restaurant"},
            {"Restaurant5Etoiles",         "Restaurant 5 étoiles"},
            {"Stade",                      "Stade"},
            {"Startup",                    "Startup"},
            {"Superette",                  "Supérette"},
            {"SushiBar",                   "Sushi bar"},
            {"TourRadio",                  "Tour radio"},
            {"Verger",                     "Verger"},
            {"Vignoble",                   "Vignoble"},
    };
    auto it = noms.find(nom_interne);
    // Une carte absente de la table rend son identifiant : mieux vaut un nom
    // disgracieux qu'un libelle vide si quelqu'un ajoute une carte sans y penser.
    return it == noms.end() ? nom_interne : it->second;
}

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