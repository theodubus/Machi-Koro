# Stockage dans ce fichier des liens pour le fichier CMakeLists.txt et les imports


## CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.23)
project(src)

set(CMAKE_CXX_STANDARD 14)

include_directories(cartes)
include_directories(cartes/batiment)
include_directories(cartes/batiment/bleu)
include_directories(cartes/batiment/rouge)
include_directories(cartes/batiment/vert)
include_directories(cartes/batiment/violet)
include_directories(cartes/monument)
include_directories(Joueur)
include_directories(controleur)
include_directories(controleur/EditionDeJeu)

add_executable(src
        main.cpp

        Joueur/Joueur.cpp
        Joueur/Joueur.h

        cartes/Carte.cpp
        cartes/Carte.h

        cartes/batiment/Batiment.cpp
        cartes/batiment/Batiment.h

        cartes/monument/Monument.cpp
        cartes/monument/Monument.h

        cartes/monument/Aeroport.cpp
        cartes/monument/Aeroport.h
        cartes/monument/CentreCommercial.cpp
        cartes/monument/CentreCommercial.h
        cartes/monument/Gare.cpp
        cartes/monument/Gare.h
        cartes/monument/ParcAttraction.cpp
        cartes/monument/ParcAttraction.h
        cartes/monument/Port.cpp
        cartes/monument/Port.h
        cartes/monument/TourRadio.cpp
        cartes/monument/TourRadio.h
        cartes/monument/FabriqueDuPereNoel.cpp
        cartes/monument/FabriqueDuPereNoel.h
        cartes/monument/HotelDeVille.cpp
        cartes/monument/HotelDeVille.h

        cartes/batiment/vert/BanqueDeMinivilles.cpp
        cartes/batiment/vert/BanqueDeMinivilles.h
        cartes/batiment/vert/Boulangerie.cpp
        cartes/batiment/vert/Boulangerie.h
        cartes/batiment/vert/EntrepriseDeDemenagement.cpp
        cartes/batiment/vert/EntrepriseDeDemenagement.h
        cartes/batiment/vert/EntrepriseDeTravauxPublics.cpp
        cartes/batiment/vert/EntrepriseDeTravauxPublics.h
        cartes/batiment/vert/Epicerie.h
        cartes/batiment/vert/Epicerie.cpp
        cartes/batiment/vert/FabriqueDeMeubles.h
        cartes/batiment/vert/FabriqueDeMeubles.cpp
        cartes/batiment/vert/Fleuriste.h
        cartes/batiment/vert/Fleuriste.cpp
        cartes/batiment/vert/Fromagerie.h
        cartes/batiment/vert/Fromagerie.cpp
        cartes/batiment/vert/HalleDeMarche.cpp
        cartes/batiment/vert/HalleDeMarche.h
        cartes/batiment/vert/MarcheDeFruitsEtLegumes.h
        cartes/batiment/vert/MarcheDeFruitsEtLegumes.cpp
        cartes/batiment/vert/MoonsterSoda.h
        cartes/batiment/vert/MoonsterSoda.cpp
        cartes/batiment/vert/Superette.h
        cartes/batiment/vert/Superette.cpp

        cartes/batiment/violet/Arboretum.cpp
        cartes/batiment/violet/Arboretum.h
        cartes/batiment/violet/CentreAffaires.cpp
        cartes/batiment/violet/CentreAffaires.h
        cartes/batiment/violet/CentreImpots.cpp
        cartes/batiment/violet/CentreImpots.h
        cartes/batiment/violet/ChaineTelevision.cpp
        cartes/batiment/violet/ChaineTelevision.h
        cartes/batiment/violet/EntrepriseRenovation.cpp
        cartes/batiment/violet/EntrepriseRenovation.h
        cartes/batiment/violet/MaisonEdition.cpp
        cartes/batiment/violet/MaisonEdition.h
        cartes/batiment/violet/Stade.cpp
        cartes/batiment/violet/Stade.h

        cartes/batiment/rouge/Cafe.cpp
        cartes/batiment/rouge/Cafe.h
        cartes/batiment/rouge/ClubPrive.cpp
        cartes/batiment/rouge/ClubPrive.h
        cartes/batiment/rouge/Pizzeria.cpp
        cartes/batiment/rouge/Pizzeria.h
        cartes/batiment/rouge/Restaurant5Etoiles.cpp
        cartes/batiment/rouge/Restaurant5Etoiles.h
        cartes/batiment/rouge/Restaurant.cpp
        cartes/batiment/rouge/Restaurant.h
        cartes/batiment/rouge/StandDeHamburger.cpp
        cartes/batiment/rouge/StandDeHamburger.h
        cartes/batiment/rouge/SushiBar.cpp
        cartes/batiment/rouge/SushiBar.h

        cartes/batiment/bleu/Chalutier.cpp
        cartes/batiment/bleu/Chalutier.h
        cartes/batiment/bleu/ChampBle.cpp
        cartes/batiment/bleu/ChampBle.h
        cartes/batiment/bleu/ChampFleur.cpp
        cartes/batiment/bleu/ChampFleur.h
        cartes/batiment/bleu/ChampMais.cpp
        cartes/batiment/bleu/ChampMais.h
        cartes/batiment/bleu/Ferme.cpp
        cartes/batiment/bleu/Ferme.h
        cartes/batiment/bleu/Foret.cpp
        cartes/batiment/bleu/Foret.h
        cartes/batiment/bleu/Mine.cpp
        cartes/batiment/bleu/Mine.h
        cartes/batiment/bleu/PetitBateauDePeche.cpp
        cartes/batiment/bleu/PetitBateauDePeche.h
        cartes/batiment/bleu/Verger.cpp
        cartes/batiment/bleu/Verger.h
        cartes/batiment/bleu/Vignoble.cpp
        cartes/batiment/bleu/Vignoble.h

        controleur/EditionDeJeu/EditionDeJeu.cpp
        controleur/EditionDeJeu/EditionDeJeu.h
        controleur/Shop.cpp
        controleur/Shop.h
        controleur/Pioche.cpp
        controleur/Pioche.h
        )
