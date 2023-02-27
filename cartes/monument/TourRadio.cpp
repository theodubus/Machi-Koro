#include "TourRadio.h"
#include "Partie.h"

TourRadio::TourRadio() :
    Monument("TourRadio",
             22,
             "Une fois par tour vous pouvez relancer vos des",
             "../assets/monuments/Tour-radio-travaux.png",
             "../assets/monuments/Tour-radio-active.png") {
    /// Constructeur de TourRadio
}

void TourRadio::declencher_effet(unsigned int possesseur, int bonus) const {
    Joueur *joueur = Partie::get_instance()->get_tab_joueurs()[possesseur];
    Partie *partie = Partie::get_instance();

    if (partie->get_tab_joueurs()[possesseur]->get_est_ia()) {
        if(rand() % 8 == 1) {
            partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet de la Tour radio du joueur \"" + joueur->get_nom() + "\"");
            partie->set_de_1(Partie::lancer_de());
            if (partie->get_de_2() != 0) {
                partie->set_de_2(Partie::lancer_de());
            }
        }
    }
    else {
        QMessageBox msgBox;
        string message = "Votre résultat de dés est de : dé 1=" + to_string(partie->get_de_1())
                + " dé 2=" + to_string(partie->get_de_2()) + ". Voulez-vous relancer vos dés ?";
        string titre = "Effet de la Tour radio du joueur \"" + joueur->get_nom() + "\"";
        msgBox.setWindowTitle(QString::fromStdString(titre));
        msgBox.setText(QString::fromStdString(message));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        if (ret == QMessageBox::Yes) {
            string effet = "Activation de l'effet de la Tour radio du joueur \"" + joueur->get_nom() + "\"";
            partie->set_de_1(Partie::lancer_de());
            if (partie->get_de_2() != 0) {
                partie->set_de_2(Partie::lancer_de());
            }
            partie->get_vue_partie()->update_des();
            partie->get_vue_partie()->get_vue_infos()->add_info(effet);
        }
    }
}
