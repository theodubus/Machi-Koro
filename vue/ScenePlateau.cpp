#include "ScenePlateau.h"
#include "ItemCarte.h"
#include "ItemBouton.h"
#include "Decor.h"
#include "Partie.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QGraphicsPathItem>
#include <QGraphicsEllipseItem>
#include <QPainterPath>
#include <QVariantAnimation>
#include <QFontMetrics>
#include <QtMath>
#include <algorithm>

namespace {

    // ------------------------------------------------------------------ zones
    //
    // Toute la mise en page tient dans ces huit rectangles, exprimes dans le
    // repere logique de la scene. Les changer suffit a deplacer un bloc entier :
    // c'est la difference avec l'ancienne interface, ou chaque widget portait sa
    // propre taille fixe en pixels et ou deplacer quoi que ce soit demandait de
    // reprendre toutes les dispositions imbriquees.
    const QRectF ZONE_RAIL       (  20,  12, 1180,  40);
    const QRectF ZONE_ENTETE     (1216,  12,  364,  40);
    const QRectF ZONE_ADVERSAIRES(  20,  58, 1180, 240);
    const QRectF ZONE_BOUTIQUE   (  20, 306, 1180, 236);
    const QRectF ZONE_ACTIF      (  20, 546, 1180, 330);
    const QRectF ZONE_JOURNAL    (1216,  58,  364, 268);
    const QRectF ZONE_AVANT      (1216, 334,  364, 482);
    const QRectF ZONE_BOUTONS    (1216, 824,  364,  56);

    const qreal ECART = 6;

    QFont police(int taille, bool gras = false) {
        QFont f;
        f.setPixelSize(taille);
        f.setBold(gras);
        return f;
    }

    /// « 1 piece », « 3 pieces », « gratuit » : deux cartes du jeu ne coutent rien.
    QString prix_en_toutes_lettres(unsigned int prix) {
        if (prix == 0) return "gratuit";
        return QString("%1 %2").arg(prix).arg(prix > 1 ? "pièces" : "pièce");
    }

    QPainterPath forme_panneau(const QRectF& r, qreal rayon) {
        QPainterPath p;
        p.addRoundedRect(r, rayon, rayon);
        return p;
    }

    /// Couleur d'une famille de cartes. Sert au rail des phases et aux liseres.
    QColor teinte_couleur(couleur_bat c) {
        switch (c) {
            case Bleu:   return QColor("#4d84ab");
            case Rouge:  return QColor("#b0505e");
            case Vert:   return QColor("#5f8a4e");
            case Violet: return QColor("#7a6191");
        }
        return QColor("#7b8a97");
    }

    /// Le plus petit numero qui active la carte, 99 si elle n'en a aucun. Sert a
    /// ranger une ville : quand les des tombent, on cherche un numero.
    unsigned int premier_numero(const Batiment* b) {
        const auto& nums = b->get_num_activation();
        if (nums.empty()) return 99;
        return *std::min_element(nums.begin(), nums.end());
    }

    /// Une carte d'une ville, telle qu'on veut la poser.
    struct Tuile {
        Batiment* batiment;
        unsigned int exemplaires;
        bool ferme;
    };

    /// Une grille de cartes : combien de colonnes, et de quelle largeur.
    struct Grille { int largeur; int colonnes; };

    /// La plus grande carte qui laisse tout tenir dans la zone, et la repartition
    /// en rangees qui va avec.
    ///
    /// Une ville va d'une carte a une trentaine, une boutique de neuf tas a
    /// trente-neuf : aucune taille fixe ne convient aux deux bouts. On essaie donc
    /// chaque nombre de rangees et on garde celui qui autorise les plus grandes
    /// cartes. Repartir en rangees egales plutot que remplir la premiere jusqu'au
    /// bord evite les mises en page a onze cartes puis une.
    ///
    /// Une carte fermee est couchee a 90 degres : elle occupe en largeur ce qu'une
    /// carte debout occupe en hauteur, soit une cellule et demie.
    Grille grille_qui_tient(int nb_cartes, int nb_couchees, const QRectF& zone, int largeur_max) {
        if (nb_cartes <= 0) return {largeur_max, 1};
        const qreal cellules = nb_cartes - nb_couchees + 1.55 * nb_couchees;
        Grille meilleure {16, nb_cartes};
        for (int rangees = 1; rangees <= 8; rangees++) {
            const int colonnes = (int) qCeil(cellules / rangees);
            if (colonnes < 1) continue;
            const qreal par_largeur = (zone.width() - (colonnes - 1) * ECART) / colonnes;
            const qreal par_hauteur = ((zone.height() - (rangees - 1) * ECART) / rangees) / 1.55;
            const int w = (int) std::min({(qreal) largeur_max, par_largeur, par_hauteur});
            if (w > meilleure.largeur) meilleure = {w, colonnes};
            // Au-dela d'une carte par rangee, ajouter des rangees ne sert plus.
            if (colonnes <= 1) break;
        }
        if (meilleure.largeur < 16) meilleure.largeur = 16;
        return meilleure;
    }
}

ScenePlateau::ScenePlateau(QObject* parent) : QGraphicsScene(parent) {
    setSceneRect(0, 0, LARGEUR, HAUTEUR);
    construire_fond();
    construire_rail();
    construire_entete();
    construire_journal();
    construire_panneau_avant();
}

// ---------------------------------------------------------------------- fond

void ScenePlateau::construire_fond() {
    fond = addPixmap(Decor::ciel(QSize(LARGEUR, HAUTEUR)));
    fond->setZValue(-200);

    // Le tapis couvre exactement la zone boutique, des et pioche compris : la
    // grille de cartes doit tomber dessus quel que soit le nombre de tas.
    const QRectF t = ZONE_BOUTIQUE.adjusted(-10, -10, 10, 10);
    le_tapis = addPixmap(Decor::tapis(t.size().toSize()));
    le_tapis->setPos(t.topLeft());
    le_tapis->setZValue(-190);
}

// ---------------------------------------------------------------------- rail

