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
    string info_affiche = "\n" + info + "\n";
    QLabel *label = new QLabel(QString::fromStdString(info_affiche), widget_layout_info);
    label->setAlignment(Qt::AlignCenter);
    label->setFixedWidth(250);
    label->setContentsMargins(0, 200, 0, 200);
    label->setWordWrap(true);

    info_layout->insertWidget(0, label);
    liste_info.push_back(label);

}