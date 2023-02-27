#ifndef PIZZERIA_H
#define PIZZERIA_H

#include "Batiment.h"

class Pizzeria : public Batiment {
    public:
        //*** Constructeurs et destructeur ***//
        Pizzeria();
        ~Pizzeria() override = default;
        Pizzeria(const Pizzeria& pizzeria) = default;

        //*** Methodes ***//
        Batiment* clone() const override {return new Pizzeria(*this);};

        void declencher_effet(unsigned int possesseur, int bonus = 0) const override;
};

#endif 