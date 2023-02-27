#ifndef MACHI_KORO_AEROPORT_H
#define MACHI_KORO_AEROPORT_H

#include "Monument.h"

class Aeroport : public Monument {
    public:
        //*** Constructeur et destructeur ***//
        Aeroport();
        ~Aeroport()override=default;
        Aeroport(const Aeroport& aeroport) = default;
        Aeroport* clone() const override {return new Aeroport(*this);};

        //*** Methodes ***//
        void declencher_effet(unsigned int possesseur, int bonus = 0) const override;

};

#endif //MACHI_KORO_AEROPORT_H
