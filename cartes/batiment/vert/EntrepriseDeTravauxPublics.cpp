#include "EntrepriseDeTravauxPublics.h"
#include "Partie.h"

EntrepriseDeTravauxPublics::EntrepriseDeTravauxPublics()
        : Batiment("EntrepriseDeTravauxPublics",
                   2,
                   "Vous devez retourner une de vos cartes monuments sur sa face en travaux. Ce faisant, vous recevez 8 pieces de la banque.",
                   "../assets/batiments/Vert/EntrepriseDeTravauxPublics.png",
                   Vert,
                   {4},
                   "entreprise") {}

void EntrepriseDeTravauxPublics::declencher_effet(unsigned int possesseur, int bonus) const {
    /// Effet de l'EntrepriseDeTravauxPublics

    // DESACTIVATION DU MONUMENT

    Partie * partie = Partie::get_instance();
    Joueur* j_actuel = partie->get_tab_joueurs()[possesseur];



    partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet de la carte Entreprise de travaux publics du joueur \""+ j_actuel->get_nom() + "\"");
    Monument* monument_choisi = j_actuel->selectionner_monument();


    if (monument_choisi == nullptr){
        return;
    }


    // On desactive le monument
    j_actuel->desactiver_monument(monument_choisi);

    // TRANSACTION AVEC LA BANQUE
    // On donne 8 pieces au joueur actuel
    j_actuel->set_argent(j_actuel->get_argent() + 8 + bonus);
}