#ifndef MACHI_KORO_FROMAGERIE_H
#define MACHI_KORO_FROMAGERIE_H

#include "Batiment.h"

class Fromagerie : public Batiment {
public:
    Fromagerie();
    ~Fromagerie() override = default;
    Fromagerie(const Fromagerie& fromagerie) = default;
    Batiment* clone() const override {return new Fromagerie(*this);};
    void declencher_effet(unsigned int possesseur, int bonus = 0) const override;
};

#endif //MACHI_KORO_FROMAGERIE_H
