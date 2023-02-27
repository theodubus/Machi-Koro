#include <QStyleFactory>
#include "VuePioche.h"
#include "Partie.h"

VuePioche::VuePioche(Pioche* pioche, QWidget *parent) {

    if(pioche->get_taille() == 0) {
        pioche_exception = new QLabel;
        pioche_exception->setText("Pioche vide!");
        this->addWidget(pioche_exception,20, Qt::AlignCenter);
    }else{
        QLabel* back_carte = new QLabel;
        QPixmap img(QString::fromStdString("../assets/batiments/BACK-cartes.png"));
        img = img.scaled(120,186,Qt::KeepAspectRatio);
        back_carte->setPixmap(img);
        back_carte->setFixedSize(120,186);
        this->addWidget(back_carte,20, Qt::AlignCenter);
    }


    barre_pioche = new QProgressBar;
    barre_pioche->setRange(0,(int)pioche->get_taille_init());
    barre_pioche->setValue((int)pioche->get_taille());//valeur que l'on devra mettre à jour à chaque fois qu'on pioche une carte
    barre_pioche->setFixedWidth(280);
    barre_pioche->setStyle(QStyleFactory::create("Fusion"));
    this->addWidget(barre_pioche,100, Qt::AlignCenter);
}