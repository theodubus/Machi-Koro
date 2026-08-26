#ifndef MACHI_KORO_VUEINFO_H
#define MACHI_KORO_VUEINFO_H

#include <string>

class ScenePlateau;

/// Le journal de la partie.
///
/// Une centaine d'endroits du modele et du controleur annoncent ce qu'ils font
/// par `get_vue_infos()->add_info(...)`. Cette classe garde ce point d'entree
/// intact et se contente de porter le message jusqu'au panneau du plateau : les
/// cartes n'ont pas a savoir comment le journal est affiche, et l'affichage a pu
/// changer entierement sans toucher a une seule d'entre elles.
///
/// Elle etait auparavant une disposition Qt qui empilait un QLabel par message.
class VueInfo {
    public :
        explicit VueInfo(ScenePlateau* scene);
        void add_info(const std::string& info);

    private:
        ScenePlateau* plateau;
};

#endif //MACHI_KORO_VUEINFO_H
