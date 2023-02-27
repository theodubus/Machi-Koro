#ifndef MACHI_KORO_EDITIONDEJEU_H
#define MACHI_KORO_EDITIONDEJEU_H

#include <map>
#include <vector>

#include "Batiment.h"
#include "Monument.h"

/// Importation des classes monuments
#include "Aeroport.h"
#include "CentreCommercial.h"
#include "FabriqueDuPereNoel.h"
#include "Gare.h"
#include "HotelDeVille.h"
#include "ParcAttraction.h"
#include "Port.h"
#include "TourRadio.h"

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
#include "CaveAVin.h"
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
#include "MgaGameCenter.h"
#include "Stade.h"

/// Importation des controleurs
#include "EditionDeJeu.h"
#include "Pioche.h"
#include "Shop.h"


class EditionDeJeu{
    protected:
        string nom;
        unsigned int nb_joueurs_max;
        unsigned int nb_monuments_win;

        vector<Monument*> monument;
        map<Batiment*, unsigned int> batiment;

        bool est_edition;

    public:
        /// *** Constructeur et destructeur *** ///
        EditionDeJeu(const string& nom);
        ~EditionDeJeu();

        /// *** Getters *** ///
        const string& get_nom() const {return nom;};

        unsigned int get_nb_joueurs_max() const {return nb_joueurs_max;};
        unsigned int get_nb_monuments_win() const {return nb_monuments_win;};

        /// *** Recuperation des monuments et des batiments *** ///
        vector<Monument*> get_monument() const {return monument;};

        map<Batiment*, unsigned int> get_batiment() const {return batiment;};
};
#endif //MACHI_KORO_EDITIONDEJEU_H
