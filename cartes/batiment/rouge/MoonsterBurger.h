#ifndef MACHI_KORO_MOONSTERBURGER_H
#define MACHI_KORO_MOONSTERBURGER_H

#include "Batiment.h"

class MoonsterBurger : public Batiment {
    public:
        //*** Constructeurs et destructeur ***//
        MoonsterBurger();
        ~MoonsterBurger() override = default;
        MoonsterBurger(const MoonsterBurger& autre) = default;

        //*** Methodes ***//
        Batiment* clone() const override {return new MoonsterBurger(*this);};

        void declencher_effet(const ContexteDeclenchement& ctx) const override;
};

#endif //MACHI_KORO_MOONSTERBURGER_H