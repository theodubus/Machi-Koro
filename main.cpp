#include <iostream>

/// QT Includes
#include <QApplication>
#include <QtGui/QScreen>
#include <QLineEdit>
#include <QRadioButton>
#include <QLayoutItem>
#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QStandardItem>


/// Project Includes
#include "Partie.h"
#include <set>
using namespace std;

void resize_and_center(QWidget *widget, int width, int height)
{
    widget->resize(width, height);
    widget->move(QApplication::primaryScreen()->geometry().center() - widget->rect().center());
}

void validate_menu_2(QWidget *menu, const string &edition, const list<string> &extensions, const map<string, string>& joueurs, const string& shop_type, unsigned int shop_size){
    //QWidget fenetre;
    // affichage des infos
    string mes_infos = "Edition: " + edition + "\n";
    if (edition == "Deluxe"){
        mes_infos += "Cette edition est basée sur la version française du jeu.\n";
        mes_infos += "Elle condense ainsi les cartes de l'édition standard et des extensions Marina et Green Valley.\n";
    } else if (edition == "Custom") {
        mes_infos += "Nous avons créé cette édition pour vous permettre de jouer avec toutes les cartes existantes.\n";
        mes_infos += "Elle contient donc toutes les cartes des éditions standard, deluxe, marina et green valley.\n";
        mes_infos += "Mais ce n'est pas tout ! Elle contient également des cartes spéciales telles que : \n";
        mes_infos += " - le MGA game center\n";
        mes_infos += " - la fabrique du père noël\n";
    }
    if (extensions.size() > 0){
        mes_infos += "Extensions: ";
        for (auto it = extensions.begin(); it != extensions.end(); ++it){
            mes_infos += *it + " ";
        }
        mes_infos += "\n";
    }

    mes_infos += "\nJoueurs: \n";
    for (const auto & joueur : joueurs){
        mes_infos += "- "+ joueur.first + " : " + joueur.second + "\n";
    }

    mes_infos += "\nType de shop : " + shop_type;
    if (shop_type == "standard")
        mes_infos += "\nLe shop est composé de " + to_string(shop_size) + " cartes différentes.";
    menu->close();
    QMessageBox::information(menu, "Informations partie", QString::fromStdString(mes_infos));


    Partie *p = Partie::get_instance(edition, extensions, joueurs, shop_type, shop_size);
    //vp->show();
    p->jouer_partie();
}

