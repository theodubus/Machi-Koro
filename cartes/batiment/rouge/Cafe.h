#ifndef MACHI_KORO_CAFE_H
#define MACHI_KORO_CAFE_H

#include "Batiment.h"

class Cafe : public Batiment {
    public:
        //*** Constructeurs et destructeur ***//
        Cafe();
        ~Cafe() override = default;
        Cafe(const Cafe& cafe) = default;

        //*** Getters et setters ***//
        Batiment* clone() const override {return new Cafe(*this);};

        void declencher_effet(unsigned int possesseur, int bonus = 0) const override;

};

#endif //MACHI_KORO_CAFE_H

