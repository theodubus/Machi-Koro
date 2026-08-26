#include <QRadioButton>
#include <QCoreApplication>
#include <QTime>
#include <QTimer>
#include "Partie.h"

// selectionner_joueur() construit une fenetre de choix. Ces en-tetes venaient
// jusqu'ici du <QtGui> global de l'ancienne VuePartie.h.
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "VuePartie.h"

Partie::Handler Partie::handler=Partie::Handler();


Partie* Partie::get_instance(const string &edition_name, const list<string> &extensions_names, const map<string, string>& joueurs, const string& shop_type, unsigned int shop_size) {
    if (handler.instance == nullptr) {
        EditionDeJeu * edition;
        vector<EditionDeJeu *> listing_extension;

        if (edition_name == "Standard" || edition_name == "Deluxe" || edition_name == "Custom") {
            edition = new EditionDeJeu(edition_name);
        }
        else {
            throw gameException("Edition inconnue");
        }

        // Les extensions etaient auparavant ignorees en silence pour les editions
        // Deluxe et Custom, alors qu'elles etaient bien transmises.
        for (const auto & extensions_name : extensions_names) {
            listing_extension.push_back(new EditionDeJeu(extensions_name));
        }

        handler.instance = new Partie(edition, joueurs, shop_type, shop_size, listing_extension);

        delete edition;

        for (auto & extension : listing_extension)
            delete extension;
    }

    return handler.instance;
}


Partie* Partie::get_instance() {
    if (handler.instance == nullptr) {
        throw gameException("Partie non initialisee");
    }
    return handler.instance;
}

Partie::Partie(EditionDeJeu* edition, const map<string, string>& joueurs, const string& shop_type, unsigned int shop_size, const vector<EditionDeJeu *>& extensions) : joueur_actuel(0), nb_monuments_win(edition->get_nb_monuments_win()), de_1(0), de_2(0), bonus_des(0), de_chalutier(0), compteur_tour(0), phase_courante(ScenePlateau::Phase::Attente) {
    ///Constructeur de Partie

    //Initialisation des variables utiles
    rejouer = false;

    tab_nom_edition.push_back(edition->get_nom());
    for (auto & extension : extensions)
        tab_nom_edition.push_back(extension->get_nom());

    for (auto bat : edition->get_batiment()) {
        for (unsigned int i = 0; i < bat.second; i++) {
            ajout_batiment(bat.first);
        }
    }

    for (auto monu : edition->get_monument()) {
        list_monuments.push_back(monu->clone());
    }

    if (!extensions.empty()) {
        for (auto ext : extensions) {
            for (auto bat : ext->get_batiment()) {
                for (unsigned int i = 0; i < bat.second; i++) {
                    ajout_batiment(bat.first);
                }
            }
            for (auto monu : ext->get_monument()) {
                list_monuments.push_back(monu->clone());
            }

            // Une extension peut relever la condition de victoire : Marina la porte
            // a 6 en apportant deux monuments constructibles de plus.
            if (ext->get_nb_monuments_win() > nb_monuments_win)
                nb_monuments_win = ext->get_nb_monuments_win();
        }
    }

    // Garde-fou : la condition ne peut pas depasser le nombre de monuments que le
    // joueur peut reellement batir, sinon la partie ne se termine jamais. Les
    // monuments offerts deja construits n'en font pas partie.
    unsigned int constructibles = 0;
    for (auto monu : list_monuments) {
        if (!Joueur::est_monument_de_depart(monu->get_nom()))
            constructibles++;
    }
    if (nb_monuments_win > constructibles)
        nb_monuments_win = constructibles;

    // Initialisation du starter
    vector<Batiment*> starter_bat = get_starter();

    // Ajout des joueurs
    for (auto & joueur : joueurs){

        if (joueur.second == "Humain") {
            tab_joueurs.push_back(new Joueur(joueur.first, list_monuments, starter_bat, 3));
        }
        else {
            if (joueur.second == "IA agressive") {
                tab_joueurs.push_back(new Joueur(joueur.first, list_monuments, starter_bat, 3, agressive));
            }
            else if (joueur.second == "IA défensive") {
                tab_joueurs.push_back(new Joueur(joueur.first, list_monuments, starter_bat, 3, defensif));
            }
            else if (joueur.second == "IA aléatoire") {
                tab_joueurs.push_back(new Joueur(joueur.first, list_monuments, starter_bat, 3, aleatoire));
            }
            else {
                throw gameException("Type de joueur inconnu");
            }
        }
    }


    random_device rd;
    mt19937 g(rd());

    shuffle(tab_joueurs.begin(), tab_joueurs.end(), g);

    pioche = new Pioche(map_to_vector(list_batiments));

    if (shop_type == "standard") {    // Construction du Shop
        shop = new Shop(shop_size);

        while (!pioche->est_vide() && shop->get_nb_tas_reel() < shop->get_nb_tas_max()){
            try  {
                shop->completer_shop(pioche->get_carte());
            }
            catch(exception const& e){
                cerr << "ERREUR : " << e.what() << endl;
            }
        }
    } else {
        shop = new Shop(list_batiments.size());

        while (!pioche->est_vide()){
            try  {
                shop->completer_shop(pioche->get_carte());
            }
            catch(exception const& e){
                cerr << "ERREUR : " << e.what() << endl;
            }
        }
    }
    vue_partie = nullptr;
}


