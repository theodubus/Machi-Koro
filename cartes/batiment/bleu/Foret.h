#ifndef MACHI_KORO_MASTER_FORET_H
#define MACHI_KORO_MASTER_FORET_H

#include "Batiment.h"

class Foret : public Batiment{
    public:
        //*** Constructeurs et destructeur ***//
        Foret();
        ~Foret() override = default;
        Foret(const Foret& foret) = default;

        //*** Méthodes ***//
        Batiment* clone() const override {return new Foret(*this);};

        void declencher_effet(unsigned int possesseur, int bonus = 0) const override;
};

#endif //MACHI_KORO_MASTER_FORET_H
