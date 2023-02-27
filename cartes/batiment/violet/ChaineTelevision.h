#ifndef MACHI_KORO_CHAINETELEVISION_H
#define MACHI_KORO_CHAINETELEVISION_H

#include "Batiment.h"

class ChaineTelevision : public Batiment {
public:
    ChaineTelevision();
    ~ChaineTelevision() override = default;
    ChaineTelevision(const ChaineTelevision& chaineTelevision) = default;
    Batiment* clone() const override {return new ChaineTelevision(*this);};
    void declencher_effet(unsigned int possesseur, int bonus = 0) const override;
};

#endif //MACHI_KORO_CHAINETELEVISION_H
