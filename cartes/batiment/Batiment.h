#ifndef MACHI_KORO_BATIMENT_H
#define MACHI_KORO_BATIMENT_H

#include "Carte.h"

using namespace std;

enum couleur_bat {Bleu, Rouge, Vert, Violet};

/// Famille d'un batiment : l'icone que porte la carte en haut a gauche.
///
/// Plusieurs cartes comptent les batiments d'une famille donnee — la Fromagerie
/// compte les elevages, la Fabrique de meubles les engrenages, la Halle de
/// marche les restaurants — et le Centre commercial majore les restaurants et
/// les commerces. La famille etait jusqu'ici une chaine litterale recopiee dans
/// chacun des 39 constructeurs, ou une faute de frappe passait inapercue : la
/// Banque de Minivilles etait declaree « commerce » alors qu'elle porte l'icone
/// valise, et la Maison d'edition la comptait donc en trop.
///
/// Enumeration fortement typee : « Restaurant » est deja le nom d'une carte, et
/// une enumeration ordinaire ferait entrer les deux noms en collision.
enum class type_bat {
    Champ,      ///< epi de ble
    Betail,     ///< vache
    Engrenage,  ///< rouage
    Bateau,     ///< bateau
    Restaurant, ///< tasse
    Commerce,   ///< pain
    Entreprise, ///< valise
    Usine,      ///< usine
    Marche,     ///< cageot ; famille propre au projet, aucune carte ne la compte
    Special     ///< etablissement majeur, les cartes violettes
};

/// Nom lisible d'une famille, tel qu'il figure dans la table de reference des
/// cartes. Sert a l'outil de verification et aux messages d'erreur.
const string& nom_type(type_bat t);

class Batiment : public Carte {
    protected:
        couleur_bat couleur;
        list<unsigned int> num_activation;
        type_bat type;
        Batiment(const string& nom, unsigned int prix, const string& description_effet, const string& path_image, couleur_bat coul, const list<unsigned int>& num, type_bat type_batiment);

    public:
        ~Batiment()override=default;
        virtual Batiment* clone() const = 0;
        bool est_monument() const override {return false;};
        couleur_bat get_couleur() const {return couleur;};

        /// Le Centre commercial majore d'une piece chaque etablissement portant
        /// l'icone tasse ou l'icone pain. C'est a la carte de dire si elle en fait
        /// partie : le controleur n'a pas a connaitre la liste des familles
        /// concernees, et une carte qui ferait exception peut redefinir ce choix.
        virtual bool beneficie_centre_commercial() const {
            return type == type_bat::Restaurant || type == type_bat::Commerce;
        };

        /// Appele juste apres l'achat de la carte, une fois son prix paye.
        ///
        /// La plupart des batiments n'ont rien a faire a ce moment. La Banque de
        /// Minivilles, elle, verse 5 pieces a l'acheteur : le controleur codait cet
        /// effet en comparant le nom de la carte, a deux endroits (achat par une IA
        /// et achat par un joueur), avec le risque habituel de n'en corriger qu'un.
        virtual void a_l_achat(unsigned int) const {};

        const list<unsigned int>& get_num_activation() const {return num_activation;};
        type_bat get_type() const {return type;};
};
#endif //MACHI_KORO_BATIMENT_H
