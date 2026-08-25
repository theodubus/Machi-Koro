#ifndef MACHI_KORO_FLEURISTE_H
#define MACHI_KORO_FLEURISTE_H

#include "Batiment.h"

class Fleuriste : public Batiment {
public:
    Fleuriste();
    ~Fleuriste() override = default;
    Fleuriste(const Fleuriste& fleuriste) = default;
    Batiment* clone() const override {return new Fleuriste(*this);};
    void declencher_effet(const ContexteDeclenchement& ctx) const override;
};

#endif //MACHI_KORO_FLEURISTE_H
