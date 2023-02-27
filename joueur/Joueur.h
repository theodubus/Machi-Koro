#ifndef MACHI_KORO_JOUEUR_H
#define MACHI_KORO_JOUEUR_H
#include "VueCarte.h"

#include <map>
#include <vector>
#include <algorithm>

#include "Monument.h"
#include "Batiment.h"
#include "gameExeption.h"

enum strat_IA {aleatoire, agressive, defensif, none};
// Agressive : l'IA veut construire des batiments de couleur rouge
// Defensif : l'IA desire construire des batiments de couleur bleue
// Aleatoire : va construire ou pas un batiment au hasard
// None : n'est pas une IA

class Joueur {
    private:
        /*** Attributs de la classe ***/
        Batiment* selec_bat = nullptr;
        const string nom;
        unsigned int argent;
        bool est_ia;
        strat_IA strategie;

        vector<Batiment*> liste_batiment_fermes;

        map<Monument*, bool> liste_monument;
        map<couleur_bat, map<Batiment*, unsigned int>> liste_batiment;

    public:
        /*** Constructeurs et destructeur ***/
        Joueur(const string& nom, const vector<Monument *>&list_mon, const vector<Batiment *>&list_bat, unsigned int arg_depart, strat_IA stratIa=none);
        ~Joueur();

        /***** Getters *****/
        unsigned int get_argent() const {return argent;};

        bool get_est_ia() const {return est_ia;};

        strat_IA get_strategie() const {return strategie;};

        const string& get_nom() const {return nom;};
        const map<Monument*, bool>& get_liste_monument() const {return liste_monument;};

        map<couleur_bat, map<Batiment*, unsigned int>> get_liste_batiment() const {return liste_batiment;};
        map<Batiment*, unsigned int> get_liste_batiment(couleur_bat couleur) {return liste_batiment[couleur];};

        vector<unsigned int> get_repartition_argent() const;
        vector<Monument*> get_monument_jouables() const;
        vector<Batiment* > get_liste_batiment_fermes() const {return liste_batiment_fermes;};


        /***** Setters *****/
        void set_argent(unsigned int arg) {argent = arg;};

        /***** Autres methodes *****/
        unsigned int count_type(const string& type) const;

        void activer_monument(Monument *mon);
        void desactiver_monument(Monument *mon);
        void ajouter_batiment(Batiment *bat);
        void retirer_batiment(Batiment *bat);

        virtual void fermer_batiment(Batiment *bat);
        virtual void ouvrir_batiment(Batiment *bat);

        Monument* possede_monument(const string& nom_mon) const;

        Batiment* selectionner_batiment() const;
        Monument* selectionner_monument() const;

        Batiment* possede_batiment(const string& nom_bat) const;
public slots:
    void selection_bat(VueCarte *);
};

#endif //MACHI_KORO_JOUEUR_H
