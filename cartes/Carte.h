#ifndef MACHI_KORO_CARTE_H
#define MACHI_KORO_CARTE_H

#include <string>
#include <list>
#include <vector>
#include <iostream>
#include "gameExeption.h"

class Carte {
protected:
    std::string nom;
    std::string description_effet;
    std::string path_image;
    unsigned int prix;
    Carte(const std::string& name, const std::string& effet_description, unsigned int price, const std::string& path_picture);


public:
    virtual ~Carte() = default;

    // Getters
    const std::string& get_nom() const { return nom; }
    const std::string& get_description() const { return description_effet; }
    const std::string& get_path_image() const { return path_image; }
    unsigned int get_prix() const { return prix; }


    static unsigned int argent_effet(int a);

    virtual std::string get_type() const = 0;
    virtual void declencher_effet(unsigned int possesseur, int bonus = 0) const = 0;

};


#endif //MACHI_KORO_CARTE_H
