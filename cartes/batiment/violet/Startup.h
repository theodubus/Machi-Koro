#ifndef MACHI_KORO_STARTUP_H
#define MACHI_KORO_STARTUP_H

#include "Batiment.h"

class Startup : public Batiment {
    public:
        Startup();
        Batiment* clone() const override {return new Startup(*this);};
        void declencher_effet(unsigned int possesseur, int bonus = 0) const override;
};

#endif //MACHI_KORO_STARTUP_H
