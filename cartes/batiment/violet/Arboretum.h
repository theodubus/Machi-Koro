#ifndef SRC_ARBORETUM_H
#define SRC_ARBORETUM_H

#include "Batiment.h"

class Arboretum : public Batiment {
public:
    Arboretum();
    ~Arboretum() override = default;
    Arboretum(const Arboretum& arboretum) = default;
    Batiment* clone() const override {return new Arboretum(*this);};
    void declencher_effet(unsigned int possesseur, int bonus = 0) const override;
};


#endif //SRC_ARBORETUM_H
