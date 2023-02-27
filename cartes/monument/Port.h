#ifndef MACHI_KORO_PORT_H
#define MACHI_KORO_PORT_H

#include "Monument.h"

class Port : public Monument {
    public :
        //*** Constructeur et destructeur ***//
        Port();
        ~Port()override=default;
        Port(const Port& port) = default;
        Port* clone() const override {return new Port(*this);};

        //*** Methodes ***//
        void declencher_effet(unsigned int possesseur, int bonus = 0) const override;
};


#endif //MACHI_KORO_PORT_H
