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


VueDes::VueDes(QWidget *parent) {
    label_de1 = new QLabel;
    label_de2 = new QLabel;
    movie_de1 = nullptr;
    movie_de2 = nullptr;
    anim1_finie = true;
    anim2_finie = true;
}

VueDes::~VueDes() {
    delete label_de1;
    delete label_de2;
    if (movie_de1 != nullptr) {
        movie_de1->stop();
    }
    if (movie_de2 != nullptr) {
        movie_de2->stop();
    }
}

void VueDes::launch_des(unsigned int de1, unsigned int de2) {
    if (de1 > 6 || de1 < 1 || de2 > 6) {
        return;
    }

    anim1_finie = false;

    auto partie = Partie::get_instance();
    movie_de1 = partie->get_vue_partie()->get_animation_de(de1);

    movie_de1->setScaledSize(QSize(120,170));

    connect(movie_de1, SIGNAL(frameChanged(int)), this, SLOT(frameChanged_Handler1(int)));
    label_de1->setMovie(movie_de1);
    movie_de1->start();

    this->addWidget(label_de1,20, Qt::AlignCenter);


    if (de2 != 0) {
        anim2_finie = false;
        movie_de2 = partie->get_vue_partie()->get_animation_de(de2);
        movie_de2->setScaledSize(QSize(120,170));

        connect(movie_de2, SIGNAL(frameChanged(int)), this, SLOT(frameChanged_Handler2(int)));
        label_de2->setMovie(movie_de2);
        movie_de2->start();
        this->addWidget(label_de2,20, Qt::AlignCenter);
    }


}