void ScenePlateau::construire_rail() {
    vider(items_rail);

    struct Etape { Phase p; const char* nom; QColor c; };
    const Etape etapes[6] = {
        { Phase::Des,    "Le jet",        Decor::Palette::or_piece() },
        { Phase::Rouge,  "Restaurants",   teinte_couleur(Rouge)  },
        { Phase::Bleu,   "Champs & mers", teinte_couleur(Bleu)   },
        { Phase::Vert,   "Commerces",     teinte_couleur(Vert)   },
        { Phase::Violet, "Majeurs",       teinte_couleur(Violet) },
        { Phase::Achat,  "Construction",  Decor::Palette::or_sombre() },
    };

    qreal x = ZONE_RAIL.left();
    const qreal y = ZONE_RAIL.top(), h = ZONE_RAIL.height();
    for (int i = 0; i < 6; i++) {
        const Etape& e = etapes[i];
        const bool active = (e.p == la_phase);
        // Les etapes deja passees restent marquees : on voit d'un coup d'oeil ou
        // l'on en est dans le tour, pas seulement ce qui se joue.
        const bool passee = (int) la_phase > (int) e.p && la_phase != Phase::Attente;

        const QFont f = police(active ? 16 : 14, active);
        QFontMetrics fm(f);
        const qreal l = fm.horizontalAdvance(e.nom) + (active ? 52 : 30);

        QColor fond_pastille = QColor(255, 255, 255, 110);
        if (passee) fond_pastille = QColor(e.c.red(), e.c.green(), e.c.blue(), 92);
        if (active) fond_pastille = e.c;

        auto* p = addPath(forme_panneau(QRectF(x, y, l, h), h / 2),
                          QPen(active ? QPen(QColor(255, 255, 255, 220), 2) : QPen(Qt::NoPen)),
                          QBrush(fond_pastille));
        p->setZValue(50);
        items_rail.append(p);

        if (active) {
            auto* pastille = addEllipse(x + 15, y + h / 2 - 6, 12, 12,
                                        QPen(Qt::NoPen), QBrush(QColor(255, 255, 255, 235)));
            pastille->setZValue(51);
            items_rail.append(pastille);
        } else if (passee) {
            auto* coche = addSimpleText("v", police(13, true));
            coche->setBrush(QColor(255, 255, 255, 220));
            coche->setPos(x + 11, y + h / 2 - 9);
            coche->setZValue(51);
            items_rail.append(coche);
        }

        auto* t = addSimpleText(e.nom, f);
        t->setBrush(active || passee ? QColor(255, 255, 255) : Decor::Palette::encre());
        t->setPos(x + (active ? 34 : 24), y + (h - fm.height()) / 2 + 1);
        t->setZValue(51);
        items_rail.append(t);

        x += l + 10;
        if (i < 5) {
            auto* trait = addRect(x - 8, y + h / 2 - 1.5, 5, 3,
                                  QPen(Qt::NoPen), QBrush(QColor(255, 255, 255, 160)));
            trait->setZValue(50);
            items_rail.append(trait);
        }
    }
}

void ScenePlateau::set_phase(Phase p) {
    if (la_phase == p) return;
    la_phase = p;
    construire_rail();
    appliquer_retrait();
}

// -------------------------------------------------------------------- entete

void ScenePlateau::construire_entete() {
    auto* p = addPath(forme_panneau(ZONE_ENTETE, 12), QPen(Qt::NoPen),
                      QBrush(QColor(253, 250, 243, 216)));
    p->setZValue(50);

    texte_edition = addSimpleText("", police(12, true));
    texte_edition->setBrush(Decor::Palette::encre());
    texte_edition->setPos(ZONE_ENTETE.left() + 14, ZONE_ENTETE.top() + 5);
    texte_edition->setZValue(51);

    texte_tour = addSimpleText("", police(12));
    texte_tour->setBrush(Decor::Palette::encre_pale());
    texte_tour->setPos(ZONE_ENTETE.left() + 14, ZONE_ENTETE.top() + 21);
    texte_tour->setZValue(51);
}

// ------------------------------------------------------------------- journal

void ScenePlateau::construire_journal() {
    auto* p = addPath(forme_panneau(ZONE_JOURNAL, 12),
                      QPen(QColor(255, 255, 255, 150), 2),
                      QBrush(QColor(253, 250, 243, 232)));
    p->setZValue(50);

    auto* titre = addSimpleText("Journal du tour", police(14, true));
    titre->setBrush(Decor::Palette::encre());
    titre->setPos(ZONE_JOURNAL.left() + 16, ZONE_JOURNAL.top() + 11);
    titre->setZValue(51);

    auto* trait = addRect(ZONE_JOURNAL.left() + 16, ZONE_JOURNAL.top() + 32,
                          ZONE_JOURNAL.width() - 32, 1.5,
                          QPen(Qt::NoPen), QBrush(QColor(0, 0, 0, 34)));
    trait->setZValue(51);

    texte_journal = addText("");
    texte_journal->setPos(ZONE_JOURNAL.left() + 14, ZONE_JOURNAL.top() + 38);
    texte_journal->setTextWidth(ZONE_JOURNAL.width() - 28);
    texte_journal->setDefaultTextColor(QColor("#55606b"));
    texte_journal->setFont(police(12));
    texte_journal->setZValue(51);
}

void ScenePlateau::journal(const std::string& texte) {
    QString ligne = QString::fromStdString(texte).trimmed();
    if (ligne.isEmpty()) return;
    ligne.replace('\n', ' ');
    lignes_journal.prepend(ligne);
    // Le journal ne garde que ce qui tient dans le panneau. Il n'y a pas de barre
    // de defilement dans une scene : au-dela, on oublie les plus anciennes lignes.
    while (lignes_journal.size() > 11) lignes_journal.removeLast();
    if (texte_journal) texte_journal->setPlainText(lignes_journal.join("\n"));
}

// ------------------------------------------------------- panneau de mise en avant

