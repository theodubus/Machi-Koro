#ifndef MACHI_KORO_HALLE_DE_MARCHE_H
#define MACHI_KORO_HALLE_DE_MARCHE_H

#include "Batiment.h"

class HalleDeMarche : public Batiment {
public:
    HalleDeMarche();
    ~HalleDeMarche() override = default;
    HalleDeMarche(const HalleDeMarche& halleDeMarche) = default;
    Batiment* clone() const override {return new HalleDeMarche(*this);};
    void declencher_effet(unsigned int possesseur, int bonus = 0) const override;
};

#endif //MACHI_KORO_HALLE_DE_MARCHE_H
