#include "Port.h"
#include "Partie.h"

Port::Port()
        : Monument("Port",
                   2,
                   "Si le resultat du jet de de est superieur ou egal a 10, vous pouvez ajouter 2 a ce resultat.",
                   "../assets/monuments/Port-travaux.png",
                   "../assets/monuments/Port-active.png") {
    /// Constructeur de Port
}

void Port::declencher_effet(const ContexteDeclenchement& ctx) const {
    Partie *partie = Partie::get_instance();
    if (partie->get_de_1() + partie->get_de_2() >= 10) {
        Joueur *joueur = partie->get_tab_joueurs()[ctx.possesseur];

        if (partie->get_tab_joueurs()[ctx.possesseur]->get_est_ia()) {
            if (rand() % 4 == 1) {
                partie->get_vue_partie()->get_vue_infos()->add_info("Activation de " + get_nom_affiche() + " chez " + joueur->get_nom());
                // On ajoute 2 au resultat, et non 1 a chaque de : le livret precise
                // « vous ne pouvez pas choisir de n'ajouter que 1 », et incrementer les
                // des ferait afficher des faces a 7.
                partie->ajouter_bonus_port();
            }
        } else {
            QMessageBox msgBox;
            string message = "Voulez-vous ajouter 2 au résultat du jet de dés qui est de " +
                             to_string(partie->get_de_1() + partie->get_de_2()) + " ?";
            string titre = "Effet du Port du joueur \"" + joueur->get_nom() + "\"";
            msgBox.setWindowTitle(QString::fromStdString(titre));
            msgBox.setText(QString::fromStdString(message));
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::No);
            int ret = msgBox.exec();
            if (ret == QMessageBox::Yes) {
                string effet = "Activation de " + get_nom_affiche() + " chez " + joueur->get_nom();
                partie->ajouter_bonus_port();
                partie->get_vue_partie()->update_des();
                partie->get_vue_partie()->get_vue_infos()->add_info(effet);
            }
        }
    }
}
