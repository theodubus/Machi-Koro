#ifndef MACHI_KORO_TOURRADIO_H
#define MACHI_KORO_TOURRADIO_H

#include "Monument.h"

class TourRadio : public Monument {
    public:
        //*** Constructeur et destructeur ***//
        TourRadio();
        ~TourRadio()override=default;
        TourRadio(const TourRadio& tourRadio) = default;
        TourRadio* clone() const override {return new TourRadio(*this);};

        //*** Methodes ***//
        void declencher_effet(unsigned int possesseur, int bonus = 0) const override;

};

#endif //MACHI_KORO_TOURRADIO_H
