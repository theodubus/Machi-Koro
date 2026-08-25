#ifndef MACHI_KORO_CHAMPBLE_H
#define MACHI_KORO_CHAMPBLE_H

#include "Batiment.h"

class ChampBle : public Batiment{
    public:
        //*** Constructeurs et destructeur ***//
        ChampBle();
        ~ChampBle() override = default;
        ChampBle(const ChampBle& champBle) = default;

        //*** Méthodes ***//
        Batiment* clone() const override {return new ChampBle(*this);};

        void declencher_effet(const ContexteDeclenchement& ctx) const override;
};

#endif //MACHI_KORO_CHAMPBLE_H
