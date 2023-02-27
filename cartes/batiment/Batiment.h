#ifndef MACHI_KORO_BATIMENT_H
#define MACHI_KORO_BATIMENT_H

#include "Carte.h"

using namespace std;

enum couleur_bat {Bleu, Rouge, Vert, Violet};

class Batiment : public Carte {
    protected:
        couleur_bat couleur;
        list<unsigned int> num_activation;
        string type;
        Batiment(const string& nom, unsigned int prix, const string& description_effet, const string& path_image, couleur_bat coul, const list<unsigned int>& num, const string& type_bat);

    public:
        ~Batiment()override=default;
        virtual Batiment* clone() const = 0;
        couleur_bat get_couleur() const {return couleur;};
        const list<unsigned int>& get_num_activation() const {return num_activation;};
        string get_type() const override {return type;};
};
#endif //MACHI_KORO_BATIMENT_H
