#ifndef MACHI_KORO_MASTER_MINE_H
#define MACHI_KORO_MASTER_MINE_H

#include "Batiment.h"

class Mine : public Batiment{
    public:
        //*** Constructeurs et destructeur ***//
        Mine();
        ~Mine() override = default;
        Mine(const Mine& mine) = default;

        //*** Méthodes ***//
        Batiment* clone() const override {return new Mine(*this);};

        void declencher_effet(const ContexteDeclenchement& ctx) const override;
};
#endif //MACHI_KORO_MASTER_MINE_H
