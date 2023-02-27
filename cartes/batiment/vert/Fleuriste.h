#ifndef MACHI_KORO_FLEURISTE_H
#define MACHI_KORO_FLEURISTE_H

#include "Batiment.h"

class Fleuriste : public Batiment {
public:
    Fleuriste();
    ~Fleuriste() override = default;
    Fleuriste(const Fleuriste& fleuriste) = default;
    Batiment* clone() const override {return new Fleuriste(*this);};
    void declencher_effet(unsigned int possesseur, int bonus = 0) const override;
};

#endif //MACHI_KORO_FLEURISTE_H