vector<Batiment *> Partie::get_starter() {
    ///Renvoie le starter de la partie
    if (list_batiments.empty()) {
        throw gameException("Aucun batiment dans la liste");
    }
    vector<Batiment*> starter;

    for (auto bat : list_batiments) {
        if (bat.first->get_nom() == "Boulangerie" ||
            bat.first->get_nom() == "ChampBle")
            starter.push_back(bat.first);
        if (starter.size() == 2)
            break;
    }
    return starter;

}


Partie::~Partie() {
    /// Destructeur de la classe Partie
    // Destruction des joueurs
    for (auto joueur : tab_joueurs){
        delete joueur;
    }

    // Destruction des batiments
    for (auto batiment : list_batiments){
        delete batiment.first;
    }

    // Destruction des monuments
    for (auto monument : list_monuments){
        delete monument;
    }

    delete shop;
    delete pioche;
}

void Partie::rejouer_tour() {
    rejouer = true;
}


void Partie::ajout_batiment(Batiment *batiment) {
    ///Ajoute un batiment dans la liste des batiments
    bool est_present = false;

    for (auto bat : list_batiments){
        if (bat.first->get_nom() == batiment->get_nom()){
            list_batiments[bat.first] += 1;
            est_present = true;
        }
    }

    if (!est_present){
        list_batiments.insert(pair<Batiment* const, unsigned int>(batiment->clone(), 1));
    }
}


void Partie::acheter_carte_ia() {
    //fonction qui permet a un joueur donne d'acheter une carte (batiment ou monument)
    int choix_ia = -1;
    bool visit[2] = {false, false};
    bool transaction_fin = false;


    choix_ia = rand() % 5;
    if (choix_ia == 0) {
        visit[0] = true;
        transaction_fin = acheter_bat_ia();
    } else {
        visit[1] = true;
        transaction_fin = acheter_monu_ia();
    }

    if (!transaction_fin && !visit[0]) {
        visit[0] = true;
        unsigned int rand_ia = Partie::lancer_de() % 3;
        if (rand_ia != 0) {
            transaction_fin = acheter_bat_ia();
        }
    }
    else if (!transaction_fin && !visit[1]) {
        visit[1] = true;
        transaction_fin = acheter_monu_ia();
    }

    suite_tour(transaction_fin);
}

bool Partie::acheter_monu_ia() {
    //fonction qui permet a un joueur donne d'acheter un monument
    Monument* mon_picked;
    Joueur *joueur_act = tab_joueurs[joueur_actuel];
    int choix = -1;
    unsigned int pos = 1;
    vector<Monument*> monuments_dispo;


    for (auto mon_act: joueur_act->get_liste_monument()) {
        if (!mon_act.second && mon_act.first->get_prix() <= joueur_act->get_argent()) {
            monuments_dispo.push_back(mon_act.first);
        }
    }

    if (monuments_dispo.empty()) {
        return false;
    }

    mon_picked = monuments_dispo[rand() % monuments_dispo.size()];

    joueur_act->activer_monument(mon_picked);
    joueur_act->set_argent(joueur_act->get_argent() - mon_picked->get_prix());

    vue_partie->get_vue_infos()->add_info("Le joueur \"" + tab_joueurs[joueur_actuel]->get_nom() + "\" a active le monument " + mon_picked->get_nom_affiche() + "\n\n");
    return true;
}

bool Partie::acheter_bat_ia() {
    //fonction qui permet a un joueur donne d'acheter un batiment
    Batiment* bat_picked;
    Joueur *joueur_act = tab_joueurs[joueur_actuel];
    int choix = -1;
    vector<Batiment*> bat_shop = shop->get_contenu_v();


    strat_IA strat = tab_joueurs[joueur_actuel]->get_strategie();
    vector<Batiment*> bat_shop_couleur;
    vector<Batiment*> bat_shop_prix_ok;
    for (auto bat_act: bat_shop) {
        if (bat_act->get_prix() <= joueur_act->get_argent()) {
            bat_shop_prix_ok.push_back(bat_act);
        }
    }

    if (strat == agressive) {
        for (auto bat_act: bat_shop) {
            if (bat_act->get_couleur() == Rouge && bat_act->get_prix() <= joueur_act->get_argent()) {
                bat_shop_couleur.push_back(bat_act);
            }
        }
    }
    else if (strat == defensif) {
        for (auto bat_act: bat_shop) {
            if (bat_act->get_couleur() == Bleu && bat_act->get_prix() <= joueur_act->get_argent()) {
                bat_shop_couleur.push_back(bat_act);
            }
        }
    }

    if (bat_shop_couleur.empty() && bat_shop_prix_ok.empty()) {
        return false;
    }
    else if (bat_shop_couleur.empty()) {
        bat_picked = bat_shop_prix_ok[rand() % bat_shop_prix_ok.size()];
    }
    else {
        bat_picked = bat_shop_couleur[rand() % bat_shop_couleur.size()];
    }


    joueur_act->ajouter_batiment(bat_picked);

    try {
        shop->acheter_batiment(bat_picked);
    }
    catch(exception const& e){
        cerr << "ERREUR : " << e.what() << endl;
    }

    joueur_act->set_argent(joueur_act->get_argent() - bat_picked->get_prix());
    bat_picked->a_l_achat(joueur_actuel);

    vue_partie->get_vue_infos()->add_info("Le joueur \"" + tab_joueurs[joueur_actuel]->get_nom() + "\" a construit " + bat_picked->get_nom_affiche() + "\n\n");
    return true;
}


