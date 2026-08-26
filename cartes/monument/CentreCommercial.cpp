#include "CentreCommercial.h"
#include "Partie.h"

using namespace std;

CentreCommercial::CentreCommercial() :
    Monument("CentreCommercial",
             10,
             "Les etablissement cafe et commerce vous rapportent une piece de plus",
             "../assets/monuments/Centre-commercial-travaux.png",
             "../assets/monuments/Centre-commercial-active.png") {
    /// Constructeur de Centre commercial
}

void CentreCommercial::declencher_effet(const ContexteDeclenchement& ctx) const {
    /// Ce monument n'a pas d'effet propre a declencher : son supplement d'une piece
    /// est applique carte par carte, via Batiment::beneficie_centre_commercial().
    /// Il ne reste ici qu'a signaler son activation dans le journal.
    Joueur* possesseur = Partie::get_instance()->get_tab_joueurs()[ctx.possesseur];
    Partie::get_instance()->get_vue_partie()->get_vue_infos()->add_info(
            "Activation de " + get_nom_affiche() + " chez " + possesseur->get_nom());
}
