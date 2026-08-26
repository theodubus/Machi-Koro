#ifndef MACHI_KORO_CARTE_H
#define MACHI_KORO_CARTE_H

#include <string>
#include <list>
#include <vector>
#include <iostream>
#include "gameExeption.h"
#include "ContexteDeclenchement.h"

/// Nom d'une carte tel qu'il figure sur son visuel.
///
/// `get_nom()` rend l'identifiant interne — « HotelDeVille », « ChampBle » — qui
/// sert de cle dans tout le code : recherche d'un monument, comparaison a l'achat,
/// table de reference des cartes. Il n'est pas montrable a un joueur, et il l'a
/// pourtant ete pendant toute la vie du projet, jusque dans le journal de partie.
const std::string& nom_lisible(const std::string& nom_interne);

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
    /// Le nom a montrer au joueur. Voir nom_lisible().
    const std::string& get_nom_affiche() const { return nom_lisible(nom); }
    const std::string& get_description() const { return description_effet; }
    const std::string& get_path_image() const { return path_image; }
    unsigned int get_prix() const { return prix; }


    static unsigned int argent_effet(int a);

    /// Seule question que le controleur posait a get_type() sur une Carte :
    /// distinguer un batiment d'un monument au moment de l'achat.
    virtual bool est_monument() const = 0;
    virtual void declencher_effet(const ContexteDeclenchement& ctx) const = 0;

};


#endif //MACHI_KORO_CARTE_H