bool Partie::acheter_carte(const Carte* carte) {
    ///Fonction qui permet a un joueur d'acheter une carte (batiment ou monument)
    if (carte == nullptr) {
        return false;
    }
    if(!carte->est_monument()) {
        return acheter_bat(carte);
    }
    else {
        return acheter_monu(carte);
    }
}

bool Partie::acheter_monu(const Carte* carte) {
    //fonction qui permet a un joueur donne d'acheter un monument
    Monument* mon_picked = nullptr;
    Joueur *joueur_act = tab_joueurs[joueur_actuel];
    vector<Monument*> monuments_dispo;

    if (!tab_joueurs[joueur_actuel]->get_est_ia()) {
        for (auto mon_act: joueur_act->get_liste_monument()) {
            if (!mon_act.second && mon_act.first->get_nom() == carte->get_nom()) {
                mon_picked = mon_act.first;
            }
        }

        if (mon_picked == nullptr || mon_picked->get_prix() > joueur_act->get_argent()) {
            return false;
        }

        joueur_act->activer_monument(mon_picked);
        joueur_act->set_argent(joueur_act->get_argent() - mon_picked->get_prix());

    } else {
        for (auto mon_act: joueur_act->get_liste_monument()) {
            if (!mon_act.second && mon_act.first->get_prix() <= joueur_act->get_argent()) {
                monuments_dispo.push_back(mon_act.first);
            }
        }

        // Aucun monument constructible : l'achat echoue, il ne faut pas tirer au sort
        // dans un tableau vide.
        if (monuments_dispo.empty()) {
            return false;
        }

        mon_picked = monuments_dispo[rand() % monuments_dispo.size()];

        joueur_act->activer_monument(mon_picked);
        joueur_act->set_argent(joueur_act->get_argent() - mon_picked->get_prix());
    }

    vue_partie->get_vue_infos()->add_info("Le joueur \"" + tab_joueurs[joueur_actuel]->get_nom() + "\" a active le monument " + mon_picked->get_nom_affiche() + "\n\n");
    return true;
}

bool Partie::acheter_bat(const Carte* carte) {
    //fonction qui permet a un joueur donne d'acheter un batiment
    Batiment* bat_picked = nullptr;
    Joueur *joueur_act = tab_joueurs[joueur_actuel];
    vector<Batiment*> bat_shop = shop->get_contenu_v();

    // Recherche de la carte cliquee dans le shop
    for(auto& bat : bat_shop){
        if(bat->get_nom() == carte->get_nom()){
            bat_picked = bat;
        }
    }

    // La carte n'est plus disponible dans le shop : l'achat echoue.
    if (bat_picked == nullptr) {
        vue_partie->get_vue_infos()->add_info("Ce batiment n'est plus disponible dans le shop");
        return false;
    }

    if (bat_picked->get_prix() > joueur_act->get_argent()) {
        QMessageBox::information(vue_partie, "Achat impossible",
                                 "Vous n'avez pas assez d'argent pour acheter ce batiment");
        return false;
    }

    joueur_act->ajouter_batiment(bat_picked);

    try {
        shop->acheter_batiment(bat_picked);
    }
    catch(exception const& e){
        cerr << "ERREUR : " << e.what() << endl;
    }

    joueur_act->set_argent(joueur_act->get_argent() - bat_picked->get_prix());
    bat_picked->a_l_achat(joueur_actuel);

    vue_partie->get_vue_infos()->add_info("Le joueur \"" + tab_joueurs[joueur_actuel]->get_nom() + "\" a construit " + bat_picked->get_nom_affiche() + "\n\n");

    return true;
}


vector<Batiment*> Partie::map_to_vector(const map<Batiment*, unsigned int>& map_batiments){
    /// Retourne un vecteur avec l'adresse des batiments
    vector<Batiment*> vector_batiments;

    for (auto batiment : map_batiments){
        for(unsigned int i = 0; i < batiment.second; i++){
            vector_batiments.push_back(batiment.first);
        }
    }

    // Melange du vecteur
    // Obtention d'un nombre aleatoire (seed)
    random_device rd;
    mt19937 g(rd());

    // Melange du vecteur avec le seed
    shuffle(vector_batiments.begin(), vector_batiments.end(), g);

    return vector_batiments;
}

