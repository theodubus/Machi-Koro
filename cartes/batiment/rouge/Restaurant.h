#ifndef RESTAURANT_H
#define RESTAURANT_H

#include "Batiment.h"

class Restaurant : public Batiment {
public:
    //*** Constructeurs et destructeur ***//
    Restaurant();
    ~Restaurant() override = default;
    Restaurant(const Restaurant& restaurant) = default;

    //*** Methodes ***//
    Batiment* clone() const override {return new Restaurant(*this);};

    void declencher_effet(unsigned int possesseur, int bonus = 0) const override;
};

#endif 
