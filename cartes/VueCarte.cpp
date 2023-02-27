#include <QLabel>
#include <QHBoxLayout>
#include "VueCarte.h"
#include "Monument.h"

VueCarte::VueCarte(const Carte &c,bool etat,bool est_act, QWidget *parent) : QPushButton(parent),carte(&c){
    /// Créer la vue d'une carte
    // Si il est à afficher dans VueJoueur
    //
    carte = &c;
    est_actif = est_act;
    if(!est_act){
        path_carte = carte->get_path_image();
    }
    else{
        Monument* mon = (Monument*)carte;
        path_carte = mon->get_path_image_actif();
    }
    if(etat){

        setFlat(true);
        setAutoFillBackground(true);
        setFixedSize(80, 124);
        connect(this,SIGNAL(clicked()),this,SLOT(clickedEvent()));
        setCheckable(true);
        // Nom du bouton
        this->setObjectName(QString::fromStdString(c.get_nom()));
        // Image du bouton
        pixmap = QPixmap(QString::fromStdString(path_carte));
        pixmap.scaled(80,124,Qt::KeepAspectRatio);
        ButtonIcon = QIcon(pixmap);
        this->setIconSize(QSize(80, 124));
        this->setIcon(ButtonIcon);
    }
    // S'il est à afficher dans une pop up (taille plus grande)
    else{
        setFlat(true);
        setAutoFillBackground(true);
        setFixedSize(120, 186);
        connect(this,SIGNAL(clicked()),this,SLOT(clickedEvent()));
        setCheckable(true);
        // Nom du bouton
        this->setObjectName(QString::fromStdString(c.get_nom()));
        // Image du bouton
        pixmap = QPixmap(QString::fromStdString(path_carte));
        pixmap.scaled(120,186,Qt::KeepAspectRatio);
        ButtonIcon = QIcon(pixmap);
        this->setIconSize(QSize(120, 186));
        this->setIcon(ButtonIcon);
    }
}

VueCarte::VueCarte(QWidget *parent): QPushButton(parent)
{
    setAutoFillBackground(true);
    setFixedSize(80, 140);
    connect(this,SIGNAL(clicked()),this,SLOT(clickedEvent()));
    setCheckable(false);
}
