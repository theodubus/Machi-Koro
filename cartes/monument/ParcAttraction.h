#ifndef MACHI_KORO_PARCATTRACTION_H
#define MACHI_KORO_PARCATTRACTION_H

#include "Monument.h"

class ParcAttraction : public Monument {
public:
    //*** Constructeur et destructeur ***//
    ParcAttraction();
    ~ParcAttraction()override=default;
    ParcAttraction(const ParcAttraction &parcAttraction) = default;
    ParcAttraction* clone() const override {return new ParcAttraction(*this);};

    //*** Methodes ***//
    void declencher_effet(const ContexteDeclenchement& ctx) const override;
};


#endif //MACHI_KORO_PARCATTRACTION_H
