#include "VueShop.h"
#include "Partie.h"


VueShop::VueShop(Shop *shop, QWidget *parent)  : carte_choisie(nullptr){
    /// Calcul de la dimension de la grille
    largeur = floor(sqrt(shop->get_nb_tas_reel()));

    int x = 0;
    int y = 1;
    unsigned int compteur=0;

    for(auto &it : shop->get_contenu()){
        tab_vue_shop.push_back(new VueCarte(*(it.first),true, false));

        connect((tab_vue_shop)[compteur],SIGNAL(carteClicked(VueCarte*)),this,SLOT(batiment_clique(VueCarte*)));
        this->addWidget((tab_vue_shop)[compteur],x,y-1);
        if(it.second>1){
            // cas où on a plusieurs cartes identiques les unes sur les autres
            QLabel* nb_carte = new QLabel;
            nb_carte->setText(QString::number(it.second));
            //nb_carte->setAttribute(Qt::WA_TranslucentBackground);
            nb_carte->setStyleSheet("QLabel { color : white; background-color : black; }");
            nb_carte->setFixedSize(20,20);
            nb_carte->setAlignment(Qt::AlignTop | Qt::AlignCenter);
            this->addWidget(nb_carte,x,y-1, Qt::AlignCenter);
            this->setAlignment(nb_carte,Qt::AlignTop | Qt::AlignRight);
        }
        if(y % largeur == 0){
            x ++;
            y = 1;
        }else{
            y++;
        }
        compteur++;
    }
}

void VueShop::batiment_clique(VueCarte *vc) {
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
    // Si on est dans la phase d'achat
    // On autorise l'achat
    bouton_acheter = new QPushButton(fenetre);
    bouton_acheter->setText(QString::fromStdString("Acheter le batiment"));

    auto indice = Partie::get_instance()->get_joueur_actuel();
    auto joueur = Partie::get_instance()->get_tab_joueurs()[indice];
    auto argent = joueur->get_argent();
    auto carte = (Batiment *) vc->getCarte();
    auto prix = carte->get_prix();
    bool deja_possede = false;


    if (carte->get_couleur() == Violet){
        auto nom = carte->get_nom();
        if (joueur->possede_batiment(nom)){
            deja_possede = true;
        }
    }

    if(Partie::get_instance()->get_moment_achat() && !joueur->get_est_ia() && argent >= prix && !deja_possede){
        bouton_acheter->setEnabled(true);
    }else{
        bouton_acheter->setEnabled(false);
    }

    carte_choisie = vc;
    // Ajout du bouton acheter
    connect(bouton_acheter, SIGNAL(clicked()), this, SLOT(clicked_acheter_event()));

    label->setPixmap(pixmap);
    label->resize(pixmap.size());
    // Affichage de la fenetre pop up
    fenetre->show();
}

void VueShop::clicked_acheter_event(){
    /// Slot lorsque le bouton acheter est cliqué
    Partie *partie = Partie::get_instance();
    // Si on est dans la phase d'achat
    if (partie->get_moment_achat()) {
        partie->set_moment_achat(false);
        partie->acheter_carte_event(carte_choisie);
        carte_choisie = nullptr;
        // On ferme la fenêtre d'achat
        Partie::get_instance()->get_vue_partie()->get_vue_carte()->close();
        Partie::get_instance()->get_vue_partie()->update_vue_partie();
        // On met la popup à nullptr
        Partie::get_instance()->get_vue_partie()->set_vue_carte(nullptr);
        Partie::get_instance()->get_vue_partie()->set_bouton_rien_faire(false);
        Partie::get_instance()->suite_tour(true);
    }
    else
    {
        // Fenetre pop up d'erreur
        QWidget *pop_up = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout();
        QLabel *label = new QLabel();
        label->setText("Désolé. Vous ne pouvez pas acheter. Ce n'est pas la phase d'achat...");
        QPushButton* bouton_erreur = new QPushButton(pop_up);
        bouton_erreur->setText(QString::fromStdString("Je comprends"));
        // Ajout du bouton d'erreur pour valider
        QObject::connect(bouton_erreur, &QPushButton::clicked, [pop_up]() {
            pop_up->close();
        });
        // Ajout des widgets
        layout->addWidget(label);
        layout->addWidget(bouton_erreur);
        pop_up->setLayout(layout);
        // Affichage de la fenetre pop up
        pop_up->show();
    }
}