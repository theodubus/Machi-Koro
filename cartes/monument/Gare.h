#ifndef MACHI_KORO_GARE_H
#define MACHI_KORO_GARE_H

#include "Monument.h"

class Gare : public Monument {
public :
    //*** Constructeur et destructeur ***//
    Gare();
    ~Gare()override=default;
    Gare(const Gare &gare) = default;
    Gare* clone() const override {return new Gare(*this);};

    //*** Methodes ***//
    void declencher_effet(const ContexteDeclenchement& ctx) const override;

};

#endif //MACHI_KORO_GARE_H
