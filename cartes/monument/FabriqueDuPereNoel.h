#ifndef SRC_FABRIQUEDUPERENOEL_H
#define SRC_FABRIQUEDUPERENOEL_H

#include "Monument.h"

class FabriqueDuPereNoel : public Monument {
public :
    //*** Constructeur et destructeur ***//
    FabriqueDuPereNoel();
    ~FabriqueDuPereNoel()override=default;
    FabriqueDuPereNoel(const FabriqueDuPereNoel &fabriqueDuPereNoel) = default;
    FabriqueDuPereNoel* clone() const override {return new FabriqueDuPereNoel(*this);};

    //*** Methodes ***//
    void declencher_effet(unsigned int possesseur, int bonus = 0) const override;

};

#endif //SRC_FABRIQUEDUPERENOEL_H
