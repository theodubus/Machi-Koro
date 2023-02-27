#ifndef MACHI_KORO_BOULANGERIE_H
#define MACHI_KORO_BOULANGERIE_H

#include "Batiment.h"

class Boulangerie : public Batiment {
public:
    Boulangerie();
    ~Boulangerie() override = default;
    Boulangerie(const Boulangerie& boulangerie) = default;
    Batiment* clone() const override {return new Boulangerie(*this);};
    void declencher_effet(unsigned int possesseur, int bonus = 0) const override;
};

#endif //MACHI_KORO_BOULANGERIE_H