bool Partie::est_gagnant(unsigned int j) const {
    ///Fonction pour verifier si un joueur a gagne
    Joueur * joueur = tab_joueurs[j];
    return joueur->nb_monuments_construits() >= nb_monuments_win;
}

bool Partie::transfert_argent(unsigned int indice_joueur1, unsigned int indice_joueur2, unsigned int somme){
    //methode qui permet d'echanger une somme d'argent entre deux joueurs
    // Transfert de l'argent du joueur 1 au joueur 2

    Joueur *joueur1 = tab_joueurs[indice_joueur1];
    Joueur *joueur2 = tab_joueurs[indice_joueur2];
    if(somme == 0){
        return false;
    }

    if(joueur1->get_argent() < somme){
        unsigned int argent_joueur1 = joueur1->get_argent();
        joueur1->set_argent(0);
        joueur2->set_argent(joueur2->get_argent() + argent_joueur1);
    }
    else if(joueur1->get_argent() >= somme){
        joueur1->set_argent(joueur1->get_argent() - somme);
        joueur2->set_argent(joueur2->get_argent() + somme);
    }
    return true;
}

void Partie::jouer_partie() {
    /// Fonction pour jouer une partie
    // Création de la vue
    // Pas de QWidget racine : VuePartie est deja la fenetre de plus haut niveau,
    // et le widget qu'on lui donnait pour parent n'etait ni utilise ni libere.
    vue_partie = new VuePartie();
    vue_partie->setWindowState(Qt::WindowMaximized);
    vue_partie->show();

    jouer_tour();
}

bool Partie::activer_monument(const vector<Monument*>& monuments, const string& nom,
                              bool condition) {
    /// Voir Partie.h : l'ordre des monuments reste dans jouer_tour(), seule la
    /// recherche est factorisee ici.
    if (!condition) {
        return false;
    }
    auto it = find_if(monuments.begin(), monuments.end(),
                      [&nom](Monument* m) { return m->get_nom() == nom; });
    if (it == monuments.end()) {
        return false;
    }
    declencher(*it, {joueur_actuel}, {joueur_actuel, joueur_actuel});
    return true;
}

// ------------------------------------------------------------------- le rejeu

/// Duree pendant laquelle une carte reste allumee au rejeu. Assez longue pour
/// qu'on lise ce qu'elle fait, assez courte pour ne pas peser sur un tour d'IA.
static const int DELAI_EFFET = 620;
/// Duree d'un simple changement d'etape, quand aucune carte ne se declenche.
static const int DELAI_ETAPE = 260;

void Partie::declencher(const Carte* carte, const vector<unsigned int>& possesseurs,
                        const ContexteDeclenchement& ctx, unsigned int fois) {
    if (carte == nullptr || fois == 0) {
        return;
    }
    // On photographie les bourses avant et apres : c'est de la difference que se
    // deduisent les pieces qui traversent le plateau au rejeu. Aucune des
    // quarante-sept cartes n'a eu a etre modifiee pour cela.
    vector<unsigned int> avant;
    avant.reserve(tab_joueurs.size());
    for (const Joueur* j : tab_joueurs) avant.push_back(j->get_argent());

    for (unsigned int n = 0; n < fois; n++) {
        try {
            carte->declencher_effet(ctx);
        }
        catch (exception const& e) {
            cerr << "ERREUR : " << e.what() << endl;
        }
    }

    vector<int> deltas;
    deltas.reserve(tab_joueurs.size());
    for (size_t i = 0; i < tab_joueurs.size(); i++)
        deltas.push_back((int) tab_joueurs[i]->get_argent() - (int) avant[i]);

    // Une meme carte qui se declenche chez plusieurs joueurs d'affilee — un Champ
    // de ble sur le 1, que tout le monde possede — ne fait qu'un temps de rejeu :
    // elle s'allume chez tous a la fois. Les montrer un par un allongeait le tour
    // sans rien apprendre.
    if (!effets_du_tour.empty() && effets_du_tour.back().carte == carte &&
        effets_du_tour.back().phase == phase_courante) {
        Declenchement& d = effets_du_tour.back();
        for (unsigned int p : possesseurs) d.possesseurs.push_back(p);
        for (size_t i = 0; i < deltas.size() && i < d.deltas.size(); i++)
            d.deltas[i] += deltas[i];
        d.resume = resumer(d.deltas);
        return;
    }

    Declenchement d{carte, possesseurs, deltas, phase_courante, resumer(deltas)};
    effets_du_tour.push_back(d);
}

void Partie::marquer_etape(ScenePlateau::Phase p) {
    phase_courante = p;
    effets_du_tour.push_back({nullptr, {}, {}, p, ""});
}

