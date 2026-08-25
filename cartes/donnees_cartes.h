#ifndef MACHI_KORO_DONNEES_CARTES_H
#define MACHI_KORO_DONNEES_CARTES_H

#include <string>
#include <vector>

/// Table de reference des cartes du jeu.
///
/// Chaque ligne est la transcription d'une carte physique : ce que la carte dit,
/// independamment de ce que le code en fait. L'executable « verifier_cartes »
/// compare cette table aux constructeurs et echoue a la moindre divergence.
///
/// Deux erreurs avaient echappe a la relecture avant que cette table n'existe :
/// la Banque de Minivilles etait declaree de type « commerce » alors qu'elle
/// porte l'icone valise, et l'Entreprise de renovation s'activait sur un 10 en
/// coutant 1 piece la ou sa carte indique 8 et 4.
///
/// En cas de divergence, c'est la carte qui a raison : on corrige le code, pas
/// cette table — sauf si la transcription elle-meme est fautive, auquel cas il
/// faut regarder le visuel dans assets/ avant de trancher.

namespace donnees_cartes {

    struct Batiment {
        const char* nom;               // get_nom()
        unsigned int prix;             // get_prix()
        std::vector<unsigned int> des; // get_num_activation()
        const char* couleur;           // "Bleu", "Rouge", "Vert" ou "Violet"
        const char* type;              // get_type()
        const char* image;             // nom de fichier dans assets/batiments/<couleur>/
    };

    struct Monument {
        const char* nom;
        unsigned int prix;
        const char* image;             // face « en travaux », ou face unique
    };

    /// 39 batiments, ranges par couleur puis par numero d'activation.
    inline const std::vector<Batiment>& batiments() {
        static const std::vector<Batiment> table = {
            // --- Bleu : rapportent pendant le tour de n'importe quel joueur ---
            {"ChampBle",                   1, {1},          "Bleu",   "champ",      "ChampsBle.png"},
            {"Ferme",                      1, {2},          "Bleu",   "betail",     "Ferme.png"},
            {"ChampMais",                  2, {3, 4},       "Bleu",   "champ",      "ChampMais.png"},
            {"ChampFleur",                 2, {4},          "Bleu",   "champ",      "ChampFleur.png"},
            {"Foret",                      3, {5},          "Bleu",   "engrenage",  "Foret.png"},
            {"Vignoble",                   3, {7},          "Bleu",   "champ",      "Vignobles.png"},
            {"PetitBateauDePeche",         2, {8},          "Bleu",   "bateau",     "Petit-bateau-peche.png"},
            {"Mine",                       6, {9},          "Bleu",   "engrenage",  "Mine.png"},
            {"Verger",                     3, {10},         "Bleu",   "champ",      "Verger.png"},
            {"Chalutier",                  5, {12, 13, 14}, "Bleu",   "bateau",     "Chalutier.png"},

            // --- Rouge : preleves sur le joueur qui a lance les des ---
            {"SushiBar",                   2, {1},          "Rouge",  "restaurant", "Sushi-bar.png"},
            {"Cafe",                       2, {3},          "Rouge",  "restaurant", "Cafe.png"},
            {"Restaurant5Etoiles",         3, {5},          "Rouge",  "restaurant", "Restaurant-5-etoiles.png"},
            {"Pizzeria",                   1, {7},          "Rouge",  "restaurant", "Pizzeria.png"},
            {"MoonsterBurger",             1, {8},          "Rouge",  "restaurant", "Monster-burger.png"},
            {"Restaurant",                 3, {9, 10},      "Rouge",  "restaurant", "Restaurant.png"},
            {"ClubPrive",                  4, {12, 13, 14}, "Rouge",  "restaurant", "Club-prive.png"},

            // --- Vert : rapportent pendant le tour de leur proprietaire ---
            {"Epicerie",                   0, {2},          "Vert",   "commerce",   "Epicerie.png"},
            {"Boulangerie",                1, {2, 3},       "Vert",   "commerce",   "Boulangerie.png"},
            {"EntrepriseDeTravauxPublics", 2, {4},          "Vert",   "entreprise", "EntrepriseDeTravauxPublics.png"},
            {"Superette",                  2, {4},          "Vert",   "commerce",   "Superette.png"},
            {"BanqueDeMinivilles",         0, {5, 6},       "Vert",   "entreprise", "BanqueDeMinivilles.png"},
            {"Fleuriste",                  1, {6},          "Vert",   "commerce",   "Fleuriste.png"},
            {"Fromagerie",                 5, {7},          "Vert",   "usine",      "Fromagerie.png"},
            {"FabriqueDeMeubles",          3, {8},          "Vert",   "usine",      "FabriqueDeMeubles.png"},
            {"CaveAVin",                   3, {9},          "Vert",   "usine",      "CaveAVin.png"},
            {"EntrepriseDeDemenagement",   2, {9, 10},      "Vert",   "entreprise", "EntrepriseDeDemenagement.png"},
            {"MoonsterSoda",               5, {11},         "Vert",   "usine",      "MoonsterSoda.png"},
            {"MarcheDeFruitsEtLegumes",    2, {11, 12},     "Vert",   "marche",     "MarcheDeFruitsEtLegumes.png"},
            {"HalleDeMarche",              2, {12, 13},     "Vert",   "usine",      "HalleDeMarche.png"},

            // --- Violet : etablissements majeurs, un seul exemplaire par joueur ---
            {"CentreAffaires",             8, {6},          "Violet", "special",    "Centre-d-affaires.png"},
            {"Stade",                      6, {6},          "Violet", "special",    "Stade.png"},
            {"ChaineTelevision",           7, {6},          "Violet", "special",    "Chaine-de-television.png"},
            {"MaisonEdition",              5, {7},          "Violet", "special",    "Maison-d-edition.png"},
            {"EntrepriseRenovation",       4, {8},          "Violet", "special",    "Entreprise-de-renovation.png"},
            {"CentreImpots",               4, {8, 9},       "Violet", "special",    "Centre-des-impots.png"},
            {"MgaGameCenter",              7, {10},         "Violet", "special",    "MgaGameCenter.png"},
            {"Startup",                    1, {10},         "Violet", "special",    "Startup.png"},
            {"Arboretum",                  3, {11, 12, 13}, "Violet", "special",    "Arboretum.png"},
        };
        return table;
    }

    /// 8 monuments. L'Hotel de ville et la Fabrique du Pere Noel sont distribues
    /// deja construits : ils n'ont pas de face « en travaux » ni de cout.
    inline const std::vector<Monument>& monuments() {
        static const std::vector<Monument> table = {
            {"Port",               2,  "Port-travaux.png"},
            {"Gare",               4,  "Gare-travaux.png"},
            {"CentreCommercial",   10, "Centre-commercial-travaux.png"},
            {"ParcAttraction",     16, "Parc-attractions-travaux.png"},
            {"TourRadio",          22, "Tour-radio-travaux.png"},
            {"Aeroport",           30, "Aeroport-travaux.png"},
            {"HotelDeVille",       0,  "HotelDeVille.png"},
            {"FabriqueDuPereNoel", 0,  "FabriqueDuPereNoel.png"},
        };
        return table;
    }

} // namespace donnees_cartes

#endif //MACHI_KORO_DONNEES_CARTES_H