void ScenePlateau::construire_panneau_avant() {
    auto* p = addPath(forme_panneau(ZONE_AVANT, 12),
                      QPen(QColor(255, 255, 255, 150), 2),
                      QBrush(QColor(47, 59, 70, 226)));
    p->setZValue(50);

    titre_avant = addSimpleText("", police(14, true));
    titre_avant->setBrush(QColor(255, 255, 255, 235));
    titre_avant->setPos(ZONE_AVANT.left() + 16, ZONE_AVANT.top() + 11);
    titre_avant->setZValue(51);

    texte_avant = addText("");
    texte_avant->setTextWidth(ZONE_AVANT.width() - 28);
    texte_avant->setDefaultTextColor(QColor(255, 255, 255, 220));
    texte_avant->setFont(police(13));
    texte_avant->setZValue(53);

    bouton_acheter = new ItemBouton("Construire", QSizeF(ZONE_AVANT.width() - 32, 44));
    bouton_acheter->setPos(ZONE_AVANT.left() + 16, ZONE_AVANT.bottom() - 58);
    bouton_acheter->setZValue(52);
    bouton_acheter->set_teinte(Decor::Palette::or_sombre());
    bouton_acheter->setVisible(false);
    addItem(bouton_acheter);
    connect(bouton_acheter, &ItemBouton::clique, this, &ScenePlateau::bouton_acheter_clique);

    bouton_rien = new ItemBouton("Ne rien construire", QSizeF(ZONE_BOUTONS.width(), ZONE_BOUTONS.height()));
    bouton_rien->setPos(ZONE_BOUTONS.topLeft());
    bouton_rien->setZValue(52);
    bouton_rien->set_actif(false);
    addItem(bouton_rien);
    connect(bouton_rien, &ItemBouton::clique, this, &ScenePlateau::rien_faire);
}

void ScenePlateau::mettre_en_avant(const Carte* carte, const QString& explication,
                                   bool proposer_achat) {
    vider(items_avant);

    // Sans carte, le texte occupe tout le panneau ; avec une carte, il se range
    // sous elle — la carte mesure 200 x 310 et part de top + 40.
    texte_avant->setPos(ZONE_AVANT.left() + 14,
                        carte == nullptr ? ZONE_AVANT.top() + 44 : ZONE_AVANT.top() + 364);
    if (carte == nullptr) {
        titre_avant->setText(moment_achat ? "À vous de construire" : "En ce moment");
        texte_avant->setPlainText(
                moment_achat
                ? "Survolez une carte pour la lire, cliquez dessus pour la choisir.\n\n"
                  "Vous pouvez construire un établissement de la boutique ou l'un de "
                  "vos monuments — ou terminer votre tour sans rien construire."
                : "Survolez n'importe quelle carte du plateau pour la lire en grand.\n\n"
                  "Pendant la résolution, la carte qui produit son effet s'allume ici "
                  "et chez chacun des joueurs qu'elle concerne.");
        bouton_acheter->setVisible(false);
        return;
    }

    titre_avant->setText(QString::fromStdString(carte->get_nom_affiche()));

    // La carte, en grand : c'est le seul endroit ou on lit son texte sans avoir
    // a survoler quoi que ce soit.
    const int largeur = 200;
    auto* ic = new ItemCarte(carte, ItemCarte::Role::Detail, largeur);
    ic->setPos(ZONE_AVANT.center().x() - largeur / 2.0, ZONE_AVANT.top() + 40);
    ic->setZValue(51);
    if (carte->est_monument()) {
        Partie* partie = Partie::get_instance();
        const Joueur* j = partie->get_tab_joueurs()[partie->get_joueur_actuel()];
        ic->set_construit(j->monument_construit(carte->get_nom()));
    }
    addItem(ic);
    items_avant.append(ic);

    texte_avant->setPlainText(explication);
    bouton_acheter->setVisible(proposer_achat);
}

void ScenePlateau::eteindre_projecteur() {
    for (ItemCarte* ic : cartes_projetees) ic->projeter(false);
    cartes_projetees.clear();
    rendre_panneau();
}

void ScenePlateau::rendre_panneau() {
    // On rend le panneau a la carte que le joueur avait choisie, s'il y en a.
    if (carte_selectionnee == nullptr) {
        mettre_en_avant(nullptr, QString(), false);
        return;
    }
    mettre_en_avant(carte_selectionnee, decrire(carte_selectionnee), true);
    bouton_acheter->set_texte(QString("Construire — %1")
                                      .arg(prix_en_toutes_lettres(carte_selectionnee->get_prix())));
}

QString ScenePlateau::decrire(const Carte* carte) const {
    if (carte == nullptr) return QString();
    QString texte = QString::fromStdString(carte->get_description());
    if (carte->est_monument())
        return QString("Monument — %1\n\n%2").arg(prix_en_toutes_lettres(carte->get_prix())).arg(texte);

    const auto* bat = static_cast<const Batiment*>(carte);
    QStringList nums;
    for (unsigned int n : bat->get_num_activation()) nums << QString::number(n);
    return QString("%1 · s'active sur %2 · %3\n\n%4")
            .arg(QString::fromStdString(nom_type(bat->get_type())))
            .arg(nums.join(", ")).arg(prix_en_toutes_lettres(carte->get_prix())).arg(texte);
}

bool ScenePlateau::constructible(const Carte* carte, int proprietaire) const {
    Partie* partie = Partie::get_instance();
    const unsigned int actuel = partie->get_joueur_actuel();
    const Joueur* j = partie->get_tab_joueurs()[actuel];
    if (!moment_achat || j->get_est_ia() || carte == nullptr) return false;
    if (carte->get_prix() > j->get_argent()) return false;

    if (carte->est_monument()) {
        // Seulement ses propres monuments, pas encore batis. Les deux monuments
        // offerts au depart le sont deja et n'ont pas de face « en travaux ».
        return proprietaire == (int) actuel &&
               !Joueur::est_monument_de_depart(carte->get_nom()) &&
               !j->monument_construit(carte->get_nom());
    }
    // Un etablissement majeur ne se possede qu'en un exemplaire.
    const auto* bat = static_cast<const Batiment*>(carte);
    return proprietaire < 0 &&
           !(bat->get_couleur() == Violet && j->possede_batiment(bat->get_nom()) != nullptr);
}

void ScenePlateau::projeter(const Carte* carte, const std::vector<unsigned int>& possesseurs,
                            const QString& explication) {
    for (ItemCarte* ic : cartes_projetees) ic->projeter(false);
    cartes_projetees.clear();

    if (carte == nullptr) { eteindre_projecteur(); return; }

    // On allume la carte **chez chacun des joueurs cites**, pas une seule fois :
    // un Cafe qui se declenche prend une piece a plusieurs joueurs a la fois, et
    // c'est precisement ce qu'il faut montrer.
    const QList<QList<QGraphicsItem*>*> sources = { &items_adversaires, &items_actif };
    for (QList<QGraphicsItem*>* liste : sources) {
        for (QGraphicsItem* it : *liste) {
            auto* ic = dynamic_cast<ItemCarte*>(it);
            if (ic == nullptr || ic->carte() != carte) continue;
            if (!possesseurs.empty() &&
                std::find(possesseurs.begin(), possesseurs.end(),
                          (unsigned int) ic->proprietaire()) == possesseurs.end()) continue;
            ic->projeter(true);
            cartes_projetees.append(ic);
        }
    }

    // Certaines cartes ne deplacent pas d'argent — l'Entreprise de demenagement
    // echange un batiment, l'Entreprise de renovation en ferme : le controleur
    // n'a alors rien a resumer, et c'est le texte de la carte qui explique.
    mettre_en_avant(carte, explication.isEmpty() ? decrire(carte) : explication, false);
}

