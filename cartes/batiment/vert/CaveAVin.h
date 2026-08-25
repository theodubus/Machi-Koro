#ifndef SRC_CAVEAVIN_H
#define SRC_CAVEAVIN_H

#include "Batiment.h"

class CaveAVin : public Batiment {
public:
    CaveAVin();
    ~CaveAVin() override = default;
    CaveAVin(const CaveAVin& caveAVin) = default;
    CaveAVin* clone() const override {return new CaveAVin(*this);};
    void declencher_effet(const ContexteDeclenchement& ctx) const override;
};


#endif //SRC_CAVEAVIN_H
