#ifndef SRC_ENTREPRISERENOVATION_H
#define SRC_ENTREPRISERENOVATION_H

#include "Batiment.h"

class EntrepriseRenovation : public Batiment {
public:
    EntrepriseRenovation();
    ~EntrepriseRenovation() override = default;
    EntrepriseRenovation(const EntrepriseRenovation& entrepriseRenovation) = default;
    Batiment* clone() const override {return new EntrepriseRenovation(*this);};
    void declencher_effet(unsigned int possesseur, int bonus = 0) const override;
};

#endif //SRC_ENTREPRISERENOVATION_H
