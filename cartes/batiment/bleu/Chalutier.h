#ifndef MACHI_KORO_MASTER_CHALUTIER_H
#define MACHI_KORO_MASTER_CHALUTIER_H

#include "Batiment.h"

class Chalutier : public Batiment{
    public:
        //*** Constructeur et destructeur ***//
        Chalutier();
        ~Chalutier() override = default;
        Chalutier(const Chalutier& chalutier) = default;

        //*** Methodes ***//
        Batiment* clone() const override {return new Chalutier(*this);};

        void declencher_effet(unsigned int possesseur, int bonus = 0) const override;
};
#endif //MACHI_KORO_MASTER_CHALUTIER_H
