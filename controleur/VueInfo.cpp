#include "VueInfo.h"
using namespace std;

VueInfo::VueInfo(QWidget *parent) {
    /// Création de la zone d'information permanente
    info_permament = new QLabel("Informations sur le tour actuel :");
    info_permament->setAlignment(Qt::AlignCenter);
    info_permament->setFixedSize(290, 50); // taille du petit label vert
    info_permament->setStyleSheet("QLabel { background-color : green; \
                                color : white; \
                                font-size : 12px; \
                                font-weight : bold; \
                                border-radius : 10px; \
                                border : 1px solid black; \
                                }");
    this->addWidget(info_permament, 1, Qt::AlignCenter);

    /// Création de la zone d'information dynamique
    scroll_info = new QScrollArea();
    scroll_info->setFixedSize(290, 400); // taille du cadre de la zone d'info
    scroll_info->setWidgetResizable(true);
    scroll_info->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll_info->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    widget_layout_info = new QWidget(scroll_info);
    info_layout = new QVBoxLayout(widget_layout_info);

    widget_layout_info->setLayout(info_layout);

    scroll_info->setWidget(widget_layout_info);

    this->addWidget(scroll_info, 1, Qt::AlignCenter);
}

void VueInfo::add_info(const string &info) {
    /// Ajoute une ligne en tete du journal, la plus recente restant visible sans
    /// avoir a faire defiler.
    ///
    /// Chaque message portait 200 px de marge en haut et en bas : cinq lignes
    /// suffisaient a remplir tout le panneau, et le journal etait inutilisable. Et
    /// aucun message n'etait jamais oublie, si bien que la liste grossissait sans
    /// limite pendant toute la partie.
    QLabel *label = new QLabel(QString::fromStdString(info), widget_layout_info);
    label->setAlignment(Qt::AlignCenter);
    label->setFixedWidth(250);
    label->setContentsMargins(0, 4, 0, 4);
    label->setWordWrap(true);

    info_layout->insertWidget(0, label);
    liste_info.push_back(label);

    // Au-dela de cette limite, les plus anciens messages sont oublies. Une partie
    // en produit plusieurs dizaines par tour et dure plus de cent tours.
    const size_t limite = 200;
    if (liste_info.size() > limite) {
        size_t a_retirer = liste_info.size() - limite;
        for (size_t i = 0; i < a_retirer; i++) {
            QLabel* vieux = liste_info[i];
            info_layout->removeWidget(vieux);
            delete vieux;
        }
        liste_info.erase(liste_info.begin(), liste_info.begin() + a_retirer);
    }
}
