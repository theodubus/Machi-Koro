#include <QGridLayout>
#include <QDialog>
#include <QLabel>
#include "Joueur.h"
#include "VueCarte.h"
#include "Partie.h"

using namespace std;

Joueur::Joueur(const string& nom, const vector<Monument *>&list_mon, const vector<Batiment *>&list_bat, unsigned int arg_depart, strat_IA stratIa)
: strategie(stratIa), est_ia(stratIa != none), nom(nom), argent(arg_depart)
{
    /// Constructeur de joueur
    for (auto mon : list_mon)
        if (mon->get_nom() == "FabriqueDuPereNoel" || mon->get_nom() == "HotelDeVille")
            liste_monument[mon] = true;
        else
            liste_monument[mon] = false;
    for (auto bat : list_bat)
        liste_batiment[bat->get_couleur()][bat] = 1;
}

vector<unsigned int> Joueur::get_repartition_argent() const {
    /// Repartition de l'argent en fonction des types de pieces
    vector<unsigned int> repartition;
    unsigned int temp = argent;

    repartition.push_back(temp/10);
    temp = temp%10;
    repartition.push_back(temp/5);
    temp = temp%5;
    repartition.push_back(temp/1);

    return repartition;
}

Joueur::~Joueur() {
    /// Destructeur de joueur
    liste_monument.clear();
    liste_batiment_fermes.clear();

    // Iteration sur les couleurs de batiments
    for (auto & it : liste_batiment) {
        // Iteration sur les batiments
        it.second.clear();
    }

    liste_batiment.clear();
}

vector<Monument*> Joueur::get_monument_jouables() const {
    /// Retourne la liste des monuments actifs
    vector<Monument*> liste_monument_jouables;

    for (auto it : liste_monument) {
        if (it.second)
            liste_monument_jouables.push_back(it.first);
    }

    return liste_monument_jouables;
}


void Joueur::activer_monument(Monument *mon) {
    /// Activer un monument passe en parametre
    if (mon == nullptr) {
        throw gameException("Le monument ne peut pas etre nul");
    }

    // On active le monument
    auto it = liste_monument.find(mon);
    if (it != liste_monument.end()) {

        liste_monument[mon] = true;
    }
}

void Joueur::desactiver_monument(Monument *mon) {
    /// Desactiver un monument passe en parametre
    if (mon == nullptr) {
        throw gameException("Le monument ne peut pas etre nul");
    }

    // On active le monument
    auto it = liste_monument.find(mon);
    if (it != liste_monument.end()) {

        liste_monument[mon] = false;
    }
}

void Joueur::ajouter_batiment(Batiment *bat) {
    /// Ajout du batiment

    if (bat == nullptr) {
        throw gameException("Le batiment ne peut pas etre nul");
    }

    // Recuperation de la couleur du batiment
    couleur_bat couleur = bat->get_couleur();

    // On regarde si le joueur possede deja un exemplaire du batiment
    for (auto batiment : liste_batiment[couleur]){
        if (batiment.first == bat){
            // Si l'utilisateur a deja un exemplaire de ce batiment et qu'il est violet : erreur
            if (bat->get_couleur() == Violet){
                string message = "Le joueur \"" + nom + "\" possede deja un exemplaire du batiment violet " + bat->get_nom();
                Partie::get_instance()->get_vue_partie()->get_vue_infos()->add_info(message);
                return;
            }
            // Sinon, on augmente le nombre d'exemplaires de celui-ci
            else{
                liste_batiment[couleur][batiment.first]++;
                string message = "Le joueur \"" + nom + "\" a ajoute un exemplaire du batiment " + bat->get_nom()+ "\n" +
                        "Le joueur \"" + nom + "\" possede maintenant " + to_string( batiment.second) + " exemplaires du batiment " + bat->get_nom();
                Partie::get_instance()->get_vue_partie()->get_vue_infos()->add_info(message);
                return;
            }
        }
    }

    // Si aucun exemplaire du batiment, on l'ajoute
    liste_batiment[couleur].insert(pair<Batiment*, unsigned int> (bat, 1));
    string messsage_ajout = "Ajout du batiment " + bat->get_nom() + " au joueur " + nom + "\n" +
            "Le joueur \"" + nom + "\" possede 1 exemplaire du batiment " + bat->get_nom();
    Partie::get_instance()->get_vue_partie()->get_vue_infos()->add_info(messsage_ajout);
}