void launch_menu_2(const string &edition_name, const list<string> &extensions){

    EditionDeJeu * edition;
    vector<EditionDeJeu *> listing_extension;

    edition = new EditionDeJeu(edition_name);

    for (const auto & extension : extensions){
        listing_extension.push_back(new EditionDeJeu(extension));
    }

    unsigned int max_joueurs = edition->get_nb_joueurs_max();
    unsigned int nb_monuments_win = edition->get_nb_monuments_win();
    for (const auto & extension : listing_extension){
        if (extension->get_nb_joueurs_max() > max_joueurs){
            max_joueurs = extension->get_nb_joueurs_max();
        }
        if (extension->get_nb_monuments_win() > nb_monuments_win){
            nb_monuments_win = extension->get_nb_monuments_win();
        }
    }

    auto list_batiments = edition->get_batiment();
    unsigned int nb_batiments_differents = list_batiments.size();


    for (const auto & extension : listing_extension){
        for (const auto & batiment : extension->get_batiment()){

            string nom = batiment.first->get_nom();
            bool found = false;

            for (const auto & batiment2 : list_batiments){
                if (batiment2.first->get_nom() == nom){
                    found = true;
                    break;
                }
            }

            if (!found){
                list_batiments[batiment.first] = batiment.second;
                nb_batiments_differents++;
            }
        }
    }

    delete edition;
    for (const auto & extension : listing_extension){
        delete extension;
    }

    auto *window = new QDialog();

    window->setWindowTitle("Machi Koro - Menu");
    window->setContentsMargins(50, 30, 50, 50);

    auto *label = new QLabel("Choisissez le nombre de joueurs :");

    auto *spinBox = new QSpinBox;
    spinBox->setRange(2, (int) max_joueurs);
    spinBox->setValue(2);

    auto *formLayout = new QFormLayout;
    for (int i = 0; i < spinBox->value(); i++) {
        auto *lineEdit = new QLineEdit;
        auto *comboBox = new QComboBox;
        comboBox->addItem("Humain");
        comboBox->addItem("IA agressive");
        comboBox->addItem("IA défensive");
        comboBox->addItem("IA aléatoire");
        comboBox->setCurrentIndex(0);

        auto *horizontalLayout = new QHBoxLayout;
        horizontalLayout->addWidget(lineEdit);
        horizontalLayout->addWidget(comboBox);
        formLayout->addRow("Joueur " + QString::number(i + 1) + " : ", horizontalLayout);
    }

    auto *layout = new QVBoxLayout;
    auto *h_layout = new QHBoxLayout;
    h_layout->addWidget(label);
    h_layout->addWidget(spinBox);
    layout->addLayout(h_layout);
    layout->addWidget(new QLabel());
    layout->addLayout(formLayout);
    layout->addWidget(new QLabel());

    auto *standardRadioButton = new QRadioButton("Standard");
    auto *extendedRadioButton = new QRadioButton("Extended");

    standardRadioButton->setChecked(true);

    auto *spinBox_shop = new QSpinBox;
    spinBox_shop->setRange(9, (int) nb_batiments_differents);
    spinBox_shop->setValue(9);

    auto *shop_layout = new QHBoxLayout;
    auto *shop_label = new QLabel("Choisissez le type du shop :");
    shop_layout->addWidget(shop_label);
    shop_layout->addWidget(standardRadioButton);
    shop_layout->addWidget(extendedRadioButton);
    layout->addLayout(shop_layout);

    auto *shop_layout2 = new QHBoxLayout;
    shop_layout2->addWidget(new QLabel("Nombre de cartes dans le shop :"));
    shop_layout2->addWidget(spinBox_shop);
    layout->addLayout(shop_layout2);

    layout->addWidget(new QLabel());

    auto *errorLabel = new QLabel();
    errorLabel->setStyleSheet("QLabel { color : red; }");
    layout->addWidget(errorLabel, 0, Qt::AlignCenter);

    layout->addWidget(new QLabel());
    auto *validateButton = new QPushButton("Jouer");
    layout->addWidget(validateButton);

    window->setLayout(layout);


    QObject::connect(standardRadioButton, &QRadioButton::clicked, [spinBox_shop](bool checked) {
        spinBox_shop->setEnabled(checked);
    });

    QObject::connect(extendedRadioButton, &QRadioButton::clicked, [spinBox_shop](bool checked) {
        spinBox_shop->setEnabled(!checked);
    });

    QObject::connect(spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [formLayout](int value) {

        while (formLayout->rowCount() > 0) {
            formLayout->removeRow(0);
        }

        for (int i = 0; i < value; i++) {
            auto *lineEdit = new QLineEdit;
            auto *comboBox = new QComboBox;
            comboBox->addItem("Humain");
            comboBox->addItem("IA agressive");
            comboBox->addItem("IA défensive");
            comboBox->addItem("IA aléatoire");
            comboBox->setCurrentIndex(0);

            auto *horizontalLayout = new QHBoxLayout;
            horizontalLayout->addWidget(lineEdit);
            horizontalLayout->addWidget(comboBox);
            formLayout->addRow("Joueur " + QString::number(i + 1) + " : ", horizontalLayout);
        }

    });

    QObject::connect(validateButton, &QPushButton::clicked, [window, formLayout, standardRadioButton, spinBox_shop, errorLabel, edition_name, extensions]() {

        map<string, string> joueurs;

        for (int i = 0; i < formLayout->rowCount(); i++) {
            auto *lineEdit = qobject_cast<QLineEdit *>(formLayout->itemAt(i, QFormLayout::FieldRole)->layout()->itemAt(0)->widget());
            auto *comboBox = qobject_cast<QComboBox *>(formLayout->itemAt(i, QFormLayout::FieldRole)->layout()->itemAt(1)->widget());
            if (lineEdit && comboBox) {
                QString value = lineEdit->text();
                QString type = comboBox->currentText();
                if (value.toStdString().empty()){
                    errorLabel->setText("Veuillez renseigner tous les noms de joueurs");
                    return;
                }
                if (joueurs.find(value.toStdString()) != joueurs.end()){
                    errorLabel->setText("Veuillez renseigner des noms de joueurs différents");
                    return;
                }
                joueurs[value.toStdString()] = type.toStdString();
            }
        }

        string shop;
        int nb_cartes;
        if (standardRadioButton->isChecked()){
            shop = "standard";
            nb_cartes = spinBox_shop->value();
        }
        else {
            shop = "extended";
            nb_cartes = 0;
        }

        validate_menu_2(window, edition_name, extensions, joueurs, shop, nb_cartes);

    });


    window->show();
}

void validate_menu_1(QWidget *menu, const string &edition, const list<string> &extensions){
    menu->close();

    launch_menu_2(edition, extensions);
}


