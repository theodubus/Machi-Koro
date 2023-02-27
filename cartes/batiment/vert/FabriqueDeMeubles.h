#ifndef MACHI_KORO_FABRIQUE_DE_MEUBLES_H
#define MACHI_KORO_FABRIQUE_DE_MEUBLES_H

#include "Batiment.h"

class FabriqueDeMeubles : public Batiment {
public:
    FabriqueDeMeubles();
    ~FabriqueDeMeubles() override = default;
    FabriqueDeMeubles(const FabriqueDeMeubles& fabriqueDeMeubles) = default;
    Batiment* clone() const override {return new FabriqueDeMeubles(*this);};
    void declencher_effet(unsigned int possesseur, int bonus = 0) const override;
};

#endif //MACHI_KORO_FABRIQUE_DE_MEUBLES_H
