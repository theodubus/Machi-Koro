#include <QRadioButton>
#include <QCoreApplication>
#include <QTime>
#include "Partie.h"
#include "VuePartie.h"

Partie::Handler Partie::handler=Partie::Handler();


Partie* Partie::get_instance(const string &edition_name, const list<string> &extensions_names, const map<string, string>& joueurs, const string& shop_type, unsigned int shop_size) {
    if (handler.instance == nullptr) {
        EditionDeJeu * edition;
        vector<EditionDeJeu *> listing_extension;

        if (edition_name == "Standard") {
            edition = new EditionDeJeu("Standard");

            for (const auto & extensions_name : extensions_names) {
                listing_extension.push_back(new EditionDeJeu(extensions_name));
            }

        }
        else if (edition_name == "Deluxe") {
            edition = new EditionDeJeu("Deluxe");
        }
        else if (edition_name == "Custom") {
            edition = new EditionDeJeu("Custom");
        }
        else {
            throw gameException("Edition inconnue");
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

Partie::Partie(EditionDeJeu* edition, const map<string, string>& joueurs, const string& shop_type, unsigned int shop_size, const vector<EditionDeJeu *>& extensions) : nb_monuments_win(edition->get_nb_monuments_win()), joueur_actuel(0), de_1(0), de_2(0),compteur_tour(0) {
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

            if (ext->get_nb_monuments_win() > nb_monuments_win)
                nb_monuments_win = ext->get_nb_monuments_win();

        }
    }

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

    vue_partie->get_vue_infos()->add_info("Le joueur \"" + tab_joueurs[joueur_actuel]->get_nom() + "\" a active le monument " + mon_picked->get_nom() + "\n\n");
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
    if (bat_picked->get_nom() == "BanqueDeMinivilles") {
        joueur_act->set_argent(joueur_act->get_argent() + 5);
    }

    vue_partie->get_vue_infos()->add_info("Le joueur \"" + tab_joueurs[joueur_actuel]->get_nom() + "\" à acheté la carte " + bat_picked->get_nom() + "\n\n");
    return true;
}


void Partie::acheter_carte(VueCarte *vue_carte) {
    ///Fonction qui permet a un joueur d'acheter une carte (batiment ou monument)

    if(vue_carte->getCarte()->get_type() != "Monument") {
        acheter_bat(vue_carte);
    }
    else {
        acheter_monu(vue_carte);
    }
}

bool Partie::acheter_monu(VueCarte* vue_carte) {
    //fonction qui permet a un joueur donne d'acheter un monument
    Monument* mon_picked;
    Joueur *joueur_act = tab_joueurs[joueur_actuel];
    vector<Monument*> monuments_dispo;

    if (!tab_joueurs[joueur_actuel]->get_est_ia()) {
        for (auto mon_act: joueur_act->get_liste_monument()) {
            if (!mon_act.second && mon_act.first->get_nom() == vue_carte->getCarte()->get_nom()) {
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

        mon_picked = monuments_dispo[rand() % monuments_dispo.size()];

        joueur_act->activer_monument(mon_picked);
        joueur_act->set_argent(joueur_act->get_argent() - mon_picked->get_prix());
    }

    vue_partie->get_vue_infos()->add_info("Le joueur \"" + tab_joueurs[joueur_actuel]->get_nom() + "\" a active le monument " + mon_picked->get_nom() + "\n\n");
    return true;
}

bool Partie::acheter_bat(VueCarte* vue_carte) {
    //fonction qui permet a un joueur donne d'acheter un batiment
    Batiment* bat_picked;
    Joueur *joueur_act = tab_joueurs[joueur_actuel];
    vector<Batiment*> bat_shop = shop->get_contenu_v();

    // Fenetre de dialogue pour l'achat
    for(auto& bat : bat_shop){
        if(bat->get_nom() == vue_carte->getCarte()->get_nom()){
            bat_picked = bat;
        }
    }

    //bat_picked = bat_shop[choix - 1];
    if (bat_picked->get_prix() > joueur_act->get_argent()) {
        QWidget* pop_up = new QWidget();
        pop_up->setWindowTitle("Erreur");
        QLabel* label = new QLabel("Vous n'avez pas assez d'argent pour acheter ce batiment", pop_up);
        QPushButton* ok = new QPushButton("OK", pop_up);
        QObject::connect(ok, &QPushButton::clicked, pop_up, &QWidget::close);

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
    if (bat_picked->get_nom() == "BanqueDeMinivilles") {
        joueur_act->set_argent(joueur_act->get_argent() + 5);
    }

    vue_partie->get_vue_infos()->add_info("Le joueur \"" + tab_joueurs[joueur_actuel]->get_nom() + "\" a achete la carte " + bat_picked->get_nom() + "\n\n");

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
    return joueur->get_monument_jouables().size() >= nb_monuments_win;
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
    QWidget *fenetre = new QWidget;
    vue_partie = new VuePartie(fenetre);
    vue_partie->setWindowState(Qt::WindowMaximized);
    vue_partie->show();

    jouer_tour();
}

void Partie::jouer_tour() {
    /// ****************************************************************************************************************
    /// ****************************** ETAPE 1 : Variables + dés *******************************************************
    /// ****************************************************************************************************************
    unsigned int de_casse;
    unsigned int de_1_temp, de_2_temp;
    bool centre_c_act = false;
    bool centre_c_possesseur = false;
    vector < Monument * > monuments_joueurs = tab_joueurs[joueur_actuel]->get_monument_jouables();

    compteur_tour++;
    /// Variable de tour
    moment_achat = false;
    rejouer = false;

    /// On update toute la vue
    vue_partie->update_vue_partie();

    /// Lancer des des
    de_1 = Partie::lancer_de();
    de_1_temp = de_1;

    de_2 = 0;
    de_casse = Partie::lancer_de() + Partie::lancer_de() + Partie::lancer_de() + Partie::lancer_de();

    /// ****************************************************************************************************************
    /// ****************************** ETAPE 2 : Effets des monuments **************************************************
    /// ****************************************************************************************************************

    /// Centre commercial
    auto it_cc = find_if(monuments_joueurs.begin(), monuments_joueurs.end(),
                         [](Monument *m) { return m->get_nom() == "CentreCommercial"; });
    if (it_cc != monuments_joueurs.end()) {
        // Si le monument est trouve, on le joue
        try {
            monuments_joueurs[it_cc - monuments_joueurs.begin()]->declencher_effet(joueur_actuel);
        }
        catch (exception const &e) {
            cerr << "ERREUR : " << e.what() << endl;
        }
        centre_c_act = true;
    }

    /// Gare
    auto it_gare = find_if(monuments_joueurs.begin(), monuments_joueurs.end(),
                           [](Monument *m) { return m->get_nom() == "Gare"; });
    if (it_gare != monuments_joueurs.end()) {
        // Si le monument est trouve, on le joue
        try {
            monuments_joueurs[it_gare - monuments_joueurs.begin()]->declencher_effet(joueur_actuel);
        }
        catch (exception const &e) {
            cerr << "ERREUR : " << e.what() << endl;
        }
    }

    //une fois que tout les effets en rapport avec les dés sont joués, on update l'affichage des dés
    vue_partie->update_des();

    /// Tour radio
    auto it_tr = find_if(monuments_joueurs.begin(), monuments_joueurs.end(),
                         [](Monument *m) { return m->get_nom() == "TourRadio"; });
    if (it_tr != monuments_joueurs.end()) {
        // Si le monument est trouve, on le joue
        try {
            monuments_joueurs[it_tr - monuments_joueurs.begin()]->declencher_effet(joueur_actuel);
        }
        catch (exception const &e) {
            cerr << "ERREUR : " << e.what() << endl;
        }
    }


    vue_partie->update_des();

    de_1_temp = de_1;
    de_2_temp = de_2;

    /// Port + Fabrique du père noel
    for (auto mon: monuments_joueurs) {
        if (mon->get_nom() == "Port" && ((de_1 + de_2) >= 10) ||
            mon->get_nom() == "FabriqueDuPereNoel" && de_casse == 16) {
            try {
                mon->declencher_effet(joueur_actuel);
            }
            catch (exception const &e) {
                cerr << "ERREUR : " << e.what() << endl;
            }
        }
    }

    vue_partie->update_des();

    /// ****************************************************************************************************************
    /// ****************************** ETAPE 3 : Effets des batiments **************************************************
    /// ****************************************************************************************************************


    /// Rouge
    unsigned int j_act_paiement = (joueur_actuel + tab_joueurs.size() - 1) % tab_joueurs.size();
    vue_partie->get_vue_infos()->add_info("Effet des batiments rouges");

    while (j_act_paiement != joueur_actuel) {
        // Regarde si le joueur possede un centre commercial
        vector < Monument * > monuments_j_act = tab_joueurs[j_act_paiement]->get_monument_jouables();
        for (auto mon: monuments_j_act) {
            if (mon->get_nom() == "CentreCommercial") {
                centre_c_possesseur = true;
            }
        }

        for (auto it: tab_joueurs[j_act_paiement]->get_liste_batiment(Rouge)) {
            if (find(it.first->get_num_activation().begin(), it.first->get_num_activation().end(), de_1 + de_2) !=
                it.first->get_num_activation().end()) {
                if (it.first->get_type() == "restaurant" && centre_c_possesseur) {
                    for (unsigned int effectif = 0; effectif < it.second; effectif++) {
                        try {
                            it.first->declencher_effet(j_act_paiement, 1);
                        }
                        catch (exception const &e) {
                            cerr << "ERREUR : " << e.what() << endl;
                        }
                    }
                } else {
                    for (unsigned int effectif = 0; effectif < it.second; effectif++) {
                        try {
                            it.first->declencher_effet(j_act_paiement);
                        }
                        catch (exception const &e) {
                            cerr << "ERREUR : " << e.what() << endl;
                        }
                    }
                }
            }
        }

        j_act_paiement = (j_act_paiement + tab_joueurs.size() - 1) % tab_joueurs.size();
    }

    /// Violet
    vue_partie->get_vue_infos()->add_info("Effet des batiments violets");
    for (auto it: tab_joueurs[joueur_actuel]->get_liste_batiment(Violet)) {
        if (find(it.first->get_num_activation().begin(), it.first->get_num_activation().end(), de_1 + de_2) !=
            it.first->get_num_activation().end()) {
            for (unsigned int effectif = 0; effectif < it.second; effectif++) {
                try {
                    it.first->declencher_effet(joueur_actuel);
                }
                catch (exception const &e) {
                    cerr << "ERREUR : " << e.what() << endl;
                }
            }
        }
    }

    /// Bleu
    vue_partie->get_vue_infos()->add_info("Effet des batiments bleus");
    for (int i = 0; i < tab_joueurs.size(); i++) {
        for (auto it: tab_joueurs[i]->get_liste_batiment(Bleu)) {
            if (find(it.first->get_num_activation().begin(), it.first->get_num_activation().end(), de_1 + de_2) !=
                it.first->get_num_activation().end()) {
                for (unsigned int effectif = 0; effectif < it.second; effectif++) {
                    try {
                        it.first->declencher_effet(i);
                    }
                    catch (exception const &e) {
                        cerr << "ERREUR : " << e.what() << endl;
                    }
                }
            }
        }
    }

    /// Vert
    vue_partie->get_vue_infos()->add_info("Effet des batiments verts");
    for (auto it: tab_joueurs[joueur_actuel]->get_liste_batiment(Vert)) {
        if (find(it.first->get_num_activation().begin(), it.first->get_num_activation().end(), de_1 + de_2) !=
            it.first->get_num_activation().end()) {
            if (it.first->get_type() == "commerce" && centre_c_act) {
                for (unsigned int effectif = 0; effectif < it.second; effectif++) {
                    try {
                        it.first->declencher_effet(joueur_actuel, 1);
                    }
                    catch (exception const &e) {
                        cerr << "ERREUR : " << e.what() << endl;
                    }
                }
            } else {
                for (unsigned int effectif = 0; effectif < it.second; effectif++) {
                    try {
                        it.first->declencher_effet(joueur_actuel);
                    }
                    catch (exception const &e) {
                        cerr << "ERREUR : " << e.what() << endl;
                    }
                }
            }
        }
    }

    /// ****************************************************************************************************************
    /// ****************************** ETAPE 4 : HotelDeVille + Achat **************************************************
    /// ****************************************************************************************************************

    /// Hotel de ville
    auto it_hdv = find_if(monuments_joueurs.begin(), monuments_joueurs.end(),
                          [](Monument *m) { return m->get_nom() == "HotelDeVille"; });
    if (it_hdv != monuments_joueurs.end()) {
        // Si le monument est trouve, on le joue
        try {
            monuments_joueurs[it_hdv - monuments_joueurs.begin()]->declencher_effet(joueur_actuel);
        }
        catch (exception const &e) {
            cerr << "ERREUR : " << e.what() << endl;
        }
    }

    /// Achat
    if(!tab_joueurs[joueur_actuel]->get_est_ia()){
        moment_achat = true;
        vue_partie->set_bouton_rien_faire(true);
    }

    vue_partie->update_vue_joueur();
    vue_partie->get_vue_infos()->add_info("Phase d'achat");

    if (tab_joueurs[joueur_actuel]->get_est_ia()) {
        /// on fige l'interface graphique pour 2 secondes
        QTime dieTime = QTime::currentTime().addSecs(2);
        while (QTime::currentTime() < dieTime) {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
        /// on lance la phase d'achat de l'IA
        acheter_carte_ia();
    } else {
        vue_partie->get_vue_infos()->add_info("Pour acheter une carte, cliquez dessus puis sur le bouton 'Acheter'");
        vue_partie->get_vue_infos()->add_info("Pour passer votre tour, cliquez sur le bouton 'Ne rien faire' puis confirmez");
        vue_partie->get_vue_infos()->add_info("C'est à vous de jouer !");
    }
}

void Partie::suite_tour(bool achat_ok){
    vector < Monument * > monuments_joueurs = tab_joueurs[joueur_actuel]->get_monument_jouables();
    moment_achat = false;

    /// ****************************************************************************************************************
    /// ****************************** ETAPE 1 : Affichage achat + effet ***********************************************
    /// ****************************************************************************************************************

    if (!achat_ok) {
        vue_partie->get_vue_infos()->add_info("Vous n'avez rien acheté");

        /// Banque
        if (tab_joueurs[joueur_actuel]->get_argent() < 2)
            tab_joueurs[joueur_actuel]->set_argent(2);

        /// Aeroport
        auto it_earo = find_if(monuments_joueurs.begin(), monuments_joueurs.end(), [](Monument* m){return m->get_nom() == "Aeroport";});
        if (it_earo != monuments_joueurs.end()){
            // Si le monument est trouve, on le joue
            try{
                monuments_joueurs[it_earo - monuments_joueurs.begin()]->declencher_effet(joueur_actuel);
            }
            catch(exception const& e){
                cerr << "ERREUR : " << e.what() << endl;
            }
        }

    }

    /// ****************************************************************************************************************
    /// ****************************** ETAPE 2 : Ouverture batiment ****************************************************
    /// ****************************************************************************************************************

    /// Ouverture
    vue_partie->get_vue_infos()->add_info("Ouverture des batiments");
    for (auto bat : tab_joueurs[joueur_actuel]->get_liste_batiment_fermes()) {
        if (find(bat->get_num_activation().begin(), bat->get_num_activation().end(), de_1 + de_2) != bat->get_num_activation().end()) {
            try {
                tab_joueurs[joueur_actuel]->ouvrir_batiment(bat);
            }
            catch(exception const& e){
                cerr << "ERREUR : " << e.what() << endl;
            }
        }
    }

    /// ParcAtraction
    auto it_parc = find_if(monuments_joueurs.begin(), monuments_joueurs.end(), [](Monument* m){return m->get_nom() == "ParcAttraction";});
    if (it_parc != monuments_joueurs.end() && de_1 == de_2){
        // Si le monument est trouve, on le joue
        try{
            monuments_joueurs[it_parc - monuments_joueurs.begin()]->declencher_effet(joueur_actuel);
        }
        catch(exception const& e){
            cerr << "ERREUR : " << e.what() << endl;
        }
    }

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

    /// Update la vue
    vue_partie->set_bouton_rien_faire(false);
    vue_partie->update_vue_joueur();
    vue_partie->update_vue_pioche();
    vue_partie->update_des();
    vue_partie->update_vue_shop();
    vue_partie->update_vue_info();
    vue_partie->get_vue_infos()->add_info("Fin du tour");
    QTime endTime = QTime::currentTime().addSecs(1);

    // Figer l'affichage jusqu'à ce que l'heure actuelle soit supérieure à l'heure de fin
    while (QTime::currentTime() < endTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    if (vue_partie->get_vue_carte() != nullptr)
        vue_partie->get_vue_carte()->close();

    /// Vérifie si la partie est finie
    if (est_gagnant(joueur_actuel)) {
        /// Fin de la partie
        vue_partie->close();
        QMessageBox::information(vue_partie, "Fin de la partie", "Le joueur " + QString::fromStdString(tab_joueurs[joueur_actuel]->get_nom()) + " a gagne !");
//        QCoreApplication::quit();
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

void Partie::acheter_carte_event(VueCarte* vc) {
    acheter_carte(vc);
}