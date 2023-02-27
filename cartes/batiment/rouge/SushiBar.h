#ifndef SUSHI_BAR_H
#define SUSHI_BAR_H

#include "Batiment.h"

class SushiBar : public Batiment {
    public:
        //*** Constructeurs et destructeur ***//
        SushiBar();
        ~SushiBar() override = default;
        SushiBar(const SushiBar& sushiBar) = default;

        //*** Methodes ***//
        Batiment* clone() const override {return new SushiBar(*this);};

        void declencher_effet(unsigned int possesseur, int bonus = 0) const override;
};

#endif 