#ifndef MACHI_KORO_SUPERETTE_H
#define MACHI_KORO_SUPERETTE_H

#include "Batiment.h"

class Superette : public Batiment {
public:
    Superette();
    ~Superette() override = default;
    Superette(const Superette& superette) = default;
    Batiment* clone() const override {return new Superette(*this);};
    void declencher_effet(unsigned int possesseur, int bonus = 0) const override;
};

#endif //MACHI_KORO_SUPERETTE_H
