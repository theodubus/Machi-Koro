#ifndef MACHI_KORO_CHAINETELEVISION_H
#define MACHI_KORO_CHAINETELEVISION_H

#include "Batiment.h"

class ChaineTelevision : public Batiment {
public:
    ChaineTelevision();
    ~ChaineTelevision() override = default;
    ChaineTelevision(const ChaineTelevision& chaineTelevision) = default;
    Batiment* clone() const override {return new ChaineTelevision(*this);};
    void declencher_effet(const ContexteDeclenchement& ctx) const override;
};

#endif //MACHI_KORO_CHAINETELEVISION_H