// ------------------------------------------------------------------- boutique

void ScenePlateau::poser_boutique() {
    vider(items_boutique);
    Partie* partie = Partie::get_instance();
    Shop* shop = partie->get_shop();
    if (shop == nullptr) return;

    const std::map<Batiment*, unsigned int>& piles = shop->get_contenu();
    const int n = (int) piles.size();

    // La boutique occupe la gauche du tapis ; les des et la pioche la droite.
    const QRectF titre_zone(ZONE_BOUTIQUE.left() + 14, ZONE_BOUTIQUE.top() + 2, 600, 22);
    const QRectF grille(ZONE_BOUTIQUE.left() + 14, ZONE_BOUTIQUE.top() + 28,
                        ZONE_BOUTIQUE.width() - 214, ZONE_BOUTIQUE.height() - 40);

    auto* titre = addSimpleText(QString("Boutique — %1 %2 à construire")
                                        .arg(n).arg(n > 1 ? "établissements" : "établissement"),
                                police(14, true));
    titre->setBrush(QColor(255, 255, 255, 235));
    titre->setPos(titre_zone.topLeft());
    titre->setZValue(12);
    items_boutique.append(titre);

    if (n == 0) return;

    const Joueur* actuel = partie->get_tab_joueurs()[partie->get_joueur_actuel()];
    const unsigned int bourse = actuel->get_argent();

    const Grille g = grille_qui_tient(n, 0, grille, 118);
    const int largeur = g.largeur;
    const qreal hauteur = largeur * 1.55;

    // La grille est centree sur le tapis : alignee a gauche, elle laissait une
    // large bande verte vide des que la boutique tenait sur une rangee.
    const int rangees = (n + g.colonnes - 1) / g.colonnes;
    const qreal marge_x = (grille.width() - (g.colonnes * largeur + (g.colonnes - 1) * ECART)) / 2;
    const qreal marge_y = (grille.height() - (rangees * hauteur + (rangees - 1) * ECART)) / 2;
    const qreal x0 = grille.left() + std::max(0.0, marge_x);
    qreal x = x0, y = grille.top() + std::max(0.0, marge_y);
    int i = 0;
    for (const auto& pile : piles) {
        Batiment* bat = pile.first;
        if (i > 0 && i % g.colonnes == 0) {
            x = x0;
            y += hauteur + ECART;
        }
        auto* item = new ItemCarte(bat, ItemCarte::Role::Boutique, largeur);
        item->set_exemplaires(pile.second);
        // Un etablissement majeur deja possede ne peut pas etre rachete : le
        // livret n'autorise qu'un exemplaire de chaque carte violette par ville.
        const bool deja = bat->get_couleur() == Violet &&
                          actuel->possede_batiment(bat->get_nom()) != nullptr;
        item->set_indisponible(bat->get_prix() > bourse || deja);
        item->setPos(x, y);
        item->set_pose(((i * 37) % 7 - 3) * 0.45);
        item->setZValue(10);
        connect(item, &ItemCarte::cliquee, this, &ScenePlateau::carte_cliquee);
        connect(item, &ItemCarte::survolee, this, &ScenePlateau::carte_survolee);
        addItem(item);
        items_boutique.append(item);
        x += largeur + ECART;
        i++;
    }
}

// ------------------------------------------------------------ des et pioche

void ScenePlateau::poser_des() {
    vider(items_des);
    Partie* partie = Partie::get_instance();

    const QRectF socle(ZONE_BOUTIQUE.right() - 190, ZONE_BOUTIQUE.top() + 12, 176, 118);
    auto* p = addPath(forme_panneau(socle, 14), QPen(QColor(255, 255, 255, 120), 2),
                      QBrush(QColor(253, 250, 243, 238)));
    p->setZValue(40);
    items_des.append(p);

    const unsigned int d1 = partie->get_de_1();
    const unsigned int d2 = partie->get_de_2();
    const unsigned int total = partie->get_total_des();

    if (d1 == 0) {
        auto* attente = addSimpleText("Dés non lancés", police(13));
        attente->setBrush(Decor::Palette::encre_pale());
        attente->setPos(socle.center().x() - 48, socle.center().y() - 8);
        attente->setZValue(41);
        items_des.append(attente);
    } else {
        auto dessiner_de = [&](qreal x, qreal y, unsigned int valeur) {
            const qreal c = 50;
            auto* d = addPath(forme_panneau(QRectF(x, y, c, c), 10),
                              QPen(QColor("#ded7c8"), 2), QBrush(Qt::white));
            d->setZValue(41);
            items_des.append(d);
            // Les points, dans la disposition d'un vrai de.
            static const int grille[7][9] = {
                {0,0,0,0,0,0,0,0,0}, {0,0,0,0,1,0,0,0,0}, {1,0,0,0,0,0,0,0,1},
                {1,0,0,0,1,0,0,0,1}, {1,0,1,0,0,0,1,0,1}, {1,0,1,0,1,0,1,0,1},
                {1,0,1,1,0,1,1,0,1},
            };
            if (valeur > 6) return;
            for (int i = 0; i < 9; i++) {
                if (!grille[valeur][i]) continue;
                const qreal px = x + c * (0.24 + (i % 3) * 0.26);
                const qreal py = y + c * (0.24 + (i / 3) * 0.26);
                auto* pt = addEllipse(px - 4, py - 4, 8, 8,
                                      QPen(Qt::NoPen), QBrush(Decor::Palette::encre()));
                pt->setZValue(42);
                items_des.append(pt);
            }
        };

        // Un seul de se centre ; deux des s'ecartent de part et d'autre du centre.
        if (d2 == 0) {
            dessiner_de(socle.center().x() - 25, socle.top() + 14, d1);
        } else {
            dessiner_de(socle.center().x() - 54, socle.top() + 14, d1);
            dessiner_de(socle.center().x() + 4, socle.top() + 14, d2);
        }

        // Le total, en gros : c'est lui qui active les etablissements. Le bonus du
        // Port s'ajoute au total et non aux des, il faut donc l'annoncer a part
        // sinon l'ecart entre les faces et le total ne s'explique pas.
        auto* t = addSimpleText(QString::number(total), police(34, true));
        t->setBrush(Decor::Palette::or_sombre());
        QFontMetrics fm(police(34, true));
        t->setPos(socle.center().x() - fm.horizontalAdvance(QString::number(total)) / 2.0,
                  socle.top() + 68);
        t->setZValue(42);
        items_des.append(t);

        const QString mention = (total != d1 + d2) ? "total (Port : +2)" : "total";
        auto* m = addSimpleText(mention, police(11));
        m->setBrush(Decor::Palette::encre_pale());
        QFontMetrics fm2(police(11));
        m->setPos(socle.center().x() - fm2.horizontalAdvance(mention) / 2.0, socle.bottom() - 20);
        m->setZValue(42);
        items_des.append(m);
    }

    // --- La pioche, sous les des ---
    Pioche* pioche = partie->get_pioche();
    const QRectF sp(socle.left(), socle.bottom() + 12, socle.width(), 118);
    auto* fond_pioche = addPath(forme_panneau(sp, 14), QPen(QColor(255, 255, 255, 120), 2),
                                QBrush(QColor(253, 250, 243, 238)));
    fond_pioche->setZValue(40);
    items_des.append(fond_pioche);

    if (pioche != nullptr && !pioche->est_vide()) {
        QPixmap dos("../assets/batiments/BACK-cartes.png");
        if (!dos.isNull()) {
            dos = dos.scaledToHeight(84, Qt::SmoothTransformation);
            auto* d = addPixmap(dos);
            d->setPos(sp.left() + 14, sp.top() + 14);
            d->setZValue(41);
            items_des.append(d);
        }
        auto* n = addSimpleText(QString::number(pioche->get_taille()), police(26, true));
        n->setBrush(Decor::Palette::encre());
        n->setPos(sp.left() + 84, sp.top() + 30);
        n->setZValue(41);
        items_des.append(n);
        auto* l = addSimpleText("en pioche", police(11));
        l->setBrush(Decor::Palette::encre_pale());
        l->setPos(sp.left() + 84, sp.top() + 64);
        l->setZValue(41);
        items_des.append(l);
    } else {
        auto* v = addSimpleText("Pioche vide", police(13));
        v->setBrush(Decor::Palette::encre_pale());
        v->setPos(sp.center().x() - 38, sp.center().y() - 8);
        v->setZValue(41);
        items_des.append(v);
    }
}