string Partie::resumer(const vector<int>& deltas) const {
    vector<size_t> gagnants, perdants;
    for (size_t i = 0; i < deltas.size(); i++) {
        if (deltas[i] > 0) gagnants.push_back(i);
        else if (deltas[i] < 0) perdants.push_back(i);
    }
    // Certaines cartes ne deplacent pas d'argent : l'Entreprise de demenagement
    // echange un batiment, l'Entreprise de renovation en ferme. Leur propre texte
    // reste affiche sous la carte, il n'y a rien a resumer.
    if (gagnants.empty() && perdants.empty()) return "";

    auto nom = [&](size_t i) { return tab_joueurs[i]->get_nom(); };
    auto pieces = [](int n) {
        return to_string(n) + (n > 1 ? " pièces" : " pièce");
    };

    if (perdants.empty()) {
        string t;
        for (size_t g : gagnants)
            t += (t.empty() ? "" : ", ") + nom(g) + " reçoit " + pieces(deltas[g]);
        return t + " de la banque.";
    }
    if (gagnants.size() == 1) {
        string qui;
        for (size_t p : perdants) qui += (qui.empty() ? "" : " et ") + nom(p);
        return nom(gagnants[0]) + " prend " + pieces(deltas[gagnants[0]]) + " à " + qui + ".";
    }
    // Cas restants : une redistribution generale, comme l'Arboretum ou le Stade.
    string t;
    for (size_t i = 0; i < deltas.size(); i++) {
        if (deltas[i] == 0) continue;
        t += (t.empty() ? "" : "   ") + nom(i) +
             (deltas[i] > 0 ? " +" : " ") + to_string(deltas[i]);
    }
    return t;
}

void Partie::rejouer_effets(size_t i) {
    if (vue_partie == nullptr) return;

    if (i >= effets_du_tour.size()) {
        vue_partie->eteindre_projecteur();
        phase_achat();
        return;
    }

    const Declenchement& d = effets_du_tour[i];
    vue_partie->set_phase(d.phase);

    if (d.carte == nullptr) {
        // Changement d'etape. Si une carte se declenche dans la foulee, on
        // enchaine aussitot : la pause ne sert qu'a montrer qu'une etape n'a rien
        // produit. La carte de l'etape precedente s'eteint dans tous les cas,
        // sans quoi elle resterait allumee sous une etape qui n'est pas la sienne.
        const bool suite_immediate = (i + 1 < effets_du_tour.size() &&
                                      effets_du_tour[i + 1].phase == d.phase);
        if (!suite_immediate) vue_partie->eteindre_projecteur();
        QTimer::singleShot(suite_immediate ? 1 : DELAI_ETAPE,
                           [i]() { Partie::get_instance()->rejouer_effets(i + 1); });
        return;
    }

    vue_partie->projeter(d.carte, d.possesseurs, QString::fromStdString(d.resume));
    vue_partie->update_des();
    animer_transferts(d);

    QTimer::singleShot(DELAI_EFFET, [i]() { Partie::get_instance()->rejouer_effets(i + 1); });
}

void Partie::animer_transferts(const Declenchement& d) {
    // Chaque payeur alimente le gagnant le plus important, et un gain sans payeur
    // vient de la banque (source -1). On ne connait des cartes que la variation
    // des bourses : cet appariement est la lecture la plus simple qui rende
    // exactement les totaux.
    vector<pair<int,int>> gains, pertes;
    for (size_t k = 0; k < d.deltas.size(); k++) {
        if (d.deltas[k] > 0) gains.emplace_back((int) k, d.deltas[k]);
        else if (d.deltas[k] < 0) pertes.emplace_back((int) k, -d.deltas[k]);
    }
    for (auto& g : gains) {
        int reste = g.second;
        for (auto& p : pertes) {
            if (reste <= 0 || p.second <= 0) continue;
            const int part = min(reste, p.second);
            vue_partie->animer_piece(p.first, g.first, part);
            reste -= part;
            p.second -= part;
        }
        if (reste > 0) vue_partie->animer_piece(-1, g.first, reste);
    }
    for (auto& p : pertes)
        if (p.second > 0) vue_partie->animer_piece(p.first, -1, p.second);
}

void Partie::montrer_dernier_effet(size_t depuis) {
    if (vue_partie == nullptr || effets_du_tour.size() <= depuis) return;
    const Declenchement& d = effets_du_tour.back();
    if (d.carte == nullptr) return;
    vue_partie->set_phase(d.phase);
    vue_partie->projeter(d.carte, d.possesseurs, QString::fromStdString(d.resume));
    animer_transferts(d);
}


