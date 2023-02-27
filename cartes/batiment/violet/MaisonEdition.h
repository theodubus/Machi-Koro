#ifndef MACHI_KORO_MAISONEDITION_H
#define MACHI_KORO_MAISONEDITION_H

#include "Batiment.h"

class MaisonEdition : public Batiment {
public:
    MaisonEdition();
    ~MaisonEdition() override = default;
    MaisonEdition(const MaisonEdition& maisonEdition) = default;
    Batiment* clone() const override {return new MaisonEdition(*this);};
    void declencher_effet(unsigned int possesseur, int bonus = 0) const override;
};

#endif //MACHI_KORO_MAISONEDITION_H
