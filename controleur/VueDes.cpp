#include "VueDes.h"

#include "Partie.h"


void VueDes::frameChanged_Handler1(int frameNumber) {
    if(frameNumber == (movie_de1->frameCount()-1)) {
        movie_de1->stop();
        anim1_finie = true;
    }
}

void VueDes::frameChanged_Handler2(int frameNumber) {
    if(frameNumber == (movie_de2->frameCount()-1)) {
        movie_de2->stop();
        anim2_finie = true;
    }
}


VueDes::VueDes(QWidget *parent) : QHBoxLayout(parent) {
    label_de1 = new QLabel;
    label_de2 = new QLabel;
    label_de1->setFixedSize(120, 170);
    label_de2->setFixedSize(120, 170);
    label_de1->setAlignment(Qt::AlignCenter);
    label_de2->setAlignment(Qt::AlignCenter);
    movie_de1 = nullptr;
    movie_de2 = nullptr;
    anim1_finie = true;
    anim2_finie = true;

    // Les deux emplacements sont poses une fois pour toutes : chaque tour se
    // contente de changer l'animation qu'ils portent. Les ajouter a chaque lance
    // empilait des QLabel dans la disposition.
    addWidget(label_de1, 20, Qt::AlignCenter);
    addWidget(label_de2, 20, Qt::AlignCenter);
    label_de1->hide();
    label_de2->hide();
}

VueDes::~VueDes() {
    // Rien a liberer ici, et surtout rien a toucher.
    //
    // Les deux QLabel sont poses dans la disposition des le constructeur : ils
    // appartiennent au widget qui la porte, qui les detruit avant que ce
    // destructeur ne s'execute.
    //
    // Les QMovie appartiennent a VuePartie, qui les partage entre les tours. Quand
    // c'est elle qui disparait, rien ne garantit qu'ils soient encore vivants a ce
    // moment : les arreter ici lisait de la memoire deja liberee.
}

void VueDes::launch_des(unsigned int de1, unsigned int de2) {
    // Les valeurs hors 1..6 ne correspondent a aucune animation. Le bonus du Port
    // s'ajoutant au total et non aux des, ce cas ne devrait plus se produire.
    if (de1 < 1 || de1 > 6 || de2 > 6) {
        label_de1->hide();
        label_de2->hide();
        return;
    }

    if (movie_de1 != nullptr) {
        movie_de1->stop();
        disconnect(movie_de1, nullptr, this, nullptr);
    }
    if (movie_de2 != nullptr) {
        movie_de2->stop();
        disconnect(movie_de2, nullptr, this, nullptr);
    }

    auto partie = Partie::get_instance();

    anim1_finie = false;
    movie_de1 = partie->get_vue_partie()->get_animation_de(de1);
    movie_de1->setScaledSize(QSize(120, 170));
    connect(movie_de1, SIGNAL(frameChanged(int)), this, SLOT(frameChanged_Handler1(int)));
    label_de1->setMovie(movie_de1);
    label_de1->show();
    movie_de1->start();

    if (de2 != 0) {
        anim2_finie = false;
        movie_de2 = partie->get_vue_partie()->get_animation_de(de2);
        movie_de2->setScaledSize(QSize(120, 170));
        connect(movie_de2, SIGNAL(frameChanged(int)), this, SLOT(frameChanged_Handler2(int)));
        label_de2->setMovie(movie_de2);
        label_de2->show();
        movie_de2->start();
    } else {
        anim2_finie = true;
        movie_de2 = nullptr;
        label_de2->hide();
    }
}
