#ifndef SRC_ENTREPRISERENOVATION_H
#define SRC_ENTREPRISERENOVATION_H

#include "Batiment.h"

class EntrepriseRenovation : public Batiment {
public:
    EntrepriseRenovation();
    ~EntrepriseRenovation() override = default;
    EntrepriseRenovation(const EntrepriseRenovation& entrepriseRenovation) = default;
    Batiment* clone() const override {return new EntrepriseRenovation(*this);};
    void declencher_effet(const ContexteDeclenchement& ctx) const override;
};

#endif //SRC_ENTREPRISERENOVATION_H
