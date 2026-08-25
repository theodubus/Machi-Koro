#ifndef MACHI_KORO_MASTER_BATEAUPECHE_H
#define MACHI_KORO_MASTER_BATEAUPECHE_H

#include "Batiment.h"

class PetitBateauDePeche : public Batiment{
    public:
        //*** Constructeurs et destructeur ***//
        PetitBateauDePeche();
        ~PetitBateauDePeche() override = default;
        PetitBateauDePeche(const PetitBateauDePeche& petitBateauDePeche) = default;

        //*** Méthodes ***//
        Batiment* clone() const override {return new PetitBateauDePeche(*this);};

        void declencher_effet(const ContexteDeclenchement& ctx) const override;
};

#endif //MACHI_KORO_MASTER_BATEAUPECHE_H