void ScenePlateau::montrer_des() {
    poser_des();
}

// ------------------------------------------------------------------- villes

qreal ScenePlateau::poser_cartes(const QRectF& zone, unsigned int indice_joueur,
                                 QList<QGraphicsItem*>& sortie, int largeur_max) {
    Partie* partie = Partie::get_instance();
    const Joueur* j = partie->get_tab_joueurs()[indice_joueur];

    std::vector<Tuile> tuiles;
    for (const auto& couleur : j->get_liste_batiment())
        for (const auto& b : couleur.second)
            tuiles.push_back({b.first, b.second, false});
    for (Batiment* f : j->get_liste_batiment_fermes())
        tuiles.push_back({f, 1, true});

    // Rangees par numero d'activation : quand les des tombent, c'est un numero
    // qu'on cherche des yeux, pas un nom.
    std::sort(tuiles.begin(), tuiles.end(), [](const Tuile& a, const Tuile& b) {
        const unsigned int na = premier_numero(a.batiment), nb = premier_numero(b.batiment);
        if (na != nb) return na < nb;
        return a.batiment->get_nom() < b.batiment->get_nom();
    });

    if (tuiles.empty()) {
        auto* vide = addSimpleText("Aucun établissement", police(12));
        vide->setBrush(Decor::Palette::encre_pale());
        vide->setPos(zone.left(), zone.top() + 6);
        vide->setZValue(34);
        sortie.append(vide);
        return 20;
    }

    int nb_couchees = 0;
    for (const Tuile& t : tuiles) if (t.ferme) nb_couchees++;

    const Grille g = grille_qui_tient((int) tuiles.size(), nb_couchees, zone, largeur_max);
    const int largeur = g.largeur;
    const qreal hauteur = largeur * 1.55;

    qreal x = zone.left(), y = zone.top();
    for (const Tuile& t : tuiles) {
        const qreal avance = t.ferme ? hauteur : largeur;
        // Une carte couchee deborde de sa cellule : on passe a la ligne des
        // qu'elle ne rentre plus, sans attendre la fin de la rangee theorique.
        if (x > zone.left() && x + avance > zone.right()) {
            x = zone.left();
            y += hauteur + ECART;
        }
        auto* ic = new ItemCarte(t.batiment, ItemCarte::Role::Ville, largeur);
        ic->set_exemplaires(t.exemplaires);
        ic->set_ferme(t.ferme);
        ic->set_jetons(j->get_jetons(t.batiment->get_nom()));
        ic->set_proprietaire((int) indice_joueur);
        // Une carte couchee pivote autour de son centre : on la decale d'un demi
        // ecart de hauteur pour que son empreinte reste dans la cellule.
        ic->setPos(x + (t.ferme ? (hauteur - largeur) / 2 : 0), y);
        ic->setZValue(30);
        connect(ic, &ItemCarte::cliquee, this, &ScenePlateau::carte_cliquee);
        connect(ic, &ItemCarte::survolee, this, &ScenePlateau::carte_survolee);
        addItem(ic);
        sortie.append(ic);
        x += avance + ECART;
    }
    return y + hauteur - zone.top();
}

