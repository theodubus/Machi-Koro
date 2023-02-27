#ifndef MACHI_KORO_ENTREPRISE_DE_TRAVAUX_PUBLICS_H
#define MACHI_KORO_ENTREPRISE_DE_TRAVAUX_PUBLICS_H

#include "Batiment.h"

class EntrepriseDeTravauxPublics : public Batiment {
public:
    EntrepriseDeTravauxPublics();
    ~EntrepriseDeTravauxPublics() override = default;
    EntrepriseDeTravauxPublics(const EntrepriseDeTravauxPublics& entrepriseDeTravauxPublics) = default;
    Batiment* clone() const override {return new EntrepriseDeTravauxPublics(*this);};
    void declencher_effet(unsigned int possesseur, int bonus = 0) const override;
};

#endif //MACHI_KORO_ENTREPRISE_DE_TRAVAUX_PUBLICS_H
