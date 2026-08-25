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
        const string nom;
        unsigned int argent;
        bool est_ia;
        strat_IA strategie;

        vector<Batiment*> liste_batiment_fermes;

        map<Monument*, bool> liste_monument;
        // Jetons poses sur une carte du joueur, indexes par nom de carte.
        // La Startup (Green Valley) accumule ainsi des pieces au fil des tours.
        // Les cartes etant partagees entre les joueurs, ce compteur ne peut pas
        // vivre dans le Batiment : il appartient au joueur.
        map<string, unsigned int> jetons;
        map<string, unsigned int> jetons_en_attente;
        map<couleur_bat, map<Batiment*, unsigned int>> liste_batiment;

    public:
        /*** Constructeurs et destructeur ***/
        Joueur(const string& nom, const vector<Monument *>&list_mon, const vector<Batiment *>&list_bat, unsigned int arg_depart, strat_IA stratIa=none);
        // La classe a des methodes virtuelles : le destructeur doit l'etre aussi.
        virtual ~Joueur();

        /***** Getters *****/
        unsigned int get_argent() const {return argent;};

        bool get_est_ia() const {return est_ia;};

        strat_IA get_strategie() const {return strategie;};

        const string& get_nom() const {return nom;};
        const map<Monument*, bool>& get_liste_monument() const {return liste_monument;};

        /// Ces deux accesseurs rendent une **copie**, et ce n'est pas un oubli.
        ///
        /// Les boucles de resolution des effets iterent dessus tout en declenchant
        /// des cartes qui modifient la ville du joueur : l'Entreprise de
        /// demenagement, verte, retire un batiment du joueur pendant la boucle
        /// verte ; l'Entreprise de renovation et le MGA Game Center en ferment
        /// pendant la boucle violette. Rendre une reference invaliderait
        /// l'iterateur de la boucle en cours. La copie est le prix de cette
        /// surete : ne pas l'« optimiser » sans revoir les boucles.
        map<couleur_bat, map<Batiment*, unsigned int>> get_liste_batiment() const {return liste_batiment;};
        map<Batiment*, unsigned int> get_liste_batiment(couleur_bat couleur) const {
            auto it = liste_batiment.find(couleur);
            return it == liste_batiment.end() ? map<Batiment*, unsigned int>() : it->second;
        };

        vector<unsigned int> get_repartition_argent() const;
        vector<Monument*> get_monument_jouables() const;
        vector<Batiment* > get_liste_batiment_fermes() const {return liste_batiment_fermes;};

        // Nombre de jetons poses sur la carte nommee, 0 si elle n'en porte aucun.
        unsigned int get_jetons(const string& nom_carte) const {
            auto it = jetons.find(nom_carte);
            return it == jetons.end() ? 0 : it->second;
        }
        void poser_jeton(const string& nom_carte) { jetons[nom_carte]++; }

        // Certaines cartes posent leur jeton « a la fin de votre tour » : on
        // l'enregistre a l'activation et Partie::terminer_tour() le pose.
        void programmer_jeton(const string& nom_carte) { jetons_en_attente[nom_carte]++; }
        void poser_jetons_en_attente() {
            for (const auto& j : jetons_en_attente) jetons[j.first] += j.second;
            jetons_en_attente.clear();
        }


        /***** Setters *****/
        void set_argent(unsigned int arg) {argent = arg;};

        /***** Autres methodes *****/
        // L'Hotel de ville (Marina) et la Fabrique du Pere Noel sont distribues deja
        // construits en debut de partie. Le livret est explicite : « L'Hotel de ville
        // est un monument, mais celui-ci est construit des le debut de la partie », et
        // la condition de victoire porte sur les 6 monuments *a construire*. Ils ne
        // comptent donc ni pour la victoire, ni pour les cartes qui denombrent les
        // monuments construits d'un joueur.
        static bool est_monument_de_depart(const string& nom_mon) {
            return nom_mon == "HotelDeVille" || nom_mon == "FabriqueDuPereNoel";
        }

        // Nombre de monuments effectivement construits par le joueur, hors monuments
        // offerts au depart.
        unsigned int nb_monuments_construits() const;

        // Vrai seulement si le monument est construit (face « en jeu »).
        // A ne pas confondre avec possede_monument(), qui retrouve la carte d'un
        // monument qu'il soit construit ou non.
        bool monument_construit(const string& nom_mon) const;

        unsigned int count_type(type_bat type) const;

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
};

#endif //MACHI_KORO_JOUEUR_H