void ScenePlateau::poser_adversaires() {
    vider(items_adversaires);
    Partie* partie = Partie::get_instance();
    const std::vector<Joueur*>& joueurs = partie->get_tab_joueurs();
    const unsigned int nb = joueurs.size();
    const unsigned int actuel = partie->get_joueur_actuel();
    if (nb < 2) return;

    const int n = (int) nb - 1;
    const qreal larg = (ZONE_ADVERSAIRES.width() - (n - 1) * 12) / n;
    qreal x = ZONE_ADVERSAIRES.left();

    // Dans l'ordre du tour a partir du suivant : on lit de gauche a droite qui
    // joue apres, ce qui compte pour les cartes rouges — elles ne rapportent que
    // pendant le tour des autres.
    for (unsigned int k = 1; k < nb; k++) {
        const unsigned int i = (actuel + k) % nb;
        const Joueur* j = joueurs[i];
        const QRectF cadre(x, ZONE_ADVERSAIRES.top(), larg, ZONE_ADVERSAIRES.height());

        auto* panneau = addPath(forme_panneau(cadre, 14),
                                QPen(QColor(255, 255, 255, 150), 2),
                                QBrush(QColor(253, 250, 243, 226)));
        panneau->setZValue(28);
        items_adversaires.append(panneau);

        // Un bandeau a la couleur du joueur : c'est ce qui permet de suivre « qui
        // paie qui » quand une piece traverse le plateau.
        const qreal hb = 34;
        QPainterPath bandeau;
        bandeau.addRoundedRect(QRectF(cadre.left(), cadre.top(), cadre.width(), hb), 14, 14);
        bandeau.addRect(QRectF(cadre.left(), cadre.top() + hb - 14, cadre.width(), 14));
        auto* b = addPath(bandeau.simplified(), QPen(Qt::NoPen),
                          QBrush(Decor::Palette::joueur(i)));
        b->setZValue(29);
        items_adversaires.append(b);

        const int ta = 46;
        auto* av = addPixmap(Decor::avatar(i, ta));
        av->setPos(cadre.left() + 8, cadre.top() - 8);
        av->setZValue(32);
        items_adversaires.append(av);

        auto* nom = addSimpleText(QString::fromStdString(j->get_nom()), police(15, true));
        nom->setBrush(Qt::white);
        nom->setPos(cadre.left() + 8 + ta + 8, cadre.top() + 8);
        nom->setZValue(31);
        items_adversaires.append(nom);

        // --- Bourse a gauche, progression vers la victoire a droite ---
        auto* piece = addPixmap(Decor::jeton(22));
        piece->setPos(cadre.left() + 12, cadre.top() + hb + 8);
        piece->setZValue(31);
        items_adversaires.append(piece);

        auto* sous = addSimpleText(QString::number(j->get_argent()), police(19, true));
        sous->setBrush(Decor::Palette::encre());
        sous->setPos(cadre.left() + 40, cadre.top() + hb + 6);
        sous->setZValue(31);
        items_adversaires.append(sous);

        const QString progres = QString("%1 / %2 monuments")
                .arg(j->nb_monuments_construits()).arg(partie->get_nb_monuments_win());
        QFontMetrics fmp(police(12, true));
        auto* compteur = addSimpleText(progres, police(12, true));
        compteur->setBrush(Decor::Palette::encre_pale());
        compteur->setPos(cadre.right() - 12 - fmp.horizontalAdvance(progres), cadre.top() + hb + 11);
        compteur->setZValue(31);
        items_adversaires.append(compteur);

        // --- Les monuments : c'est la condition de victoire, elle reste visible
        //     chez tout le monde et en permanence.
        int nb_mon = 0;
        for (const auto& m : j->get_liste_monument())
            if (!Joueur::est_monument_de_depart(m.first->get_nom())) nb_mon++;
        const int lm = nb_mon > 0
                ? std::clamp((int) ((cadre.width() - 24) / nb_mon) - 4, 18, 34) : 18;
        qreal mx = cadre.left() + 12;
        const qreal my = cadre.top() + hb + 34;
        for (const auto& m : j->get_liste_monument()) {
            if (Joueur::est_monument_de_depart(m.first->get_nom())) continue;
            auto* im = new ItemCarte(m.first, ItemCarte::Role::Monument, lm);
            im->set_construit(m.second);
            im->set_proprietaire((int) i);
            im->setPos(mx, my);
            im->setZValue(31);
            connect(im, &ItemCarte::cliquee, this, &ScenePlateau::carte_cliquee);
            connect(im, &ItemCarte::survolee, this, &ScenePlateau::carte_survolee);
            addItem(im);
            items_adversaires.append(im);
            mx += lm + 4;
        }

        const qreal bas_mon = my + lm * 1.55 + 8;
        poser_cartes(QRectF(cadre.left() + 12, bas_mon,
                            cadre.width() - 24, cadre.bottom() - bas_mon - 10),
                     i, items_adversaires, 62);

        x += larg + 12;
    }
}

