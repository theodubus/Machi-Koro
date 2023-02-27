#ifndef MACHI_KORO_ENTREPRISE_DE_DEMENAGEMENT_H
#define MACHI_KORO_ENTREPRISE_DE_DEMENAGEMENT_H

#include "Batiment.h"

class EntrepriseDeDemenagement : public Batiment {
public:
    EntrepriseDeDemenagement();
    ~EntrepriseDeDemenagement() override = default;
    EntrepriseDeDemenagement(const EntrepriseDeDemenagement& entrepriseDeDemenagement) = default;
    Batiment* clone() const override {return new EntrepriseDeDemenagement(*this);};
    void declencher_effet(unsigned int possesseur, int bonus = 0) const override;
};

#endif //MACHI_KORO_ENTREPRISE_DE_DEMENAGEMENT_H