void launch_menu_1(QApplication *app){

    auto *menu = new QWidget();
    resize_and_center(menu, 500, 180);


    menu->setContentsMargins(50, 30, 50, 50);
    auto *gridLayout = new QGridLayout;


    auto *editionLabel = new QLabel("Choix de l'édition :");
    auto* editionCombo = new QComboBox();
    auto *model = new QStandardItemModel();
    auto *item1 = new QStandardItem("-- Veuillez choisir une option --");
    auto *item2 = new QStandardItem("Standard");
    auto *item3 = new QStandardItem("Deluxe");
    auto *item4 = new QStandardItem("Custom");

    model->appendRow(item1);
    model->appendRow(item2);
    model->appendRow(item3);
    model->appendRow(item4);

    editionCombo->setCurrentIndex(0);
    editionCombo->setMinimumWidth(200);
    editionCombo->setStyle(QStyleFactory::create("Fusion"));

    item1->setFlags(item1->flags() & ~Qt::ItemIsEnabled);

    editionCombo->setModel(model);

    gridLayout->addWidget(editionLabel, 0, 0);
    gridLayout->addWidget(editionCombo, 0, 1);

    gridLayout->addItem(new QSpacerItem(0, 15), 1, 0);

    auto *extensionLabel = new QLabel("Choix des extension(s) :");
    gridLayout->addWidget(extensionLabel, 2, 0);

    auto *layout_h1 = new QHBoxLayout;
    auto *label_vide1 = new QLabel();
    label_vide1->setFixedWidth(5);
    auto *greenValleyCheck = new QCheckBox("Green valley");
    layout_h1->addWidget(label_vide1);
    layout_h1->addWidget(greenValleyCheck);

    auto *layout_h2 = new QHBoxLayout;
    auto *label_vide2 = new QLabel();
    label_vide2->setFixedWidth(5);
    auto *marinaCheck = new QCheckBox("Marina");
    layout_h2->addWidget(label_vide2);
    layout_h2->addWidget(marinaCheck);

    greenValleyCheck->setEnabled(true);
    marinaCheck->setEnabled(true);

    gridLayout->addLayout(layout_h1, 2, 1);
    gridLayout->addLayout(layout_h2, 3, 1);

    gridLayout->addItem(new QSpacerItem(0, 15), 4, 0);

    auto *validateButton = new QPushButton("Valider");
    validateButton->setFixedWidth(200);
    validateButton->setStyle(QStyleFactory::create("Fusion"));

    auto *cancelButton = new QPushButton("Annuler");
    cancelButton->setFixedWidth(200);
    cancelButton->setStyle(QStyleFactory::create("Fusion"));

    gridLayout->addWidget(validateButton, 5, 0, Qt::AlignCenter);
    gridLayout->addWidget(cancelButton, 5, 1, Qt::AlignCenter);

    QObject::connect(cancelButton, &QPushButton::clicked, app, &QApplication::quit);

    QObject::connect(editionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                     [greenValleyCheck, marinaCheck](int index) {
                         if (index == 1) { // Si l'index sélectionné est Standard
                             greenValleyCheck->setEnabled(true);
                             marinaCheck->setEnabled(true);
                         } else {
                             greenValleyCheck->setEnabled(false);
                             greenValleyCheck->setChecked(false);
                             marinaCheck->setEnabled(false);
                             marinaCheck->setChecked(false);
                         }
                     });

    // le bouton valider fait appel à la fonction validate_menu_1
    QObject::connect(validateButton, &QPushButton::clicked, [menu, editionCombo, greenValleyCheck, marinaCheck](){
        string edition = editionCombo->currentText().toStdString();
        list<string> extensions;
        if (greenValleyCheck->isChecked()){
            extensions.emplace_back("GreenValley");
        }
        if (marinaCheck->isChecked()){
            extensions.emplace_back("Marina");
        }
        validate_menu_1(menu, edition, extensions);
    });

    menu->setLayout(gridLayout);
    menu->setWindowTitle("Machi Koro - Menu");

    menu->show();
}

void build_content_jeu(QWidget *jeu){

    auto *main_layout = new QVBoxLayout(jeu);

    auto *layout_entete = new QHBoxLayout();
    auto label_entete = new QLabel("Ici entete");
    layout_entete->addWidget(label_entete);

    auto *layout_centre = new QHBoxLayout();
    auto *layout_centre_gauche = new QVBoxLayout(); // Pioche et affichages

    // build_content_centre_gauche(layout_centre_gauche);
    auto label_centre_gauche = new QLabel("Ici pioche");
    layout_centre_gauche->addWidget(label_centre_gauche);

    auto *layout_centre_droite = new QVBoxLayout(); // Shop
    auto label_centre_droite = new QLabel("Ici shop");
    // build_content_centre_droite(layout_centre_droite);
    layout_centre_droite->addWidget(label_centre_droite);
    layout_centre->addLayout(layout_centre_gauche);
    layout_centre->addLayout(layout_centre_droite);


    auto *layout_joueur = new QHBoxLayout();
    auto *layout_joueur_gauche = new QVBoxLayout(); // Infos joueur, monuments, argent, etc

    // build_content_joueur_gauche(layout_joueur_gauche);
    auto label_joueur_gauche = new QLabel("Ici infos joueur");
    layout_joueur_gauche->addWidget(label_joueur_gauche);

    auto *layout_joueur_droite = new QVBoxLayout(); // Batiments et bouton batiments fermes
    // build_content_joueur_droite(layout_joueur_droite);

    auto label_joueur_droite = new QLabel("Ici batiments");
    layout_joueur_droite->addWidget(label_joueur_droite);
    layout_joueur->addLayout(layout_joueur_gauche);
    layout_joueur->addLayout(layout_joueur_droite);

    main_layout->addLayout(layout_entete);
    main_layout->addLayout(layout_centre);
    main_layout->addLayout(layout_joueur);

    jeu->setLayout(main_layout);

    jeu->setWindowTitle("Machi Koro - Jeu");
}

int main(int argc, char * argv[]) {

    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create("Fusion"));
    launch_menu_1(&app);

    return QApplication::exec();
}