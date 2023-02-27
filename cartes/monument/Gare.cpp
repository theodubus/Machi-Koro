#include "Gare.h"
#include "Partie.h"
#include <QMessageBox>

Gare::Gare()
    : Monument("Gare",
               4,
               "Vous pouvez lancer 2 des",
               "../assets/monuments/Gare-travaux.png",
               "../assets/monuments/Gare-active.png") {
    /// Constructeur de Gare
}

void Gare::declencher_effet(unsigned int possesseur, int bonus) const {
    Joueur *joueur = Partie::get_instance()->get_tab_joueurs()[possesseur];
    Partie *partie = Partie::get_instance();
    if (joueur->get_est_ia()) {
        int choix = rand() % 4;
        if (choix != 0) {
            partie->get_vue_partie()->get_vue_infos()->add_info("Activation de l'effet de la gare du joueur \"" + joueur->get_nom() + "\"");
            partie->set_de_2(Partie::lancer_de());
        }
    }
    else {
        QMessageBox msgBox;
        string title = "Effet de la gare du joueur \"" + joueur->get_nom() + "\"";
        msgBox.setWindowTitle(QString::fromStdString(title));
        msgBox.setText("Voulez-vous lancer deux dés ?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        if (ret == QMessageBox::Yes) {
            string effet = "Activation de l'effet de la gare du joueur \"" + joueur->get_nom() + "\"";
            partie->set_de_2(Partie::lancer_de());
            partie->get_vue_partie()->update_des();
            partie->get_vue_partie()->get_vue_infos()->add_info(effet);
        }
    }
}