void Partie::jouer_tour() {
    /// ****************************************************************************************************************
    /// ****************************** ETAPE 1 : Variables + dés *******************************************************
    /// ****************************************************************************************************************
    unsigned int de_casse;
    bool centre_c_act = false;
    vector < Monument * > monuments_joueurs = tab_joueurs[joueur_actuel]->get_monument_jouables();

    compteur_tour++;
    /// Variable de tour
    moment_achat = false;
    rejouer = false;

    /// On update toute la vue
    vue_partie->update_vue_partie();

    /// Le journal des effets du tour precedent est rejoue, on repart a vide.
    effets_du_tour.clear();
    phase_courante = ScenePlateau::Phase::Des;
    vue_partie->set_phase(ScenePlateau::Phase::Des);

    /// Lancer des des
    de_1 = Partie::lancer_de();
    de_2 = 0;
    bonus_des = 0;

    // Le Chalutier fait lancer deux des une seule fois par tour, meme si un joueur en
    // possede plusieurs, et ce meme jet sert a tous les Chalutiers de la table. Il est
    // independant des des du tour : ni le Port ni les autres cartes ne l'affectent, et
    // il ne declenche aucun autre effet.
    de_chalutier = Partie::lancer_de() + Partie::lancer_de();

    // Simulation du « lance de des casse » de la Fabrique du Pere Noel : un de qui
    // bascule sur une arete n'existe pas en numerique, on tire donc quatre des a part
    // et on considere le lance casse s'ils totalisent 16, soit environ 9,6 % des
    // tours. Ce tirage n'entre dans aucun autre calcul.
    de_casse = Partie::lancer_de() + Partie::lancer_de() + Partie::lancer_de() + Partie::lancer_de();

    /// ****************************************************************************************************************
    /// ****************************** ETAPE 2 : Effets des monuments **************************************************
    /// ****************************************************************************************************************

    /// Centre commercial
    centre_c_act = activer_monument(monuments_joueurs, "CentreCommercial");

    /// Gare
    activer_monument(monuments_joueurs, "Gare");

    /// Fabrique du Pere Noel
    /// Un lance casse se constate au moment ou les des tombent : cet effet se resout
    /// donc juste apres le lance, une fois la Gare passee puisqu'elle decide combien
    /// de des sont lances, et avant que la Tour radio ne propose de relancer.
    activer_monument(monuments_joueurs, "FabriqueDuPereNoel", de_casse == 16);

    //une fois que tout les effets en rapport avec les dés sont joués, on update l'affichage des dés
    vue_partie->update_des();

    /// Tour radio
    activer_monument(monuments_joueurs, "TourRadio");


    vue_partie->update_des();

    /// Port
    activer_monument(monuments_joueurs, "Port", (de_1 + de_2) >= 10);

    vue_partie->update_des();

    /// ****************************************************************************************************************
    /// ****************************** ETAPE 3 : Effets des batiments **************************************************
    /// ****************************************************************************************************************


    /// Le livret est explicite sur l'ordre de resolution :
    /// « rouge en premier, bleu/vert ensuite et enfin violet ».

    /// Rouge
    unsigned int j_act_paiement = (joueur_actuel + tab_joueurs.size() - 1) % tab_joueurs.size();
    marquer_etape(ScenePlateau::Phase::Rouge);
    vue_partie->get_vue_infos()->add_info("Effet des batiments rouges");

    while (j_act_paiement != joueur_actuel) {
        // Le bonus du Centre commercial est propre au joueur encaisse : il doit etre
        // reevalue a chaque tour de boucle, sans quoi le premier possesseur rencontre
        // le distribuait a tous les joueurs suivants.
        bool centre_c_possesseur = false;
        vector < Monument * > monuments_j_act = tab_joueurs[j_act_paiement]->get_monument_jouables();
        for (auto mon: monuments_j_act) {
            if (mon->get_nom() == "CentreCommercial") {
                centre_c_possesseur = true;
            }
        }

        for (auto it: tab_joueurs[j_act_paiement]->get_liste_batiment(Rouge)) {
            if (find(it.first->get_num_activation().begin(), it.first->get_num_activation().end(), get_total_des()) !=
                it.first->get_num_activation().end()) {
                int bonus = (centre_c_possesseur && it.first->beneficie_centre_commercial()) ? 1 : 0;
                declencher(it.first, {j_act_paiement},
                           {j_act_paiement, joueur_actuel, bonus}, it.second);
            }
        }

        j_act_paiement = (j_act_paiement + tab_joueurs.size() - 1) % tab_joueurs.size();
    }

    /// Bleu
    marquer_etape(ScenePlateau::Phase::Bleu);
    vue_partie->get_vue_infos()->add_info("Effet des batiments bleus");
    for (unsigned int i = 0; i < tab_joueurs.size(); i++) {
        for (auto it: tab_joueurs[i]->get_liste_batiment(Bleu)) {
            if (find(it.first->get_num_activation().begin(), it.first->get_num_activation().end(), get_total_des()) !=
                it.first->get_num_activation().end()) {
                declencher(it.first, {i}, {i, joueur_actuel}, it.second);
            }
        }
    }

    /// Vert
    marquer_etape(ScenePlateau::Phase::Vert);
    vue_partie->get_vue_infos()->add_info("Effet des batiments verts");
    for (auto it: tab_joueurs[joueur_actuel]->get_liste_batiment(Vert)) {
        if (find(it.first->get_num_activation().begin(), it.first->get_num_activation().end(), get_total_des()) !=
            it.first->get_num_activation().end()) {
            int bonus = (centre_c_act && it.first->beneficie_centre_commercial()) ? 1 : 0;
            declencher(it.first, {joueur_actuel},
                       {joueur_actuel, joueur_actuel, bonus}, it.second);
        }
    }

    /// Violet (resolu en dernier)
    marquer_etape(ScenePlateau::Phase::Violet);
    vue_partie->get_vue_infos()->add_info("Effet des batiments violets");
    for (auto it: tab_joueurs[joueur_actuel]->get_liste_batiment(Violet)) {
        if (find(it.first->get_num_activation().begin(), it.first->get_num_activation().end(), get_total_des()) !=
            it.first->get_num_activation().end()) {
            declencher(it.first, {joueur_actuel}, {joueur_actuel, joueur_actuel}, it.second);
        }
    }

    /// ****************************************************************************************************************
    /// ****************************** ETAPE 4 : HotelDeVille + Achat **************************************************
    /// ****************************************************************************************************************

    /// Hotel de ville
    /// « Avant de construire un etablissement ou un monument... » : la carte se
    /// declenche a l'entree de la phase de construction, pas avec les violets.
    marquer_etape(ScenePlateau::Phase::Achat);
    activer_monument(monuments_joueurs, "HotelDeVille");

    /// Tout est resolu : on le montre, carte par carte, avant de rendre la main.
    rejouer_effets(0);
}

