#include "EditionDeJeu.h"


EditionDeJeu::EditionDeJeu(const string& nom_edition) : nom(nom_edition) {
    /// Constructeur de EditionDeJeu

    //Gestion des erreurs
    if (nom.empty()){
        throw gameException("Le nom de l'edition ne peut etre vide.");
    }

    if (nom_edition == "Deluxe") {
        /// Constructeur de Deluxe
        nom = "Deluxe";
        est_edition = true;
        nb_joueurs_max = 5;
        nb_monuments_win = 5;

        /// Liste des monuments specifiques a Deluxe
        monument.push_back(new Aeroport());
        monument.push_back(new CentreCommercial());
        monument.push_back(new Gare());
        monument.push_back(new ParcAttraction());
        monument.push_back(new Port());
        monument.push_back(new TourRadio());

        /// Liste des batiments specifiques a Deluxe
        // Batiments violets
        batiment.insert(pair<Batiment*, unsigned int>(new CentreAffaires(), 5));
        batiment.insert(pair<Batiment*, unsigned int>(new CentreImpots(), 5));
        batiment.insert(pair<Batiment*, unsigned int>(new ChaineTelevision(), 5));
        batiment.insert(pair<Batiment*, unsigned int>(new MaisonEdition(), 5));
        batiment.insert(pair<Batiment*, unsigned int>(new Stade(), 5));

        // Batiments verts
        batiment.insert(pair<Batiment*, unsigned int>(new Boulangerie(), 5));
        batiment.insert(pair<Batiment*, unsigned int>(new EntrepriseDeDemenagement(), 5));
        batiment.insert(pair<Batiment*, unsigned int>(new EntrepriseDeTravauxPublics(), 5));
        batiment.insert(pair<Batiment*, unsigned int>(new Epicerie(), 5));
        batiment.insert(pair<Batiment*, unsigned int>(new FabriqueDeMeubles(), 5));
        batiment.insert(pair<Batiment*, unsigned int>(new HalleDeMarche(), 5));
        batiment.insert(pair<Batiment*, unsigned int>(new MarcheDeFruitsEtLegumes(), 5));
        batiment.insert(pair<Batiment*, unsigned int>(new MoonsterSoda(), 5));
        batiment.insert(pair<Batiment*, unsigned int>(new Superette(), 5));

        // Batiments bleus
        batiment.insert(pair<Batiment*, unsigned int>(new Chalutier(), 5));
        batiment.insert(pair<Batiment*, unsigned int>(new ChampBle(), 5));
        batiment.insert(pair<Batiment*, unsigned int>(new Ferme(), 5));
        batiment.insert(pair<Batiment*, unsigned int>(new Foret(), 5));
        batiment.insert(pair<Batiment*, unsigned int>(new Mine(), 5));
        batiment.insert(pair<Batiment*, unsigned int>(new PetitBateauDePeche(), 5));
        batiment.insert(pair<Batiment*, unsigned int>(new Verger(), 5));
        batiment.insert(pair<Batiment*, unsigned int>(new Vignoble, 5));

        // Batiments rouges
        batiment.insert(pair<Batiment*, unsigned int>(new Cafe(), 5));
        batiment.insert(pair<Batiment*, unsigned int>(new ClubPrive(), 5));
        batiment.insert(pair<Batiment*, unsigned int>(new StandDeHamburger(), 5));
        batiment.insert(pair<Batiment*, unsigned int>(new Pizzeria(), 5));
        batiment.insert(pair<Batiment*, unsigned int>(new Restaurant(), 5));
        batiment.insert(pair<Batiment*, unsigned int>(new Restaurant5Etoiles(), 5));
    }
    else if (nom_edition == "Standard") {
        /// Constructeur de Standard
        nom = "Standard";
        est_edition = true;
        nb_joueurs_max = 4;
        nb_monuments_win = 4;

        /// Liste des monuments specifiques a Standard
        monument.push_back(new CentreCommercial());
        monument.push_back(new Gare());
        monument.push_back(new ParcAttraction());
        monument.push_back(new TourRadio());

        /// Liste des batiments specifiques a Standard
        // Batiments violets
        batiment.insert(pair<Batiment*, unsigned int>(new CentreAffaires(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new ChaineTelevision(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new Stade(), 4));

        // Batiments verts
        batiment.insert(pair<Batiment*, unsigned int>(new Boulangerie(), 6));
        batiment.insert(pair<Batiment*, unsigned int>(new Fromagerie(), 6));
        batiment.insert(pair<Batiment*, unsigned int>(new FabriqueDeMeubles(), 6));
        batiment.insert(pair<Batiment*, unsigned int>(new MarcheDeFruitsEtLegumes(), 6));
        batiment.insert(pair<Batiment*, unsigned int>(new Superette(), 6));

        // Batiments bleus
        batiment.insert(pair<Batiment*, unsigned int>(new ChampBle(), 6));
        batiment.insert(pair<Batiment*, unsigned int>(new Ferme(), 6));
        batiment.insert(pair<Batiment*, unsigned int>(new Foret(), 6));
        batiment.insert(pair<Batiment*, unsigned int>(new Mine(), 6));
        batiment.insert(pair<Batiment*, unsigned int>(new Verger(), 6));

        // Batiments rouges
        batiment.insert(pair<Batiment*, unsigned int>(new Cafe(), 6));
        batiment.insert(pair<Batiment*, unsigned int>(new Restaurant(), 6));
    }
    else if (nom_edition == "GreenValley") {
        /// Constructeur de GreenValley
        nom = "GreenValley";
        est_edition = false;
        nb_joueurs_max = 4;
        nb_monuments_win = 4;


        /// Liste des batiments specifiques a GreenValley
        // Batiments violets
        batiment.insert(pair<Batiment*, unsigned int>(new Arboretum(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new EntrepriseRenovation(), 4));

        // Batiments verts
        batiment.insert(pair<Batiment*, unsigned int>(new BanqueDeMinivilles(), 6));
        batiment.insert(pair<Batiment*, unsigned int>(new CaveAVin(), 6));
        batiment.insert(pair<Batiment*, unsigned int>(new EntrepriseDeTravauxPublics(), 6));
        batiment.insert(pair<Batiment*, unsigned int>(new MoonsterSoda(), 6));
        batiment.insert(pair<Batiment*, unsigned int>(new Epicerie(), 6));
        batiment.insert(pair<Batiment*, unsigned int>(new EntrepriseDeDemenagement(), 6));
        batiment.insert(pair<Batiment*, unsigned int>(new Vignoble(), 6));

        // Batiments bleus
        batiment.insert(pair<Batiment*, unsigned int>(new ChampMais(), 6));

        // Batiments rouges
        batiment.insert(pair<Batiment*, unsigned int>(new ClubPrive(), 6));
        batiment.insert(pair<Batiment*, unsigned int>(new Restaurant5Etoiles(), 6));
    }
    else if (nom_edition == "Marina") {

        /// Constructeur de Marina
        nom = "Marina";
        est_edition = false;
        nb_joueurs_max = 5;
        nb_monuments_win = 5;

        /// Constructeur de Marina
        // Liste des monuments

        monument.push_back(new Aeroport());
        monument.push_back(new Port());
        monument.push_back(new HotelDeVille());

        /// Liste des batiments specifiques a Marina
        // Liste des batiments

        // Batiments violets
        batiment.insert(pair<Batiment*, unsigned int>(new ChaineTelevision(), 1));
        batiment.insert(pair<Batiment*, unsigned int>(new CentreAffaires(), 1));
        batiment.insert(pair<Batiment*, unsigned int>(new CentreImpots(), 5));
        batiment.insert(pair<Batiment*, unsigned int>(new MaisonEdition(), 5));
        batiment.insert(pair<Batiment*, unsigned int>(new Stade(), 1));

        // Batiments rouges
        batiment.insert(pair<Batiment*, unsigned int>(new StandDeHamburger(), 6));
        batiment.insert(pair<Batiment*, unsigned int>(new SushiBar(), 6));
        batiment.insert(pair<Batiment*, unsigned int>(new Pizzeria(), 6));

        // Batiments bleus
        batiment.insert(pair<Batiment*, unsigned int>(new PetitBateauDePeche(), 6));
        batiment.insert(pair<Batiment*, unsigned int>(new ChampFleur(), 6));
        batiment.insert(pair<Batiment*, unsigned int>(new Chalutier(), 6));
        batiment.insert(pair<Batiment*, unsigned int>(new ChampBle(), 1));

        // Batiments verts
        batiment.insert(pair<Batiment*, unsigned int>(new Boulangerie(), 1));
        batiment.insert(pair<Batiment*, unsigned int>(new Epicerie(), 6));
        batiment.insert(pair<Batiment*, unsigned int>(new Fleuriste(), 6));
        batiment.insert(pair<Batiment*, unsigned int>(new MoonsterSoda(), 6));
    }
    else if (nom_edition == "Custom") {
        // Custom = Edition personnalisee
        // Contient toutes les cartes de toutes les editions
        
        /// Constructeur de Custom
        nom = "Custom";
        est_edition = true;
        nb_joueurs_max = 6;
        nb_monuments_win = 8;

        /// Liste des monuments specifiques a Custom
        monument.push_back(new Aeroport());
        monument.push_back(new CentreCommercial());
        monument.push_back(new FabriqueDuPereNoel());
        monument.push_back(new Gare());
        monument.push_back(new HotelDeVille());
        monument.push_back(new ParcAttraction());
        monument.push_back(new Port());
        monument.push_back(new TourRadio());

        /// Liste des batiments specifiques a Custom
        // Batiments violets
        batiment.insert(pair<Batiment*, unsigned int>(new Arboretum(), 2));
        batiment.insert(pair<Batiment*, unsigned int>(new CentreAffaires(), 2));
        batiment.insert(pair<Batiment*, unsigned int>(new CentreImpots(), 2));
        batiment.insert(pair<Batiment*, unsigned int>(new ChaineTelevision(), 2));
        batiment.insert(pair<Batiment*, unsigned int>(new EntrepriseRenovation(), 2));
        batiment.insert(pair<Batiment*, unsigned int>(new MaisonEdition(), 2));
        batiment.insert(pair<Batiment*, unsigned int>(new MgaGameCenter(), 2));
        batiment.insert(pair<Batiment*, unsigned int>(new Stade(), 2));

        // Batiments verts
        batiment.insert(pair<Batiment*, unsigned int>(new BanqueDeMinivilles(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new Boulangerie(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new CaveAVin(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new EntrepriseDeDemenagement(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new EntrepriseDeTravauxPublics(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new Epicerie(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new FabriqueDeMeubles(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new Fleuriste(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new Fromagerie(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new HalleDeMarche(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new MarcheDeFruitsEtLegumes(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new MoonsterSoda(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new Superette(), 4));

        // Batiments bleus
        batiment.insert(pair<Batiment*, unsigned int>(new Chalutier(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new ChampBle(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new ChampFleur(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new ChampMais(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new Ferme(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new Foret(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new Mine(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new PetitBateauDePeche(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new Verger(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new Vignoble(), 4));

        // Batiments rouges
        batiment.insert(pair<Batiment*, unsigned int>(new Cafe(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new ClubPrive(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new Pizzeria(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new Restaurant, 4));
        batiment.insert(pair<Batiment*, unsigned int>(new Restaurant5Etoiles(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new StandDeHamburger(), 4));
        batiment.insert(pair<Batiment*, unsigned int>(new SushiBar(), 4));
    }

}

EditionDeJeu::~EditionDeJeu() {
    /// Destructeur d'Edition de Jeu

    // Destruction des monuments
    for (auto& mon : monument) {
        delete mon;
    }
    monument.clear();
    // Destruction des batiments
    for (auto& bat : batiment) {
        delete bat.first;
    }
    batiment.clear();
}