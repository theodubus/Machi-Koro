cmake_minimum_required(VERSION 3.24)
project(Machi_Koro)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Emplacement de Qt6.
# Par defaut, on utilise le Qt installe sur le systeme (paquets qt6-base-dev et
# qt6-multimedia-dev sous Debian/Ubuntu, Homebrew sous macOS).
# Si Qt est installe ailleurs (installateur officiel, Windows...), indiquez son
# chemin a la configuration, sans modifier ce fichier :
#   cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.4.1/mingw_64/lib/cmake"
#   cmake -S . -B build -DCMAKE_PREFIX_PATH="$HOME/Qt/6.4.1/gcc_64/lib/cmake"

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Multimedia
        REQUIRED)

include_directories(cartes)
include_directories(cartes/batiment)
include_directories(cartes/batiment/bleu)
include_directories(cartes/batiment/rouge)
include_directories(cartes/batiment/vert)
include_directories(cartes/batiment/violet)
include_directories(cartes/monument)
include_directories(controleur)
include_directories(controleur/EditionDeJeu)
include_directories(exception)
include_directories(joueur)
include_directories(vue)

# Sources du jeu, main.cpp excepte : partagees entre l'executable et l'outil de
# verification des cartes, pour qu'aucun des deux ne puisse deriver de l'autre.
set(MACHI_KORO_SOURCES
        
        cartes/VueCarte.cpp
        cartes/VueCarte.h

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
        cartes/batiment/vert/CaveAVin.cpp
        cartes/batiment/vert/CaveAVin.h

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
        cartes/batiment/violet/Startup.cpp
        cartes/batiment/violet/Startup.h
        cartes/batiment/violet/MgaGameCenter.cpp
        cartes/batiment/violet/MgaGameCenter.h

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
        cartes/batiment/rouge/MoonsterBurger.cpp
        cartes/batiment/rouge/MoonsterBurger.h
        cartes/batiment/rouge/SushiBar.cpp
        cartes/batiment/rouge/SushiBar.h

        cartes/batiment/bleu/PetitBateauDePeche.cpp
        cartes/batiment/bleu/PetitBateauDePeche.h
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
        cartes/batiment/bleu/Verger.cpp
        cartes/batiment/bleu/Verger.h
        cartes/batiment/bleu/Vignoble.cpp
        cartes/batiment/bleu/Vignoble.h

        controleur/EditionDeJeu/EditionDeJeu.cpp
        controleur/EditionDeJeu/EditionDeJeu.h
        controleur/Pioche.cpp
        controleur/Pioche.h
        controleur/Partie.cpp
        controleur/Partie.h
        controleur/Shop.cpp
        controleur/Shop.h
        controleur/VuePartie.cpp
        controleur/VuePartie.h
        controleur/VueInfo.cpp
        controleur/VueInfo.h

        joueur/Joueur.cpp
        joueur/Joueur.h

        exception/gameExeption.h

        vue/Decor.cpp
        vue/Decor.h
        vue/ItemBouton.cpp
        vue/ItemBouton.h
        vue/ItemCarte.cpp
        vue/ItemCarte.h
        vue/Perspective.cpp
        vue/Perspective.h
        vue/ScenePlateau.cpp
        vue/ScenePlateau.h
        vue/StyleJeu.cpp
        vue/StyleJeu.h
        vue/VuePlateau.cpp
        vue/VuePlateau.h)

add_executable(Machi_Koro main.cpp ${MACHI_KORO_SOURCES})

target_link_libraries(Machi_Koro
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Multimedia
        )


# Verification des donnees de cartes.
#
# Compare chaque carte creee par le jeu a la table de reference
# cartes/donnees_cartes.h, transcrite depuis les visuels du dossier assets/ :
# cout, numeros d'activation, couleur, type et image. Cible separee : elle ne
# change rien au jeu et peut tourner en integration continue.
#
#   cmake --build build --target verifier_cartes
#   cd build && ./verifier_cartes
#
# Doit etre lance depuis un sous-repertoire du depot, comme le jeu, pour que les
# chemins ../assets/... se resolvent.
add_executable(verifier_cartes EXCLUDE_FROM_ALL
        outils/verifier_cartes.cpp
        cartes/donnees_cartes.h
        ${MACHI_KORO_SOURCES})

target_link_libraries(verifier_cartes
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Multimedia
        )


if (WIN32)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets Multimedia)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()