```

## Imports
```cpp
/// Importation des classes monuments
#include "Aeroport.h"
#include "CentreCommercial.h"
#include "FabriqueDuPereNoel.h"
#include "Gare.h"
#include "HotelDeVille.h"
#include "ParcAttraction.h"
#include "Port.h"
#include "TourRadio.h"

/// Importation des batiments verts
#include "BanqueDeMinivilles.h"
#include "Boulangerie.h"
#include "EntrepriseDeDemenagement.h"
#include "EntrepriseDeTravauxPublics.h"
#include "Epicerie.h"
#include "FabriqueDeMeubles.h"
#include "Fleuriste.h"
#include "Fromagerie.h"
#include "HalleDeMarche.h"
#include "MarcheDeFruitsEtLegumes.h"
#include "MoonsterSoda.h"
#include "Superette.h"

/// Importation des batiments bleus
#include "Chalutier.h"
#include "ChampBle.h"
#include "ChampFleur.h"
#include "ChampMais.h"
#include "Ferme.h"
#include "Foret.h"
#include "Mine.h"
#include "PetitBateauDePeche.h"
#include "Verger.h"
#include "Vignoble.h"

/// Importation des batiments rouges
#include "Cafe.h"
#include "ClubPrive.h"
#include "Pizzeria.h"
#include "Restaurant.h"
#include "Restaurant5Etoiles.h"
#include "StandDeHamburger.h"
#include "SushiBar.h"

/// Importation des batiments violets
#include "Arboretum.h"
#include "CentreAffaires.h"
#include "CentreImpots.h"
#include "ChaineTelevision.h"
#include "EntrepriseRenovation.h"
#include "MaisonEdition.h"
#include "Stade.h"
#include "Startup.h"

/// Importation des controleurs
#include "EditionDeJeu.h"
#include "Pioche.h"
#include "Shop.h"

/// Importation des joueurs
)
```