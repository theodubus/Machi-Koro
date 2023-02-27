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

void Port::declencher_effet(unsigned int possesseur, int bonus) const {
    Partie *partie = Partie::get_instance();
    if (partie->get_de_1() + partie->get_de_2() >= 10) {
        Joueur *joueur = partie->get_tab_joueurs()[possesseur];

        if (partie->get_tab_joueurs()[possesseur]->get_est_ia()) {
            if (rand() % 4 == 1) {
                partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet du Port du joueur \"" + joueur->get_nom() + "\"");
                partie->set_de_1(partie->get_de_1() + 1);
                partie->set_de_2(partie->get_de_2() + 1);
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
                string effet = "Activation de l'effet du Port du joueur \"" + joueur->get_nom() + "\"";
                partie->set_de_2(partie->get_de_2() + 1);
                partie->set_de_1(partie->get_de_1() + 1);
                partie->get_vue_partie()->update_des();
                partie->get_vue_partie()->get_vue_infos()->add_info(effet);
            }
        }
    }
}
