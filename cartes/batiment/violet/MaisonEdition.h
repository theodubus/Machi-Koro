#ifndef MACHI_KORO_MAISONEDITION_H
#define MACHI_KORO_MAISONEDITION_H

#include "Batiment.h"

class MaisonEdition : public Batiment {
public:
    MaisonEdition();
    ~MaisonEdition() override = default;
    MaisonEdition(const MaisonEdition& maisonEdition) = default;
    Batiment* clone() const override {return new MaisonEdition(*this);};
    void declencher_effet(const ContexteDeclenchement& ctx) const override;
};

#endif //MACHI_KORO_MAISONEDITION_H
