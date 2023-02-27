#ifndef STAND_DE_HAMBURGER_H
#define STAND_DE_HAMBURGER_H

#include "Batiment.h"

class StandDeHamburger : public Batiment {
    public:
        //*** Constructeurs et destructeur ***//
        StandDeHamburger();
        ~StandDeHamburger() override = default;
        StandDeHamburger(const StandDeHamburger& standDeHamburger) = default;

        //*** Methodes ***//
        Batiment* clone() const override {return new StandDeHamburger(*this);};

        void declencher_effet(unsigned int possesseur, int bonus = 0) const override;
};

#endif 