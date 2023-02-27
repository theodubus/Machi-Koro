#ifndef CLUB_PRIVE_H
#define CLUB_PRIVE_H

#include "Batiment.h"

class ClubPrive : public Batiment {
    public:
        //*** Constructeurs et destructeur ***//
        ClubPrive();
        ~ClubPrive() override = default;
        ClubPrive(const ClubPrive& clubPrive) = default;

        //*** Methodes ***//
        Batiment* clone() const override {return new ClubPrive(*this);};

        void declencher_effet(unsigned int possesseur, int bonus = 0) const override;
};

#endif 