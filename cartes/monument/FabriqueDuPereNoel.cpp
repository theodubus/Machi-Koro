#include "FabriqueDuPereNoel.h"
#include "Partie.h"

FabriqueDuPereNoel::FabriqueDuPereNoel() :
        Monument("FabriqueDuPereNoel",
                 0,
                 "Si le jet de de est casse vous gagnez 3 pieces",
                 "../assets/monuments/FabriqueDuPereNoel.png",
                 "../assets/monuments/FabriqueDuPereNoel.png") {
    /// Constructeur de la Fabrique du Pere Noel
}

void FabriqueDuPereNoel::declencher_effet(const ContexteDeclenchement& ctx) const {
    /// « Une fois par tour, si votre lance de des est casse, recevez 3 pieces de la
    /// banque. » Un lance casse — « auf der Kippe » dans la version allemande — est
    /// un de qui ne repose pas a plat et n'affiche donc pas de resultat net. La regle
    /// veut alors qu'on relance le meme nombre de des, et qu'on encaisse 3 pieces.
    /// « Une fois par tour » : si la relance retombe cassee, pas de seconde prime.
    ///
    /// Un de qui bascule sur une arete ne peut evidemment pas se produire dans un jeu
    /// numerique. Le projet en simule donc la survenue par un tirage separe de quatre
    /// des dont la somme doit valoir 16, soit environ 9,6 % des tours. Ce tirage
    /// n'entre dans aucun autre calcul et ne declenche aucun autre effet.
    Partie *partie = Partie::get_instance();
    Joueur *joueur = partie->get_tab_joueurs()[ctx.possesseur];

    partie->get_vue_partie()->get_vue_infos()->add_info("Le lance de des du joueur \"" + joueur->get_nom() + "\" est casse : la Fabrique du Pere Noel rapporte 3 pieces et les des sont relances");

    joueur->set_argent(joueur->get_argent() + 3);

    // On relance le meme nombre de des : deux si la Gare a ete jouee, un sinon.
    partie->set_de_1(Partie::lancer_de());
    if (partie->get_de_2() != 0) {
        partie->set_de_2(Partie::lancer_de());
    }
}