void Joueur::retirer_batiment(Batiment *bat) {
    /// Retire 1 exemplaire du batiment d'un joueur

    if (bat == nullptr) {
        throw gameException("Le batiment ne peut pas etre nul");
    }

    // Recuperation de la couleur du batiment
    couleur_bat couleur = bat->get_couleur();

    // On regarde si le joueur possede deja un exemplaire du batiment
    for (auto batiment : liste_batiment[couleur]){
        if (batiment.first == bat){
            // Si un seul exemplaire, on supprime l'element
            if (batiment.second == 1){
                liste_batiment[couleur].erase(batiment.first);
            }
            // Sinon, on decremente le nombre d'exemplaires
            else{
                liste_batiment[couleur][batiment.first]--;
            }
            string message = "Le batiment " + bat->get_nom() + " a ete retire du joueur \"" + nom + "\"";
            Partie::get_instance()->get_vue_partie()->get_vue_infos()->add_info(message);
            return;
        }
    }
    // Sinon, aucun batiment correspondant pour le joueur, on renvoie une erreur
    string message = "Impossible de retirer le batiment. Le joueur \"" + nom + "\" ne possede aucun batiment " + bat->get_nom();
    Partie::get_instance()->get_vue_partie()->get_vue_infos()->add_info(message);
}

unsigned int Joueur::count_type(const string& type) const {
    /// Compte le nombre de cartes d'un type donne
    unsigned int count = 0;
    auto liste_bat = liste_batiment;
    // pour chaque couleur de la liste de batiments du joueur
    for (const auto& couleur : liste_bat) {
        // pour chaque batiment de la couleur, (batiments sous forme de map (Batiment*, unsigned int))
        for (auto batiment : liste_bat[couleur.first]) {
            if (batiment.first->get_type() == type) {
                count += batiment.second;
            }
        }
    }
    return count;
}

Batiment* Joueur::possede_batiment(const string& nom_bat) const{
    // pour chaque couleur de la liste de batiments du joueur
    for (const auto& pair : liste_batiment) {
        // pour chaque batiment de la couleur, (batiments sous forme de map (Batiment*, unsigned int))
        for (auto batiment : pair.second) {
            // Si c'est l'element qu'on recherche, on le renvoie
            if (batiment.first->get_nom() == nom_bat) {
                return batiment.first;
            }
        }
    }
    // Sinon, retourne nullptr
    return nullptr;
}

Batiment* Joueur::selectionner_batiment() const {
    /// Fonction pour selectionner un batiment
    Batiment *bat_picked = nullptr;
    int i = 0;
    // On verifie que le joueur possede au moins un batiment selectionnable
    for (auto& couleur : get_liste_batiment()) {
        for (auto& bat: couleur.second) {
            i++;
        }
    }
    if (i == 0) return nullptr;
    // Selection du batiment
    if (! this->get_est_ia()){
        // Création d'une nouvelle fenetre
        while (bat_picked == nullptr){
            QDialog* window = new QDialog();
            window->setWindowTitle("Machi Koro - Selectionner un batiment");
            window->setContentsMargins(50, 30, 50, 50);

            vector<VueCarte*> liste_batiments;
            QVBoxLayout* layout = new QVBoxLayout;
            // Texte informatif
            QLabel *texte = new QLabel(QString::fromStdString("Quel batiment veux tu sélectionner dans le jeu de " + this->get_nom() + " ?"));
            texte->setStyleSheet("QLabel { font-weight : bold; font-size : 25px; }");
            layout->addWidget(texte);
            vector<VueCarte*> vue_batiments;
            QGridLayout* layout_batiments = new QGridLayout;
            i = 0;
            for (auto& couleur : get_liste_batiment()) {
                for (auto& bat: couleur.second) {
                    // Affichage du batiment
                    vue_batiments.push_back(new VueCarte(*bat.first, true, false, window));
                    layout_batiments->addWidget(vue_batiments[i], i / 4, i % 4);
                    QObject::connect(vue_batiments[i], &QPushButton::clicked, [window, bat, &bat_picked]() {
                        bat_picked = bat.first;
                        window->accept();
                    });
                    i++;
                }
            }
            // Execution de la fenetre
            layout->addLayout(layout_batiments);
            window->setLayout(layout);
            window->exec();
        }
    }
    // Si c'est une IA
    else{
        int compteur = 0;
        for (auto& couleur : get_liste_batiment()) {
            for (auto &bat: couleur.second) {
                compteur++;
            }
        }
        compteur = 0;
        // Choix de l'IA de manière aléatoire
        int choix = rand() % compteur;
        Batiment * bat_picked = nullptr;
        for (auto& couleur : get_liste_batiment()) {
            for (auto &bat: couleur.second) {
                if (compteur == choix) {
                    bat_picked = bat.first;
                }
                compteur++;
            }
        }
    }
    string message = "Le joueur " + this->get_nom() + " a selectionne le batiment " + bat_picked->get_nom();
    Partie::get_instance()->get_vue_partie()->get_vue_infos()->add_info(message);
    return bat_picked;
}