void ScenePlateau::poser_ville_active() {
    vider(items_actif);
    Partie* partie = Partie::get_instance();
    const unsigned int actuel = partie->get_joueur_actuel();
    const Joueur* j = partie->get_tab_joueurs()[actuel];

    auto* panneau = addPath(forme_panneau(ZONE_ACTIF, 16),
                            QPen(Decor::Palette::or_piece(), 3),
                            QBrush(QColor(253, 250, 243, 244)));
    panneau->setZValue(28);
    items_actif.append(panneau);

    // ------------------------------------------------ le bloc d'identite
    const QRectF ident(ZONE_ACTIF.left(), ZONE_ACTIF.top(), 214, ZONE_ACTIF.height());
    QPainterPath bande;
    bande.addRoundedRect(ident, 16, 16);
    bande.addRect(QRectF(ident.right() - 18, ident.top(), 18, ident.height()));
    auto* b = addPath(bande.simplified(), QPen(Qt::NoPen),
                      QBrush(Decor::Palette::joueur(actuel)));
    b->setZValue(29);
    items_actif.append(b);

    auto centrer = [&](QGraphicsSimpleTextItem* t, qreal y) {
        QFontMetrics fm(t->font());
        t->setPos(ident.center().x() - fm.horizontalAdvance(t->text()) / 2.0, y);
    };

    // Un disque plus clair derriere l'avatar : sans lui, la silhouette se perd
    // sur un fond de sa propre couleur.
    auto* disque = addEllipse(ident.center().x() - 56, ident.top() + 16, 112, 112,
                              QPen(Qt::NoPen), QBrush(QColor(255, 255, 255, 52)));
    disque->setZValue(30);
    items_actif.append(disque);

    auto* av = addPixmap(Decor::avatar(actuel, 96));
    av->setPos(ident.center().x() - 48, ident.top() + 24);
    av->setZValue(31);
    items_actif.append(av);

    auto* nom = addSimpleText(QString::fromStdString(j->get_nom()), police(21, true));
    nom->setBrush(Qt::white);
    centrer(nom, ident.top() + 138);
    nom->setZValue(31);
    items_actif.append(nom);

    auto* qui = addSimpleText(j->get_est_ia() ? "joue…" : "à vous de jouer", police(12, true));
    qui->setBrush(QColor(255, 255, 255, 190));
    centrer(qui, ident.top() + 166);
    qui->setZValue(31);
    items_actif.append(qui);

    // La bourse, dans une pastille sombre : c'est le chiffre qu'on regarde avant
    // de decider quoi construire.
    const QRectF bourse(ident.center().x() - 66, ident.top() + 192, 132, 46);
    auto* fond_bourse = addPath(forme_panneau(bourse, 23), QPen(Qt::NoPen),
                                QBrush(QColor(0, 0, 0, 46)));
    fond_bourse->setZValue(30);
    items_actif.append(fond_bourse);

    auto* piece = addPixmap(Decor::jeton(30));
    piece->setPos(bourse.left() + 12, bourse.center().y() - 15);
    piece->setZValue(31);
    items_actif.append(piece);

    auto* sous = addSimpleText(QString::number(j->get_argent()), police(28, true));
    sous->setBrush(Qt::white);
    sous->setPos(bourse.left() + 52, bourse.center().y() - 19);
    sous->setZValue(31);
    items_actif.append(sous);

    // La progression vers la victoire, en segments : on voit d'un coup d'oeil
    // combien de monuments il reste a batir.
    const unsigned int but = partie->get_nb_monuments_win();
    const unsigned int faits = j->nb_monuments_construits();
    const qreal lseg = (ident.width() - 40 - (but - 1) * 4) / std::max(1u, but);
    for (unsigned int m = 0; m < but; m++) {
        auto* seg = addPath(forme_panneau(QRectF(ident.left() + 20 + m * (lseg + 4),
                                                 ident.bottom() - 52, lseg, 9), 4.5),
                            QPen(Qt::NoPen),
                            QBrush(m < faits ? Decor::Palette::or_piece()
                                             : QColor(255, 255, 255, 70)));
        seg->setZValue(31);
        items_actif.append(seg);
    }
    auto* victoire = addSimpleText(QString("%1 / %2 monuments").arg(faits).arg(but), police(12, true));
    victoire->setBrush(QColor(255, 255, 255, 215));
    centrer(victoire, ident.bottom() - 36);
    victoire->setZValue(31);
    items_actif.append(victoire);

    // ------------------------------------------- les monuments, puis la ville
    const QRectF droite(ident.right() + 18, ZONE_ACTIF.top() + 10,
                        ZONE_ACTIF.width() - ident.width() - 34, ZONE_ACTIF.height() - 20);

    auto* titre_mon = addSimpleText("Monuments", police(12, true));
    titre_mon->setBrush(Decor::Palette::encre_pale());
    titre_mon->setPos(droite.left(), droite.top());
    titre_mon->setZValue(31);
    items_actif.append(titre_mon);

    const int lm = 66;
    qreal mx = droite.left();
    const qreal my = droite.top() + 20;
    for (const auto& m : j->get_liste_monument()) {
        auto* im = new ItemCarte(m.first, ItemCarte::Role::Monument, lm);
        im->set_construit(m.second);
        im->set_proprietaire((int) actuel);
        im->setPos(mx, my);
        im->setZValue(31);
        connect(im, &ItemCarte::cliquee, this, &ScenePlateau::carte_cliquee);
        connect(im, &ItemCarte::survolee, this, &ScenePlateau::carte_survolee);
        addItem(im);
        items_actif.append(im);
        mx += lm + ECART;
    }

    const qreal bas_mon = my + lm * 1.55;
    auto* titre_ville = addSimpleText("Votre ville", police(12, true));
    titre_ville->setBrush(Decor::Palette::encre_pale());
    titre_ville->setPos(droite.left(), bas_mon + 6);
    titre_ville->setZValue(31);
    items_actif.append(titre_ville);

    poser_cartes(QRectF(droite.left(), bas_mon + 26,
                        droite.width(), droite.bottom() - bas_mon - 26),
                 actuel, items_actif, 118);
}

// ------------------------------------------------------------------ retrait

bool ScenePlateau::concernee_par_la_phase(const Carte* carte, unsigned int joueur) const {
    if (carte == nullptr || carte->est_monument()) return true;
    Partie* partie = Partie::get_instance();
    const auto* bat = static_cast<const Batiment*>(carte);
    const unsigned int total = partie->get_total_des();
    const unsigned int actuel = partie->get_joueur_actuel();

    couleur_bat attendue;
    switch (la_phase) {
        case Phase::Rouge:  attendue = Rouge;  break;
        case Phase::Bleu:   attendue = Bleu;   break;
        case Phase::Vert:   attendue = Vert;   break;
        case Phase::Violet: attendue = Violet; break;
        default: return true;                 // hors resolution, rien n'est estompe
    }
    if (bat->get_couleur() != attendue) return false;

    // Le numero doit tomber, et le beneficiaire doit etre le bon : un rouge ne
    // rapporte qu'aux **autres** joueurs, un vert et un violet qu'a celui dont
    // c'est le tour.
    const auto& nums = bat->get_num_activation();
    if (std::find(nums.begin(), nums.end(), total) == nums.end()) return false;
    switch (attendue) {
        case Rouge:  return joueur != actuel;
        case Bleu:   return true;
        default:     return joueur == actuel;
    }
}

void ScenePlateau::appliquer_retrait() {
    const bool en_resolution = (la_phase == Phase::Rouge || la_phase == Phase::Bleu ||
                                la_phase == Phase::Vert  || la_phase == Phase::Violet);
    const QList<QList<QGraphicsItem*>*> listes = { &items_adversaires, &items_actif };
    for (QList<QGraphicsItem*>* liste : listes) {
        for (QGraphicsItem* it : *liste) {
            auto* ic = dynamic_cast<ItemCarte*>(it);
            if (ic == nullptr || ic->role() == ItemCarte::Role::Monument) continue;
            ic->set_en_retrait(en_resolution &&
                               !concernee_par_la_phase(ic->carte(), (unsigned int) ic->proprietaire()));
        }
    }
}

// --------------------------------------------------------------------- clics

