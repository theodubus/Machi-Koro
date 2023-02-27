#include "VueJoueur.h"
#include <QPixmap>
#include "Partie.h"

VueJoueur::VueJoueur(Joueur* j,bool e_j_a, QWidget *parent) : carte_choisie(nullptr) {
    /// Vue d'un joueur
    est_joueur_actuel = e_j_a;
    joueur = j;
    // Nom du joueur
    nom_joueur = new QLabel;
    if(j->get_est_ia()){
        nom_joueur->setText(QString::fromStdString(joueur->get_nom()+" [IA]"));
    }
    else{
        nom_joueur->setText(QString::fromStdString(joueur->get_nom()));
    }

    if (est_joueur_actuel) {
        nom_joueur->setStyleSheet("QLabel { color : green; background-color : transparent; }");

        string nom = nom_joueur->text().toStdString();
        nom += " (Joueur actuel)";
        nom_joueur->setText(QString::fromStdString(nom));
    }
    else {
        nom_joueur->setStyleSheet("QLabel { color : red; background-color : transparent; }");
    }

    QFont font = nom_joueur->font();
    font.setUnderline(true);
    nom_joueur->setFont(font);

    // Argent du joueur
    auto label_argent = new QLabel("Argent : ");
    auto monument_label = new QLabel("Monuments");
    label_argent->setAlignment(Qt::AlignCenter | Qt::AlignRight);
    monument_label->setAlignment(Qt::AlignCenter);
    argent=new QLCDNumber;
    argent->display((int)joueur->get_argent());
    argent->setFixedSize(70,30);

    // Barre avec les informations du joueur
    layout_informations = new QHBoxLayout;
    layout_informations_gauche = new QVBoxLayout;
    layout_haut_gauche = new QHBoxLayout;

    // Création de la grid
    layout_batiments = new QGridLayout;
    layout_monuments = new QGridLayout;
    layout_haut_droit = new QHBoxLayout;
    layout_droit = new QVBoxLayout;
    text_bat = new QLabel("Bâtiments");
    text_bat->setAlignment(Qt::AlignCenter);

    bat_ferme = new QPushButton("Bâtiments Fermés");
    bat_ferme->setFixedSize(150,30);
    int i = 0;
    int ind_bat;
    int ind_couleurs = 1;

    // Création des vues des batiments du joueur
    vue_batiments = new vector<VueCarte*>;
    widget_scroll_bat = new QWidget();


    for(auto &couleur : j->get_liste_batiment()){
        ind_bat = 0;
        for (auto bat : couleur.second){
            // Création vue carte
            vue_batiments->push_back(new VueCarte(*(bat.first),true, false));
            // Ajout du slot
            connect((*vue_batiments)[i],SIGNAL(carteClicked(VueCarte*)),this,SLOT(batimentClique(VueCarte*)));
            // Ajout du widget
            layout_batiments->addWidget((*vue_batiments)[i],ind_couleurs,ind_bat);
            if(bat.second>1){
                // Si plusieurs exemplaires du batiment
                QLabel* nb_bat = new QLabel;
                nb_bat->setText(QString::number(bat.second));
                nb_bat->setStyleSheet("QLabel { color : white; background-color : black; }");
                // Mise en forme
                nb_bat->setFixedSize(20,20);
                nb_bat->setAlignment(Qt::AlignTop | Qt::AlignCenter);
                // Ajout du widget
                layout_batiments->addWidget(nb_bat,ind_couleurs,ind_bat, Qt::AlignCenter);
                layout_batiments->setAlignment(nb_bat,Qt::AlignTop | Qt::AlignRight);
            }
            // Incrémentation
            ind_bat++;
            i++;
        }
        ind_couleurs++;
    }
    layout_batiments->setAlignment(Qt::AlignLeft);
    widget_scroll_bat->setLayout(layout_batiments);
    scroll_bat = new QScrollArea();
    scroll_bat->setWidget(widget_scroll_bat);
    scroll_bat->setWidgetResizable(true);
    //scroll_bat->setFixedWidth(130 * 4);
    scroll_bat->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scroll_bat->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    // Création d'un bouton donnant accès aux batiments fermés

    connect(bat_ferme,SIGNAL(clicked()),this,SLOT(affichage_bat_ferme()));

    if (joueur->get_liste_batiment_fermes().empty()){
        bat_ferme->setEnabled(false);
    }



    // Création des vues des monuments du joueur
    vue_monuments = new vector<VueCarte*>;
    widget_scroll_mon = new QWidget();
    int ind_mon=0;
    for (auto& mon : joueur->get_liste_monument()){
        vue_monuments->push_back(new VueCarte(*mon.first,true, (bool*)mon.second, parent));
        layout_monuments->addWidget((*vue_monuments)[ind_mon], ind_mon/3, ind_mon%3, Qt::Alignment());
        connect((*vue_monuments)[ind_mon],SIGNAL(carteClicked(VueCarte*)),this,SLOT(monumentClique(VueCarte*)));
        ind_mon++;
    }
    widget_scroll_mon->setLayout(layout_monuments);
    scroll_mon = new QScrollArea();
    scroll_mon->setWidget(widget_scroll_mon);
    scroll_mon->setWidgetResizable(true);
    scroll_mon->setFixedWidth(320);
    scroll_mon->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll_mon->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);


    int ind_bat_ferme = 0;
    vue_batiments_ferme = new vector<VueCarte*>;
    // Ajout des batiments fermés à une nouvelle fenêtre
    fenetre_bat_fermes = new QWidget;
    layout_batiments_ferme = new QGridLayout(fenetre_bat_fermes);
    for (auto bat: joueur->get_liste_batiment_fermes()) {
        vue_batiments_ferme->push_back(new VueCarte(*bat, false, false, fenetre_bat_fermes));
        layout_batiments_ferme->addWidget((*vue_batiments_ferme)[ind_bat_ferme], i % 4, i / 4);
        ind_bat_ferme++;
    }

    // Création du layout gauche
    layout_haut_gauche->addWidget(nom_joueur);
    layout_haut_gauche->addWidget(label_argent);
    layout_haut_gauche->addWidget(argent);

    layout_informations_gauche->addLayout(layout_haut_gauche);
    layout_informations_gauche->addWidget(monument_label);
    layout_informations_gauche->addWidget(scroll_mon);


    // Ajout des layouts à la page d'informations
    layout_informations->addLayout(layout_informations_gauche);

    layout_haut_droit->addWidget(bat_ferme);
    layout_droit->addLayout(layout_haut_droit);
    layout_droit->addWidget(text_bat);
    layout_droit->addWidget(scroll_bat);
    layout_informations->addLayout(layout_droit);

    // Définition du layout principal
    setLayout(layout_informations);
}