Monument* Joueur::possede_monument(const string& nom_mon) const{
    // pour chaque monument dans la liste de monuments du joueur
    for (auto mon : liste_monument) {
        if (mon.first->get_nom() == nom_mon) {
            return mon.first;
        }
    }
    // Sinon, retourne nullptr
    return nullptr;
}

void Joueur::fermer_batiment(Batiment *bat) {
    if (bat == nullptr){
        throw gameException("Le batiment entre n'est pas valide");
    }
    // On retire le batiment de la liste de batiments du joueur
    retirer_batiment(bat);

    // On ajoute le batiment a la liste de batiments fermes du joueur
    liste_batiment_fermes.push_back(bat);
}

void Joueur::ouvrir_batiment(Batiment *bat) {

    if (bat == nullptr){
        throw gameException("Le batiment entre n'est pas valide");
    }

    // on le cherche dans les batiments fermes pour avoir un iterateur
    auto it = find(liste_batiment_fermes.begin(), liste_batiment_fermes.end(), bat);

    // si on le trouve
    if (it != liste_batiment_fermes.end()){
        // on l'enleve de la liste des fermes
        liste_batiment_fermes.erase(it);
        // on l'ajoute a la liste des batiments du joueur
        ajouter_batiment(bat);
    }
    else{
        throw gameException("Le joueur n'a pas de batiment ferme correspondant a ce batiment");
    }
}

Monument *Joueur::selectionner_monument() const {
    /// Choisir un de ses monuments
    Monument* mon_picked = nullptr;
    vector<Monument*> monuments_jouables = this->get_monument_jouables();
    // Récupération des monuments jouables
    for (auto mon : monuments_jouables) {
        if (mon->get_nom() == "HotelDeVille" || mon->get_nom() == "FabriqueDuPereNoel") {
            auto it = find(monuments_jouables.begin(), monuments_jouables.end(), mon);
            if (it != monuments_jouables.end()) {
                monuments_jouables.erase(it);
            }
        }
    }
    // Gestion d'erreurs
    if (monuments_jouables.empty()) {
        Partie::get_instance()->get_vue_partie()->get_vue_infos()->add_info("Le joueur n'a pas de monument jouable.");
        return nullptr;
    }

    int choix = -1;
    if (this->get_est_ia()){
        choix = rand() % monuments_jouables.size();
        mon_picked = monuments_jouables[choix];
    }
    // Si joueur humain
    else {
        // Création d'une nouvelle fenetre
        while (mon_picked == nullptr){
            QDialog* window = new QDialog();
            window->setWindowTitle("Machi Koro - Selectionner un monument");
            window->setContentsMargins(50, 30, 50, 50);

            vector<VueCarte*> liste_monuments;
            QVBoxLayout* layout = new QVBoxLayout;
            // Texte informatif
            QLabel *texte = new QLabel(QString::fromStdString(this->get_nom() + ", quel batiment veux tu sélectionner ?"));
            texte->setStyleSheet("QLabel { font-weight : bold; font-size : 25px; }");
            layout->addWidget(texte);
            vector<VueCarte*> vue_monuments;
            QGridLayout* layout_monuments = new QGridLayout;
            int i = 0;
            for (auto& monument : monuments_jouables) {
                // Affichage du monument
                VueCarte* vue_mon = new VueCarte(*monument, true, true, window);
                vue_monuments.push_back(vue_mon);
                layout_monuments->addWidget(vue_monuments[i], i / 4, i % 4);
                // Connexion avec slot
                QObject::connect(vue_monuments[i], &QPushButton::clicked, [window, &mon_picked, &monument]()  {
                    window->accept();
                    mon_picked = monument;
                });
                i++;
            }
            // Execution de la fenetre
            layout->addLayout(layout_monuments);
            window->setLayout(layout);
            window->exec();
        }
    }
    // Retourne le monument choisi
    return mon_picked;
}