void ScenePlateau::carte_cliquee(ItemCarte* item) {
    if (item == nullptr || item->carte() == nullptr) return;
    const Carte* carte = item->carte();
    const bool achetable = constructible(carte, item->proprietaire());

    if (achetable) {
        carte_selectionnee = carte;
        rendre_panneau();
        return;
    }

    // Cliquer une carte qu'on ne peut pas construire la donne a lire : c'est le
    // geste naturel pour examiner la ville d'un adversaire. Si on est en phase de
    // construction, on dit aussi pourquoi elle n'est pas constructible — sinon
    // le bouton manque sans explication.
    carte_selectionnee = nullptr;
    mettre_en_avant(carte, decrire(carte) + pourquoi_pas(carte, item->proprietaire()), false);
}

QString ScenePlateau::pourquoi_pas(const Carte* carte, int proprietaire) const {
    Partie* partie = Partie::get_instance();
    const unsigned int actuel = partie->get_joueur_actuel();
    const Joueur* j = partie->get_tab_joueurs()[actuel];
    if (!moment_achat || j->get_est_ia() || carte == nullptr) return QString();

    if (carte->est_monument()) {
        if (proprietaire != (int) actuel) return "\n\nCe monument est celui d'un autre joueur.";
        if (Joueur::est_monument_de_depart(carte->get_nom()))
            return "\n\nCe monument est offert construit : il n'y a rien à bâtir.";
        if (j->monument_construit(carte->get_nom())) return "\n\nDéjà construit.";
    } else if (const auto* bat = static_cast<const Batiment*>(carte);
               bat->get_couleur() == Violet && j->possede_batiment(bat->get_nom()) != nullptr) {
        return "\n\nVous possédez déjà cet établissement majeur : le livret n'en "
               "autorise qu'un par ville.";
    } else if (proprietaire >= 0) {
        return "\n\nCette carte est déjà dans une ville. La boutique est au centre du plateau.";
    }

    if (carte->get_prix() > j->get_argent())
        return QString("\n\nIl vous manque %1 pour la construire.")
                .arg(prix_en_toutes_lettres(carte->get_prix() - j->get_argent()));
    return QString();
}

void ScenePlateau::carte_survolee(ItemCarte* item, bool entre) {
    // Le survol ne fait que donner a lire, et ne touche a rien tant qu'une carte
    // est choisie ou qu'une autre est en train de produire son effet.
    if (carte_selectionnee != nullptr || !cartes_projetees.isEmpty()) return;
    if (!entre) { rendre_panneau(); return; }
    if (item == nullptr || item->carte() == nullptr) return;
    mettre_en_avant(item->carte(), decrire(item->carte()), false);
}

void ScenePlateau::bouton_acheter_clique() {
    if (carte_selectionnee == nullptr) return;
    const Carte* c = carte_selectionnee;
    carte_selectionnee = nullptr;
    emit achat_demande(c);
}

void ScenePlateau::set_moment_achat(bool actif) {
    if (moment_achat == actif) return;
    moment_achat = actif;
    if (bouton_rien) bouton_rien->set_actif(actif);
    if (!actif) carte_selectionnee = nullptr;
    rendre_panneau();
}

// ------------------------------------------------------------------- pieces

void ScenePlateau::animer_piece(int source, int destination, int montant) {
    if (montant <= 0) return;
    Partie* partie = Partie::get_instance();
    const unsigned int nb = partie->get_tab_joueurs().size();
    const unsigned int actuel = partie->get_joueur_actuel();

    auto position = [&](int indice) -> QPointF {
        if (indice < 0 || (unsigned int) indice >= nb)
            return QPointF(ZONE_BOUTIQUE.center());           // la banque : le tapis
        if ((unsigned int) indice == actuel)
            return QPointF(ZONE_ACTIF.left() + 107, ZONE_ACTIF.top() + 100);
        // Les adversaires sont ranges dans l'ordre du tour a partir du suivant.
        const unsigned int rang = (indice + nb - actuel) % nb - 1;
        const qreal larg = (ZONE_ADVERSAIRES.width() - (nb - 2) * 12) / (nb - 1);
        return QPointF(ZONE_ADVERSAIRES.left() + rang * (larg + 12) + larg / 2,
                       ZONE_ADVERSAIRES.top() + 50);
    };

    const QPointF depart = position(source);
    const QPointF arrivee = position(destination);
    const int combien = std::min(montant, 6);               // au-dela, illisible

    for (int i = 0; i < combien; i++) {
        auto* piece = addPixmap(Decor::jeton(26));
        piece->setPos(depart);
        piece->setZValue(700);
        // Un QGraphicsPixmapItem n'est pas un QObject : il n'a pas de propriete
        // « pos » a animer. On interpole donc le point nous-memes.
        auto* anim = new QVariantAnimation(this);
        anim->setDuration(420 + i * 70);
        anim->setStartValue(depart);
        anim->setEndValue(arrivee);
        anim->setEasingCurve(QEasingCurve::InOutCubic);
        connect(anim, &QVariantAnimation::valueChanged, this,
                [piece](const QVariant& v) { piece->setPos(v.toPointF()); });
        // La piece se supprime en arrivant : sans ca, chaque transfert laisserait
        // un jeton sur le plateau.
        connect(anim, &QVariantAnimation::finished, this, [this, piece]() {
            removeItem(piece);
            delete piece;
        });
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

// --------------------------------------------------------------- rafraichir

void ScenePlateau::rafraichir() {
    Partie* partie = Partie::get_instance();

    // Les items projetes appartiennent aux listes qu'on va vider : on eteint le
    // projecteur avant, sinon il garderait des pointeurs sur des items detruits.
    cartes_projetees.clear();
    carte_selectionnee = nullptr;

    std::string edition;
    for (const auto& e : partie->get_nom_edition())
        edition += (edition.empty() ? "" : " + ") + e;
    texte_edition->setText(QString::fromStdString(edition));
    texte_tour->setText(QString("Tour %1 · %2 joueurs · %3 monuments pour gagner")
                                .arg(partie->get_compteur_tour() / partie->get_tab_joueurs().size() + 1)
                                .arg(partie->get_tab_joueurs().size())
                                .arg(partie->get_nb_monuments_win()));

    poser_boutique();
    poser_adversaires();
    poser_ville_active();
    poser_des();
    appliquer_retrait();
    rendre_panneau();
    update();
}

void ScenePlateau::vider(QList<QGraphicsItem*>& liste) {
    for (QGraphicsItem* it : liste) {
        auto* ic = dynamic_cast<ItemCarte*>(it);
        if (ic != nullptr) cartes_projetees.removeAll(ic);
        removeItem(it);
        delete it;
    }
    liste.clear();
}
