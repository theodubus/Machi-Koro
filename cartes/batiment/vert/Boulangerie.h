#ifndef MACHI_KORO_BOULANGERIE_H
#define MACHI_KORO_BOULANGERIE_H

#include "Batiment.h"

class Boulangerie : public Batiment {
public:
    Boulangerie();
    ~Boulangerie() override = default;
    Boulangerie(const Boulangerie& boulangerie) = default;
    Batiment* clone() const override {return new Boulangerie(*this);};
    void declencher_effet(const ContexteDeclenchement& ctx) const override;
};

#endif //MACHI_KORO_BOULANGERIE_H
