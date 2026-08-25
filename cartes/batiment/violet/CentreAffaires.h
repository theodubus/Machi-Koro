#ifndef MACHI_KORO_CENTREAFFAIRES_H
#define MACHI_KORO_CENTREAFFAIRES_H

#include "Batiment.h"

class CentreAffaires : public Batiment {
public:
    CentreAffaires();
    ~CentreAffaires() override = default;
    CentreAffaires(const CentreAffaires& centreAffaires) = default;
    Batiment* clone() const override {return new CentreAffaires(*this);};
    void declencher_effet(const ContexteDeclenchement& ctx) const override;
};

#endif //MACHI_KORO_CENTREAFFAIRES_H
