#ifndef SRC_MGAGAMECENTER_H
#define SRC_MGAGAMECENTER_H

#include "Batiment.h"

class MgaGameCenter : public Batiment {
public:
    MgaGameCenter();
    ~MgaGameCenter() override = default;
    MgaGameCenter(const MgaGameCenter& mgaGameCenter) = default;
    Batiment* clone() const override {return new MgaGameCenter(*this);};
    void declencher_effet(unsigned int possesseur, int bonus = 0) const override;
};


#endif //SRC_MGAGAMECENTER_H