void Partie::phase_achat() {
    if (vue_partie == nullptr) {
        return;
    }
    phase_courante = ScenePlateau::Phase::Achat;
    vue_partie->update_vue_joueur();
    vue_partie->set_phase(ScenePlateau::Phase::Achat);

    if(!tab_joueurs[joueur_actuel]->get_est_ia()){
        moment_achat = true;
        vue_partie->set_bouton_rien_faire(true);
    }

    vue_partie->get_vue_infos()->add_info("Phase d'achat");

    if (tab_joueurs[joueur_actuel]->get_est_ia()) {
        /// On laisse deux secondes au joueur pour lire le tour de l'IA, puis on
        /// lance sa phase d'achat. Passer par une minuterie plutot que par une
        /// boucle d'attente active libere le processeur et, surtout, rend la main
        /// a la boucle d'evenements : la pile d'appels se vide entre deux tours.
        QTimer::singleShot(2000, []() { Partie::get_instance()->acheter_carte_ia(); });
    } else {
        vue_partie->get_vue_infos()->add_info("Cliquez une carte pour la lire, puis sur « Construire »");
        vue_partie->get_vue_infos()->add_info("Ou terminez votre tour avec « Ne rien construire »");
        vue_partie->get_vue_infos()->add_info("C'est a vous de jouer !");
    }
}

void Partie::suite_tour(bool achat_ok){
    vector < Monument * > monuments_joueurs = tab_joueurs[joueur_actuel]->get_monument_jouables();
    moment_achat = false;

    /// ****************************************************************************************************************
    /// ****************************** ETAPE 1 : Affichage achat + effet ***********************************************
    /// ****************************************************************************************************************

    const size_t effets_avant = effets_du_tour.size();

    if (!achat_ok) {
        vue_partie->get_vue_infos()->add_info("Vous n'avez rien acheté");

        /// Aeroport
        activer_monument(monuments_joueurs, "Aeroport");

    }

    /// ****************************************************************************************************************
    /// ****************************** ETAPE 2 : Ouverture batiment ****************************************************
    /// ****************************************************************************************************************

    /// Ouverture
    // Un etablissement ferme rouvre lorsque les des l'activent a nouveau, selon les
    // memes fenetres que ses revenus : les bleus pendant le tour de n'importe quel
    // joueur, les rouges pendant le tour des autres joueurs, les verts et violets
    // pendant le tour de leur proprietaire uniquement. La reouverture ne declenche
    // pas l'effet de la carte pour cette fois.
    vue_partie->get_vue_infos()->add_info("Ouverture des batiments");
    for (unsigned int j = 0; j < tab_joueurs.size(); j++) {
        for (auto bat : tab_joueurs[j]->get_liste_batiment_fermes()) {
            bool activable;
            switch (bat->get_couleur()) {
                case Bleu:   activable = true;                break;
                case Rouge:  activable = (j != joueur_actuel); break;
                default:     activable = (j == joueur_actuel); break;
            }
            if (!activable) continue;

            if (find(bat->get_num_activation().begin(), bat->get_num_activation().end(), get_total_des()) != bat->get_num_activation().end()) {
                try {
                    tab_joueurs[j]->ouvrir_batiment(bat);
                }
                catch(exception const& e){
                    cerr << "ERREUR : " << e.what() << endl;
                }
            }
        }
    }

    /// ParcAtraction
    activer_monument(monuments_joueurs, "ParcAttraction", de_1 == de_2);

    /// Complete le shop
    while (!pioche->est_vide() && shop->get_nb_tas_reel() < shop->get_nb_tas_max()) {
        try {
            shop->completer_shop(pioche->get_carte());
        }
        catch (exception const& e) {
            cerr << "ERREUR : " << e.what() << endl;
        }
    }
    de_1 = 0;
    de_2 = 0;
    bonus_des = 0;

    /// Update la vue
    vue_partie->set_bouton_rien_faire(false);
    vue_partie->update_vue_joueur();
    vue_partie->update_vue_pioche();
    vue_partie->update_des();
    vue_partie->update_vue_shop();
    vue_partie->update_vue_info();
    vue_partie->set_phase(ScenePlateau::Phase::Attente);
    vue_partie->get_vue_infos()->add_info("Fin du tour");

    // L'Aeroport et le Parc d'attractions se declenchent une fois le rejeu du
    // tour termine : on les montre ici, apres le rafraichissement qui aurait
    // efface leur mise en lumiere, et la seconde d'attente qui suit laisse le
    // temps de les lire.
    montrer_dernier_effet(effets_avant);

    /// On marque une seconde avant d'enchainer. Le passage par une minuterie evite
    /// que jouer_tour(), acheter_carte_ia() et suite_tour() ne s'appellent en
    /// cascade : la partie entiere s'empilait sur la pile d'appels, a raison de
    /// trois cadres par tour jamais depiles avant la fin de la partie.
    QTimer::singleShot(1000, []() { Partie::get_instance()->terminer_tour(); });
}