void VueJoueur::batimentClique(VueCarte* vc){
    /// Slot lorsque la carte est cliquée
    // Création d'une nouvelle fenetre
    if (Partie::get_instance()->get_vue_partie()->get_vue_carte() != nullptr) {
        Partie::get_instance()->get_vue_partie()->get_vue_carte()->close();
    }
    QWidget* fenetre = new QWidget();
    Partie::get_instance()->get_vue_partie()->set_vue_carte(fenetre);
    // Création d'un label contenant l'image
    QLabel *label = new QLabel(fenetre);
    QPixmap pixmap(QString::fromStdString(vc->getCarte()->get_path_image()));
    label->setPixmap(pixmap);
    label->resize(pixmap.size());
    // Affichage de la fenetre pop up
    fenetre->show();
}
void VueJoueur::monumentClique(VueCarte* vc){
    /// Slot lorsque la carte est cliquée

    bool deja_actif;

    // Création d'une nouvelle fenetre
    if (Partie::get_instance()->get_vue_partie()->get_vue_carte() != nullptr) {
        Partie::get_instance()->get_vue_partie()->get_vue_carte()->close();
    }
    QWidget* fenetre = new QWidget();
    Partie::get_instance()->get_vue_partie()->set_vue_carte(fenetre);
    // Création d'un label contenant l'image
    QLabel *label = new QLabel(fenetre);
    QPixmap pixmap;
    if(vc->get_est_actif()){
        Monument* mon = (Monument*)vc->getCarte();
        pixmap.load( QString::fromStdString(mon->get_path_image_actif()));
    }
    else{
        pixmap.load(QString::fromStdString(vc->getCarte()->get_path_image()));
    }
    label->setPixmap(pixmap);
    label->resize(pixmap.size());
    if(get_est_joueur_actuel()){
        bouton_achat = new QPushButton(fenetre);
        bouton_achat->setText(QString::fromStdString("Acheter monument"));
        carte_choisie = vc;

        Monument * mon = joueur->possede_monument(carte_choisie->getCarte()->get_nom());
        if (mon) {
            vector<Monument*> mons = joueur->get_monument_jouables();
            if (std::find(mons.begin(), mons.end(), mon) != mons.end()) {
                deja_actif = true;
            }
        }

        connect(bouton_achat, SIGNAL(clicked()), this, SLOT(clicked_acheter_event()));


        auto indice = Partie::get_instance()->get_joueur_actuel();
        auto argent_joueur = Partie::get_instance()->get_tab_joueurs()[indice]->get_argent();
        auto prix = vc->getCarte()->get_prix();

        if(Partie::get_instance()->get_moment_achat() && !joueur->get_est_ia() && argent_joueur >= prix && !deja_actif){
            bouton_achat->setEnabled(true);
        }else{
            bouton_achat->setEnabled(false);
        }
    }

    // Affichage de la fenetre pop up
    fenetre->show();
}



void VueJoueur::affichage_bat_ferme(){
    /// Affichage de la fenetre avec les batiments fermés
    // Ouverture de la fenetre si elle est fermée
    fenetre_bat_fermes->show();
}

void VueJoueur::clicked_acheter_event(){
    Partie *partie = Partie::get_instance();
    partie->get_vue_partie()->get_vue_carte()->close();
    VueCarte* carte = partie->get_vue_partie()->get_vue_joueur()->get_carte_choisie();
    partie->acheter_carte_event(carte);

    carte_choisie = nullptr;
    partie->suite_tour(true);
}