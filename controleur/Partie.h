#ifndef MACHI_KORO_PARTIE_H
#define MACHI_KORO_PARTIE_H

#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <exception>
#include <ctime>
#include <cstdlib>
#include <unistd.h>
#include <random>
#include <QMessageBox>

#include "EditionDeJeu.h"
#include "Joueur.h"
#include "Shop.h"
#include "Pioche.h"
#include "gameExeption.h"
#include "VuePartie.h"

class Partie {
private:
    map<Batiment*, unsigned int> list_batiments;
    vector<Monument*> list_monuments;
    vector<string> tab_nom_edition;
    vector<Joueur*> tab_joueurs;

    unsigned int joueur_actuel;
    unsigned int nb_monuments_win;
    unsigned int de_1;
    unsigned int de_2;
    unsigned int compteur_tour;
    bool rejouer;
    bool moment_achat;

    VuePartie* vue_partie;
    Shop* shop;
    Pioche* pioche;

    struct Handler{
        Partie* instance;
        Handler() : instance(nullptr){}
        ~Handler(){delete instance;}
    };
    static Handler handler;

    //// Constructeur et Destructeur ////
    ~Partie();
    explicit Partie(EditionDeJeu* edition, const map<string, string>& joueurs, const string& shop_type, unsigned int shop_size, const vector<EditionDeJeu *>& extensions = vector<EditionDeJeu *>());


public:
    Partie(Partie const&) = delete;
    void operator=(const Partie&) = delete;
    static Partie* get_instance();
    static Partie* get_instance(const string &edition_name, const list<string> &extensions_names, const map<string, string>& joueurs, const string& shop_type, unsigned int shop_size);


    //********** Jouer une partie **********//
    void jouer_partie();
    void jouer_tour();
    void suite_tour(bool achat_ok);
    bool est_gagnant(unsigned int j) const;


    //********** Gestion des des **********//
    unsigned int get_de_1() const { return de_1; }
    void set_de_1(unsigned int de1) {de_1 = de1;}
    unsigned int get_de_2() const {return de_2;}
    void set_de_2(unsigned int de2) {de_2 = de2;}


    //********** Constructeurs et getters **********//
    unsigned int get_joueur_actuel() const {return joueur_actuel;};
    const vector <Joueur*>& get_tab_joueurs() const {return tab_joueurs;};
    Shop* get_shop() const {return shop;};
    Pioche* get_pioche() const {return pioche;};
    vector<string> get_nom_edition() const {return tab_nom_edition;};
    VuePartie* get_vue_partie() const {return vue_partie;};
    bool get_moment_achat() const {return moment_achat;}
    void set_moment_achat(bool b) {moment_achat = b;}
    unsigned int get_nb_monuments_win() const {return nb_monuments_win;}
    unsigned int get_compteur_tour() const {return compteur_tour;}


    //********** Methodes **********//
    //********** Methodes statiques **********//
    static vector<Batiment*> map_to_vector(const map<Batiment*, unsigned int>& map_batiments);

    // Renvoie l'indice d'un joueur entier valide different de l'indice du joueur actuel
    static unsigned int selectionner_joueur(const vector<Joueur*>& tab_joueurs, unsigned int joueur_actuel);

    // Lance un de
    static unsigned int lancer_de();

    //********** Methodes non statiques **********//
    void ajout_batiment(Batiment *batiment);
    bool acheter_monu_ia();
    bool acheter_bat_ia();
    void acheter_carte_ia();
    void acheter_carte(VueCarte* vue_carte);
    bool acheter_monu(VueCarte* vc);//sous fonction appelee dans acheter_carte
    bool acheter_bat(VueCarte* vc);//sous fonction appelee dans acheter_carte
    bool transfert_argent(unsigned int indice_joueur1, unsigned int indice_joueur2, unsigned int somme);
    void rejouer_tour();

    vector<Batiment *> get_starter();

    void acheter_carte_event(VueCarte* vc);
};

#endif //MACHI_KORO_PARTIE_H
