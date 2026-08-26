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
    // Bonus du Port : le livret parle d'ajouter 2 « au resultat », pas 1 a chaque de.
    // On le garde donc a part, sinon un de peut afficher 7 ou 8.
    unsigned int bonus_des;
    // Le Chalutier fait lancer deux des une seule fois par tour, et ce jet est
    // partage par tous les Chalutiers de tous les joueurs.
    unsigned int de_chalutier;
    unsigned int compteur_tour;
    bool rejouer;
    bool moment_achat;

    VuePartie* vue_partie;
    Shop* shop;
    Pioche* pioche;

    /// Un declenchement de carte, tel qu'on veut le rejouer a l'ecran.
    ///
    /// Un tour se resout d'un bloc : les quatre boucles de couleurs s'enchainent
    /// sans rendre la main, si bien que le joueur voyait le resultat sans jamais
    /// voir *ce qui s'etait passe*. On note donc chaque declenchement au passage —
    /// la carte, chez qui elle s'allume, ce que chacun a gagne ou perdu — et on le
    /// rejoue ensuite carte par carte, avant la phase de construction.
    ///
    /// Une entree dont la carte vaut nullptr est un simple changement d'etape : le
    /// rail avance meme quand une couleur ne declenche rien.
    struct Declenchement {
        const Carte* carte;
        vector<unsigned int> possesseurs;   ///< chez qui la carte s'allume
        vector<int> deltas;                 ///< variation de bourse, par joueur
        ScenePlateau::Phase phase;
        string resume;                      ///< ce que la carte vient de faire
    };
    vector<Declenchement> effets_du_tour;
    ScenePlateau::Phase phase_courante;

    /// Declenche une carte `fois` fois en notant son effet pour le rejeu.
    void declencher(const Carte* carte, const vector<unsigned int>& possesseurs,
                    const ContexteDeclenchement& ctx, unsigned int fois = 1);
    /// Marque un changement d'etape dans le rejeu.
    void marquer_etape(ScenePlateau::Phase p);
    /// Rejoue le declenchement numero `i`, puis programme le suivant.
    void rejouer_effets(size_t i);
    /// Fait voyager les pieces d'un declenchement d'un joueur a l'autre.
    void animer_transferts(const Declenchement& d);
    /// Montre le dernier declenchement enregistre, si la liste a grossi depuis
    /// `depuis`. Les deux monuments qui se declenchent apres la construction —
    /// l'Aeroport et le Parc d'attractions — arrivent une fois le rejeu du tour
    /// termine : sans cela, ils n'apparaitraient nulle part.
    void montrer_dernier_effet(size_t depuis);
    /// Ouvre la phase de construction. Appelee a la fin du rejeu.
    void phase_achat();
    /// Met en phrase ce qu'un declenchement a change dans les bourses.
    string resumer(const vector<int>& deltas) const;

    struct Handler{
        Partie* instance;
        Handler() : instance(nullptr){}
        ~Handler(){delete instance;}
    };
    static Handler handler;

    /// Declenche un monument du joueur courant, s'il le possede et si la
    /// condition est remplie. Rend true si le monument a ete declenche.
    ///
    /// Les huit monuments se declenchent chacun a un moment precis du tour et
    /// sous sa propre condition : la Gare avant que le lance ne fixe le nombre
    /// de des, la Fabrique du Pere Noel des que les des tombent, le Port si le
    /// total atteint 10, l'Aeroport seulement si rien n'a ete achete. Cet ordre
    /// est la structure meme du tour : il reste ecrit noir sur blanc dans
    /// jouer_tour(). Seule la recherche du monument, repetee huit fois a
    /// l'identique, est factorisee ici.
    bool activer_monument(const vector<Monument*>& monuments, const string& nom,
                          bool condition = true);

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
    // Seconde moitie de suite_tour(), appelee par minuterie : verifie la victoire
    // et enchaine sur le tour suivant.
    void terminer_tour();
    bool est_gagnant(unsigned int j) const;


    //********** Gestion des des **********//
    unsigned int get_de_1() const { return de_1; }
    void set_de_1(unsigned int de1) {de_1 = de1;}
    unsigned int get_de_2() const {return de_2;}
    void set_de_2(unsigned int de2) {de_2 = de2;}

    // Resultat servant a activer les etablissements : les des plus l'eventuel
    // bonus du Port. Les valeurs brutes de_1 et de_2 restent dans 1..6, ce dont
    // depend l'affichage des des et la detection des doubles.
    unsigned int get_total_des() const {return de_1 + de_2 + bonus_des;}
    void ajouter_bonus_port() {bonus_des += 2;}

    unsigned int get_de_chalutier() const {return de_chalutier;}


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
    // Rend vrai si la carte a effectivement ete achetee. L'appelant doit s'en
    // servir : un achat refuse laisse au joueur le benefice du « rien construit »
    // (effet de l'Aeroport).
    // La carte, et non la vignette qui la representait : le controleur n'a jamais
    // eu besoin que du modele, et il n'a plus a connaitre les types de la vue.
    bool acheter_carte(const Carte* carte);
    bool acheter_monu(const Carte* carte);//sous fonction appelee dans acheter_carte
    bool acheter_bat(const Carte* carte);//sous fonction appelee dans acheter_carte
    bool transfert_argent(unsigned int indice_joueur1, unsigned int indice_joueur2, unsigned int somme);
    void rejouer_tour();

    vector<Batiment *> get_starter();
};

#endif //MACHI_KORO_PARTIE_H
