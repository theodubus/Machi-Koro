#ifndef SRC_HOTELDEVILLE_H
#define SRC_HOTELDEVILLE_H

#include "Monument.h"

class HotelDeVille : public Monument {
public :
    //*** Constructeur et destructeur ***//
    HotelDeVille();
    ~HotelDeVille()override=default;
    HotelDeVille(const HotelDeVille &hotelDeVille) = default;
    HotelDeVille* clone() const override {return new HotelDeVille(*this);};

    //*** Methodes ***//
    void declencher_effet(const ContexteDeclenchement& ctx) const override;

};

#endif //SRC_HOTELDEVILLE_H
