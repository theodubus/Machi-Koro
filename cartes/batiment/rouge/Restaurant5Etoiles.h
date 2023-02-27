#ifndef RESTAURANT_5_ETOILES_H
#define RESTAURANT_5_ETOILES_H

#include "Batiment.h"

class Restaurant5Etoiles : public Batiment {
    public:
        //*** Constructeurs et destructeur ***//
        Restaurant5Etoiles();
        ~Restaurant5Etoiles() override = default;
        Restaurant5Etoiles(const Restaurant5Etoiles& restaurant5Etoiles) = default;

        //*** Methodes ***//
        Batiment* clone() const override {return new Restaurant5Etoiles(*this);};

        void declencher_effet(unsigned int possesseur, int bonus = 0) const override;
};

#endif 