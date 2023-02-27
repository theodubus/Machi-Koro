#ifndef MACHI_KORO_MASTER_CHAMPFLEUR_H
#define MACHI_KORO_MASTER_CHAMPFLEUR_H

#include "Batiment.h"

class ChampFleur : public Batiment{
    public:
        //*** Constructeurs et destructeur ***//
        ChampFleur();
        ChampFleur(const ChampFleur& champFleur) = default;
        ~ChampFleur() override = default;

        //*** Methodes ***//
        Batiment* clone() const override {return new ChampFleur(*this);};

        void declencher_effet(unsigned int possesseur, int bonus = 0) const override;
};

#endif //MACHI_KORO_MASTER_CHAMPFLEUR_H
