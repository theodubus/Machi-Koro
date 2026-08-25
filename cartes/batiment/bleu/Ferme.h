#ifndef MACHI_KORO_FERME_H
#define MACHI_KORO_FERME_H

#include "Batiment.h"

class Ferme : public Batiment{
    public:
        //*** Constructeurs et destructeur ***//
        Ferme();
        ~Ferme() override = default;
        Ferme(const Ferme& ferme) = default;

        //*** Méthodes ***//
        Batiment* clone() const override {return new Ferme(*this);};

        void declencher_effet(const ContexteDeclenchement& ctx) const override;
};

#endif //MACHI_KORO_MASTER_RANCH_H
