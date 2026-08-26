#include <iostream>

/// QT Includes
#include <QApplication>
#include <QStyleFactory>
#include <QtGui/QScreen>
#include <QLineEdit>
#include <QRadioButton>
#include <QLayoutItem>
#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QStandardItem>
// Ces en-tetes arrivaient jusqu'ici par VuePartie.h, qui incluait <QtGui> en
// entier. Le menu s'en sert directement : il les demande desormais lui-meme.
#include <QLabel>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>


/// Project Includes
#include "Partie.h"
#include "StyleJeu.h"
#include <set>
using namespace std;

/// Bandeau de titre des menus. Les deux ecrans de reglage s'ouvraient sur une
/// liste deroulante, sans rien dire du jeu qu'on lance.
QVBoxLayout* entete_menu(const QString& sous_titre) {
    auto* bloc = new QVBoxLayout;
    auto* titre = new QLabel("Minivilles");
    titre->setObjectName("titre");
    titre->setAlignment(Qt::AlignCenter);
    bloc->addWidget(titre);
    auto* legende = new QLabel(sous_titre);
    legende->setObjectName("sous_titre");
    legende->setAlignment(Qt::AlignCenter);
    bloc->addWidget(legende);
    bloc->addSpacing(14);
    return bloc;
}

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

    window->setWindowTitle("Minivilles — réglages de la partie");
    window->setContentsMargins(50, 30, 50, 50);

    auto *label = new QLabel("Choisissez le nombre de joueurs :");

    auto *spinBox = new QSpinBox;
    spinBox->setRange(2, (int) max_joueurs);
    spinBox->setValue(2);

    auto *formLayout = new QFormLayout;
    for (int i = 0; i < spinBox->value(); i++) {
        auto *lineEdit = new QLineEdit;
        // Le champ etait vide et sans indication : rien ne disait qu'il fallait
        // le remplir avant que la validation ne refuse la partie.
        lineEdit->setPlaceholderText("Nom du joueur");
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
    layout->addLayout(entete_menu("Qui joue, et avec quelle boutique ?"));
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
            lineEdit->setPlaceholderText("Nom du joueur");
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
    menu->setObjectName("menu_minivilles");
    resize_and_center(menu, 580, 300);


    menu->setContentsMargins(50, 26, 50, 40);
    auto *pile = new QVBoxLayout(menu);
    pile->addLayout(entete_menu("Choisissez votre édition avant de lancer la partie"));
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

    editionCombo->setMinimumWidth(270);

    item1->setFlags(item1->flags() & ~Qt::ItemIsEnabled);

    // setModel remet l'index a zero : il faut donc le positionner apres, sinon
    // l'element d'invite reste selectionne.
    editionCombo->setModel(model);
    editionCombo->setCurrentIndex(0);

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

    auto *cancelButton = new QPushButton("Annuler");
    cancelButton->setFixedWidth(200);

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
        // L'element d'invite est desactive dans la liste, mais reste selectionne
        // tant que l'utilisateur n'a rien choisi : il ne designe aucune edition.
        if (editionCombo->currentIndex() <= 0) {
            QMessageBox::information(menu, "Choix de l'édition",
                                     "Veuillez choisir une édition avant de valider.");
            return;
        }
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

    pile->addLayout(gridLayout);
    menu->setWindowTitle("Minivilles");

    menu->show();
}

int main(int argc, char * argv[]) {

    QApplication app(argc, argv);
    // Le style est pose une fois pour toutes sur l'application, qui en prend la
    // propriete et le detruira. Tous les widgets le recuperent par defaut : les
    // appels setStyle() widget par widget construisaient un second objet Fusion,
    // au rendu identique et que personne ne liberait.
    app.setStyle(QStyleFactory::create("Fusion"));
    // Les menus et les fenetres de choix des cartes portaient chacun ses propres
    // couleurs, quand ils en avaient. Une seule feuille de style les accorde au
    // plateau, sans avoir a reprendre chaque widget.
    app.setStyleSheet(Style::feuille());
    launch_menu_1(&app);

    return QApplication::exec();
}