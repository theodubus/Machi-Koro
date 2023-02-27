#ifndef MACHI_KORO_MONUMENT_H
#define MACHI_KORO_MONUMENT_H

#include "Carte.h"

class Monument : public Carte{
    protected:
        std::string path_image_actif;
        Monument(const std::string& nom, unsigned int prix, const std::string& description_effet, const std::string& path_image,const std::string& path_actif);


    public:
        //*** Constructeur et destructeur ***//
        ~Monument()override=default;
        virtual Monument* clone() const = 0;

        //*** GETTER ***//
        std::string get_type() const override {return "Monument";};
        const std::string& get_path_image_actif() const { return path_image_actif; }
};

#endif //MACHI_KORO_MONUMENT_H
