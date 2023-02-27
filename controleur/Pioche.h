#ifndef MACHI_KORO_PIOCHE_H
#define MACHI_KORO_PIOCHE_H

#include <stack>

#include "Batiment.h"
#include "gameExeption.h"

class Pioche {
    friend class Batiment;

    private:
        std::stack<Batiment *> contenu;
        unsigned int taille_init;

    public:
        explicit Pioche(const vector<Batiment*>& batiments);
        ~Pioche() = default;
        bool est_vide() {return contenu.empty();};
        Batiment* get_carte(); // retourne la carte du dessus de la pioche
        unsigned int get_taille() const {return contenu.size();};
        unsigned int get_taille_init() const {return taille_init;};
};

#endif //MACHI_KORO_PIOCHE_H
