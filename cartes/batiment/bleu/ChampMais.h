#ifndef MACHI_KORO_MASTER_CHAMPSMAIS_H
#define MACHI_KORO_MASTER_CHAMPSMAIS_H

#include "Batiment.h"

class ChampMais : public Batiment{
    public:
        //*** Constructeurs et destructeur ***//
        ChampMais();
        ~ChampMais() override = default;
        ChampMais(const ChampMais& champMais) = default;

        //*** Méthodes ***//
        Batiment* clone() const override {return new ChampMais(*this);};

        void declencher_effet(unsigned int possesseur, int bonus = 0) const override;
};

#endif //MACHI_KORO_MASTER_CHAMPSMAIS_H