void Partie::terminer_tour() {
    // Les cartes qui posent un jeton « a la fin de votre tour » l'ont programme
    // pendant la phase de revenus : c'est ici qu'il se pose.
    tab_joueurs[joueur_actuel]->poser_jetons_en_attente();

    /// Vérifie si la partie est finie
    if (est_gagnant(joueur_actuel)) {
        /// Fin de la partie
        // La fenetre porte Qt::WA_DeleteOnClose : la fermer programme sa destruction.
        // On affiche donc le message AVANT de fermer, sinon on l'accroche a un parent
        // en cours de destruction, et on oublie le pointeur juste apres.
        VuePartie* fenetre_finie = vue_partie;
        QMessageBox::information(fenetre_finie, "Fin de la partie", "Le joueur " + QString::fromStdString(tab_joueurs[joueur_actuel]->get_nom()) + " a gagne !");
        vue_partie = nullptr;
        fenetre_finie->close();
    } else {
        /// Fin du tour
        if (!rejouer) {
            joueur_actuel = (joueur_actuel + 1) % tab_joueurs.size();
        }
        jouer_tour();
    }
}

unsigned int Partie::selectionner_joueur(const vector<Joueur*>& tab_joueurs, unsigned int joueur_actuel){
    unsigned int count = 0;
    unsigned int selection = -1;

    //cas où la decision doit se faire par une ia
    if(tab_joueurs[joueur_actuel]->get_est_ia()){
        selection = rand() % tab_joueurs.size();
        if(selection == joueur_actuel) selection = (selection + 1)%tab_joueurs.size();
    }
    //cas où c'est un joueur reel qui prend la decision
    else{
        // Fenetre de dialogue pour la selection
        while (selection == -1) {
            QDialog *window = new QDialog();
            window->setWindowTitle("Machi Koro - Selectionner un joueur");
            window->setContentsMargins(50, 30, 50, 50);

            vector<QPushButton *> liste_joueurs;
            QVBoxLayout *layout_joueurs = new QVBoxLayout;
            // Texte informatif
            QLabel *texte = new QLabel(QString::fromStdString(
                    tab_joueurs[joueur_actuel]->get_nom() + ", quel joueur veux tu sélectionner ?"));
            texte->setStyleSheet("QLabel { font-weight : bold; font-size : 25px; }");
            layout_joueurs->addWidget(texte);
            // Meme raison qu'au selecteur de batiment : la fenetre se repose tant
            // qu'aucun choix n'est fait, il faut le dire au joueur.
            QLabel *obligatoire = new QLabel("Ce choix est obligatoire : l'effet de la carte doit s'appliquer.");
            obligatoire->setStyleSheet("QLabel { color : #a0522d; font-style : italic; }");
            layout_joueurs->addWidget(obligatoire);

            int i = 0, ind_joueur = 0;
            for (auto &joueur: tab_joueurs) {
                // Ajout du joueur
                if (joueur != tab_joueurs[joueur_actuel]) {
                    // Ajout du bouton
                    QPushButton *bouton = new QPushButton;
                    bouton->setText(QString::fromStdString(joueur->get_nom()));
                    liste_joueurs.push_back(bouton);
                    layout_joueurs->addWidget(liste_joueurs[i]);
                    // Ajout du slot
                    QObject::connect(liste_joueurs[i], &QPushButton::clicked, [window, ind_joueur, &selection]() {
                        window->accept();
                        selection = ind_joueur;
                    });
                    i++;
                }
                ind_joueur++;
            }
            window->setLayout(layout_joueurs);
            // Affichage de la fenêtre
            window->exec();
            // Choix obligatoire, mais on ne garde pas la fenetre precedente en memoire.
            delete window;
        }
    }
    Partie::get_instance()->get_vue_partie()->get_vue_infos()->add_info("Joueur sélectionné : "+ tab_joueurs[selection]->get_nom());
    return selection;
}

unsigned int Partie::lancer_de() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 6);
    return dis(gen);
}
