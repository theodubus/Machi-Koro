#ifndef MACHI_KORO_CENTREIMPOTS_H
#define MACHI_KORO_CENTREIMPOTS_H

#include "Batiment.h"

class CentreImpots : public Batiment {
public:
    CentreImpots();
    ~CentreImpots() override = default;
    CentreImpots(const CentreImpots& centreImpots) = default;
    Batiment* clone() const override {return new CentreImpots(*this);};
    void declencher_effet(unsigned int possesseur, int bonus = 0) const override;
};

#endif //MACHI_KORO_CENTREIMPOTS_H
