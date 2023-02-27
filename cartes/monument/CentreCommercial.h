#ifndef MACHI_KORO_CENTRECOMMERCIAL_H
#define MACHI_KORO_CENTRECOMMERCIAL_H

#include "Monument.h"

class CentreCommercial : public Monument {
    public :
        //*** Constructeur et destructeur ***//
        CentreCommercial();
        ~CentreCommercial()override=default;
        CentreCommercial(const CentreCommercial& centreCommercial) = default;
        CentreCommercial* clone() const override {return new CentreCommercial(*this);};

        //*** Methodes ***//
        void declencher_effet(unsigned int possesseur, int bonus = 0) const override;

};

#endif //MACHI_KORO_CENTRECOMMERCIAL_H
