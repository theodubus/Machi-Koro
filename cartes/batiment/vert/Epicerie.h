#ifndef MACHI_KORO_EPICERIE_H
#define MACHI_KORO_EPICERIE_H

#include "Batiment.h"

class Epicerie : public Batiment {
public:
    Epicerie();
    ~Epicerie() override = default;
    Epicerie(const Epicerie& epicerie) = default;
    Batiment* clone() const override {return new Epicerie(*this);};
    void declencher_effet(unsigned int possesseur, int bonus = 0) const override;

};

#endif //MACHI_KORO_EPICERIE_H
