#ifndef MACHI_KORO_STADE_H
#define MACHI_KORO_STADE_H

#include "Batiment.h"

class Stade : public Batiment {
public:
    Stade();
    ~Stade() override = default;
    Stade(const Stade& stade) = default;
    Batiment* clone() const override {return new Stade(*this);};
    void declencher_effet(unsigned int possesseur, int bonus = 0) const override;
};

#endif //MACHI_KORO_STADE_H
