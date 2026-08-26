#include "ScenePlateau.h"
#include "ItemCarte.h"
#include "ItemBouton.h"
#include "Decor.h"
#include "Perspective.h"
#include "Partie.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QGraphicsPathItem>
#include <QGraphicsEllipseItem>
#include <QPainterPath>
#include <QPainter>
#include <QVariantAnimation>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>
#include <QFontMetrics>
#include <QtMath>
#include <algorithm>

namespace {

    // ------------------------------------------- ce qui n'est pas sur la table
    //
    // Trois blocs seulement flottent au-dessus du paysage : le rail des etapes,
    // l'entete, et les boutons. Tout le reste — boutique, villes, des, pioche —
    // est **pose sur le tapis**, en coordonnees (u, v) de Perspective.
    const QRectF ZONE_RAIL    (  20,  14, 1000, 40);
    const QRectF ZONE_ENTETE  (1216,  14,  364, 44);
    const QRectF ZONE_JOURNAL (  20,  74,  244, 168);
    const QRectF ZONE_BOUTONS (1268, 828,  312,  52);

    // ------------------------------------------------- les places sur la table
    // Toutes ces mesures sont en **unites de table** — le tapis a 620 de rayon —
    // et non en pixels : c'est la camera qui decide de ce que cela donne a
    // l'ecran, et elle change tout d'un coup si l'on touche a son inclinaison.
    const qreal BOUTIQUE_U      = 320;   ///< demi-largeur de la boutique
    const qreal BOUTIQUE_V_FOND = 0.28;
    const qreal BOUTIQUE_V_PRES = 0.60;
    const qreal MA_VILLE_V      = 0.86;
    const qreal MA_VILLE_U      = 470;
    // Les des se lancent devant soi, et la pioche est a portee de main : tous
    // deux au bord proche de la table, dans les coins que la ville laisse libres.
    const qreal DES_U           = -450;
    const qreal DES_V           = 0.71;
    const qreal PIOCHE_U        =  450;
    const qreal PIOCHE_V        = 0.71;

    /// Part de la hauteur d'une carte qui doit rester visible quand la rangee de
    /// devant la recouvre. Un quart cache, c'est ce que fait un vrai etalement
    /// sur une table : on voit le nom et le numero de chaque carte du fond.
    const qreal VISIBLE_DERRIERE = 0.75;

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

    /// Les cartes d'une ville, rangees par numero d'activation : quand les des
    /// tombent, c'est un numero qu'on cherche des yeux, pas un nom.
    std::vector<Tuile> ville_de(const Joueur* j) {
        std::vector<Tuile> tuiles;
        for (const auto& couleur : j->get_liste_batiment())
            for (const auto& b : couleur.second)
                tuiles.push_back({b.first, b.second, false});
        for (Batiment* f : j->get_liste_batiment_fermes())
            tuiles.push_back({f, 1, true});
        std::sort(tuiles.begin(), tuiles.end(), [](const Tuile& a, const Tuile& b) {
            const unsigned int na = premier_numero(a.batiment), nb = premier_numero(b.batiment);
            if (na != nb) return na < nb;
            return a.batiment->get_nom() < b.batiment->get_nom();
        });
        return tuiles;
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

    // Le tapis est l'ellipse que decrit Perspective : c'est la meme geometrie qui
    // dessine la table et qui y pose les cartes, elles ne peuvent pas diverger.
    const QRectF t = Perspective::tapis();
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
    while (lignes_journal.size() > 8) lignes_journal.removeLast();
    if (texte_journal) texte_journal->setPlainText(lignes_journal.join("\n"));
}

// ------------------------------------------------------- panneau de mise en avant

void ScenePlateau::construire_panneau_avant() {
    // Une bulle, pas un panneau fixe : elle se pose **a cote de la carte** dont
    // elle parle. Un tiers d'ecran reserve en permanence a un cadre vide serait
    // exactement le tableau de bord qu'on cherche a eviter.
    bulle = addPath(QPainterPath(), QPen(QColor(255, 255, 255, 170), 2),
                    QBrush(QColor(47, 59, 70, 238)));
    bulle->setZValue(650);
    bulle->setVisible(false);

    titre_avant = addSimpleText("", police(16, true));
    titre_avant->setBrush(QColor(255, 255, 255, 240));
    titre_avant->setZValue(652);
    titre_avant->setVisible(false);

    texte_avant = addText("");
    texte_avant->setTextWidth(LARGEUR_BULLE - 32);
    texte_avant->setDefaultTextColor(QColor(255, 255, 255, 228));
    texte_avant->setFont(police(13));
    texte_avant->setZValue(652);
    texte_avant->setVisible(false);

    bouton_acheter = new ItemBouton("Construire", QSizeF(LARGEUR_BULLE - 32, 42));
    bouton_acheter->setZValue(653);
    bouton_acheter->set_teinte(Decor::Palette::or_sombre());
    bouton_acheter->setVisible(false);
    addItem(bouton_acheter);
    connect(bouton_acheter, &ItemBouton::clique, this, &ScenePlateau::bouton_acheter_clique);

    bouton_rien = new ItemBouton("Ne rien construire", QSizeF(ZONE_BOUTONS.width(), ZONE_BOUTONS.height()));
    bouton_rien->setPos(ZONE_BOUTONS.topLeft());
    bouton_rien->setZValue(653);
    bouton_rien->set_actif(false);
    addItem(bouton_rien);
    connect(bouton_rien, &ItemBouton::clique, this, &ScenePlateau::rien_faire);
}

void ScenePlateau::mettre_en_avant(const Carte* carte, const QString& explication,
                                   bool proposer_achat) {
    if (carte == nullptr) {
        bulle->setVisible(false);
        titre_avant->setVisible(false);
        texte_avant->setVisible(false);
        bouton_acheter->setVisible(false);
        return;
    }

    titre_avant->setText(QString::fromStdString(carte->get_nom_affiche()));
    texte_avant->setPlainText(explication);

    // La bulle se pose du cote ou il y a de la place, et jamais hors de l'ecran.
    // Pres du bord bas elle passe **au-dessus** de la carte : c'est la que se
    // trouvent la plaque du joueur et les boutons, qu'elle ne doit pas couvrir.
    const qreal h_texte = texte_avant->boundingRect().height();
    const qreal haut = 44 + h_texte + (proposer_achat ? 56 : 10);
    qreal x = ancre_bulle.x() + 30;
    if (x + LARGEUR_BULLE > LARGEUR - 12) x = ancre_bulle.x() - LARGEUR_BULLE - 30;
    x = std::clamp(x, 12.0, (qreal) LARGEUR - LARGEUR_BULLE - 12);
    qreal y = ancre_bulle.y() > HAUTEUR * 0.66 ? ancre_bulle.y() - haut - 40
                                               : ancre_bulle.y() - haut / 2;
    y = std::clamp(y, 70.0, (qreal) HAUTEUR - haut - 12);

    const QRectF r(x, y, LARGEUR_BULLE, haut);
    bulle->setPath(forme_panneau(r, 14));
    bulle->setVisible(true);
    titre_avant->setPos(r.left() + 16, r.top() + 12);
    titre_avant->setVisible(true);
    texte_avant->setPos(r.left() + 14, r.top() + 38);
    texte_avant->setVisible(true);

    bouton_acheter->setPos(r.left() + 16, r.bottom() - 52);
    bouton_acheter->setVisible(proposer_achat);
}

void ScenePlateau::eteindre_projecteur() {
    for (ItemCarte* ic : cartes_projetees) ic->projeter(false);
    cartes_projetees.clear();
    rendre_panneau();
}

void ScenePlateau::rendre_panneau() {
    // On rend la bulle a la carte que le joueur avait choisie, s'il y en a.
    viser_bulle(carte_visee);
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
            if (cartes_projetees.isEmpty()) viser_bulle(ic);
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
    if (n == 0) return;

    const Joueur* actuel = partie->get_tab_joueurs()[partie->get_joueur_actuel()];
    const unsigned int bourse = actuel->get_argent();

    // La boutique va de neuf tas a trente-neuf : on cherche la disposition qui
    // remplit la bande centrale de la table sans deborder, en essayant chaque
    // nombre de rangees. Les rangees se resserrent vers le fond toutes seules,
    // c'est la perspective qui s'en charge.
    int rangees = 1, colonnes = n, largeur = 40;
    for (int r = 1; r <= 4; r++) {
        const int c = (n + r - 1) / r;
        const qreal pas_v = (BOUTIQUE_V_PRES - BOUTIQUE_V_FOND) / std::max(1, r);
        const int par_largeur = (int) ((2 * BOUTIQUE_U - (c - 1) * ECART) / c);
        // La hauteur d'une carte doit tenir dans le pas de profondeur, sans quoi
        // une rangee recouvre celle de devant.
        // Une carte couchee occupe `profondeur_carte(w)` en profondeur : pour que
        // la rangee de devant n'en cache qu'un quart, il faut assez d'ecart.
        int par_profondeur = 16;
        while (par_profondeur < 260 &&
               Perspective::profondeur_carte(par_profondeur + 1) * VISIBLE_DERRIERE <= pas_v)
            par_profondeur++;
        const int w = std::min({par_largeur, par_profondeur, 98});
        if (w > largeur) { largeur = w; rangees = r; colonnes = c; }
    }

    const qreal pas_v = (BOUTIQUE_V_PRES - BOUTIQUE_V_FOND) / std::max(1, rangees);
    int i = 0;
    for (const auto& pile : piles) {
        Batiment* bat = pile.first;
        const int r = i / colonnes, c = i % colonnes;
        const int dans_rangee = std::min(colonnes, n - r * colonnes);
        const qreal v = BOUTIQUE_V_FOND + r * pas_v;
        const qreal u = (c - (dans_rangee - 1) / 2.0) * (largeur + ECART);

        auto* item = new ItemCarte(bat, ItemCarte::Role::Boutique, largeur);
        item->set_exemplaires(pile.second);
        // Un etablissement majeur deja possede ne peut pas etre rachete : le
        // livret n'autorise qu'un exemplaire de chaque carte violette par ville.
        const bool deja = bat->get_couleur() == Violet &&
                          actuel->possede_batiment(bat->get_nom()) != nullptr;
        item->set_indisponible(bat->get_prix() > bourse || deja);
        item->poser(u, v);
        connect(item, &ItemCarte::cliquee, this, &ScenePlateau::carte_cliquee);
        connect(item, &ItemCarte::survolee, this, &ScenePlateau::carte_survolee);
        addItem(item);
        items_boutique.append(item);
        i++;
    }

}

// ------------------------------------------------------------ des et pioche

void ScenePlateau::poser_des() {
    vider(items_des);
    Partie* partie = Partie::get_instance();

    // Les des et la pioche sont poses **sur la table**, devant la boutique. Ils
    // sont dessines dans une image puis mis a l'echelle de leur profondeur :
    // c'est le meme traitement que les cartes, donc la meme perspective.
    auto poser_plateau = [&](const QPixmap& px, qreal u, qreal v) {
        if (px.isNull()) return;
        auto* it = addPixmap(px);
        // Les des et la pioche sont dessines a une taille commode ; on les ramene
        // a l'echelle de la table, ou une unite vaut a peu pres un pixel.
        const qreal e = Perspective::echelle(v) * 0.58;
        // Rien ne se pose a cote de la table : on ramene sur le feutre.
        const qreal marge = std::max(0.0, Perspective::demi_largeur(v) - px.width() * e / 2.0 - 20);
        const QPointF a = Perspective::projeter(std::clamp(u, -marge, marge), v);
        QTransform t;
        t.translate(a.x(), a.y());
        t.scale(e, e);
        t.translate(-px.width() / 2.0, -px.height());
        it->setTransform(t);
        it->setZValue(10 + 100 * v);
        items_des.append(it);
    };

    // --- Les des ---
    const unsigned int d1 = partie->get_de_1();
    const unsigned int d2 = partie->get_de_2();
    const unsigned int total = partie->get_total_des();

    QPixmap socle(300, 188);
    socle.fill(Qt::transparent);
    {
        QPainter p(&socle);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0, 0, 0, 46));
        p.drawEllipse(QRectF(30, 162, 240, 24));

        if (d1 == 0) {
            p.setPen(QColor(255, 255, 255, 215));
            p.setFont(police(23, true));
            p.drawText(QRectF(0, 62, 300, 46), Qt::AlignCenter, "Dés non lancés");
        } else {
            auto de = [&](qreal x, qreal y, unsigned int valeur) {
                const qreal c = 92;
                p.setPen(QPen(QColor("#ded7c8"), 3));
                p.setBrush(Qt::white);
                p.drawRoundedRect(QRectF(x, y, c, c), 16, 16);
                static const int grille[7][9] = {
                    {0,0,0,0,0,0,0,0,0}, {0,0,0,0,1,0,0,0,0}, {1,0,0,0,0,0,0,0,1},
                    {1,0,0,0,1,0,0,0,1}, {1,0,1,0,0,0,1,0,1}, {1,0,1,0,1,0,1,0,1},
                    {1,0,1,1,0,1,1,0,1},
                };
                if (valeur > 6) return;
                p.setPen(Qt::NoPen);
                p.setBrush(Decor::Palette::encre());
                for (int k = 0; k < 9; k++) {
                    if (!grille[valeur][k]) continue;
                    p.drawEllipse(QPointF(x + c * (0.24 + (k % 3) * 0.26),
                                          y + c * (0.24 + (k / 3) * 0.26)), 7.5, 7.5);
                }
            };
            if (d2 == 0) de(104, 6, d1);
            else { de(50, 6, d1); de(158, 6, d2); }

            // Le total, en gros : c'est lui qui active les etablissements. Le bonus
            // du Port s'ajoute au total et non aux des, il faut donc l'annoncer a
            // part sinon l'ecart entre les faces et le total ne s'explique pas.
            p.setPen(Decor::Palette::or_sombre());
            p.setFont(police(54, true));
            p.drawText(QRectF(0, 100, 300, 58), Qt::AlignCenter, QString::number(total));
            p.setPen(QColor(255, 255, 255, 230));
            p.setFont(police(15, true));
            p.drawText(QRectF(0, 156, 300, 22), Qt::AlignCenter,
                       total != d1 + d2 ? "total (Port : +2)" : "total");
        }
    }
    poser_plateau(socle, DES_U, DES_V);

    // --- La pioche ---
    Pioche* pioche = partie->get_pioche();
    QPixmap tas(290, 188);
    tas.fill(Qt::transparent);
    {
        QPainter p(&tas);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0, 0, 0, 46));
        p.drawEllipse(QRectF(26, 162, 238, 24));
        if (pioche != nullptr && !pioche->est_vide()) {
            QPixmap dos("../assets/batiments/BACK-cartes.png");
            if (!dos.isNull()) {
                dos = dos.scaledToHeight(162, Qt::SmoothTransformation);
                // Une pile, pas une carte : deux epaisseurs decalees dessous.
                p.setBrush(QColor(0, 0, 0, 60));
                p.drawRoundedRect(QRectF(16, 8, dos.width(), 162), 8, 8);
                p.drawPixmap(11, 3, dos);
                p.drawPixmap(6, 0, dos);
            }
            p.setPen(QColor(255, 255, 255, 240));
            p.setFont(police(46, true));
            p.drawText(QRectF(124, 40, 156, 56), Qt::AlignLeft | Qt::AlignVCenter,
                       QString::number(pioche->get_taille()));
            p.setPen(QColor(255, 255, 255, 200));
            p.setFont(police(16, true));
            p.drawText(QRectF(124, 96, 156, 24), Qt::AlignLeft, "en pioche");
        } else {
            p.setPen(QColor(255, 255, 255, 215));
            p.setFont(police(22, true));
            p.drawText(QRectF(0, 62, 290, 46), Qt::AlignCenter, "Pioche vide");
        }
    }
    poser_plateau(tas, PIOCHE_U, PIOCHE_V);
}

void ScenePlateau::montrer_des() {
    poser_des();
}

// ------------------------------------------------------------------- villes

ScenePlateau::Place ScenePlateau::place_de(unsigned int rang, unsigned int nb) const {
    /// Ou les adversaires s'installent autour de la table, dans l'ordre du tour a
    /// partir du joueur suivant : on lit de gauche a droite qui joue apres, ce qui
    /// compte pour les cartes rouges — elles remontent depuis le joueur precedent.
    ///
    /// `u`, `v` situent leur ville **sur le tapis** ; `plaque` situe leur panneau
    /// **hors du tapis**, contre le bord de l'ecran. Les deux sont separes parce
    /// qu'au bord de la table il n'y a plus de place pour un panneau : le poser
    /// sur le feutre reviendrait a couvrir les cartes.
    ///
    /// `colonnes` dit comment la ville s'etale : en rangee au fond, en colonne sur
    /// les cotes — comme un joueur qui pose son jeu devant lui.
    const qreal L = LARGEUR, H = HAUTEUR;
    static const qreal PL = 236;                        // largeur d'une plaque
    const QPointF haut_g(L * 0.15, 88), haut_d(L * 0.85 - PL, 88), haut_c(L / 2 - PL / 2, 74);
    const QPointF gauche(18, H * 0.42), droite(L - PL - 18, H * 0.42);

    switch (nb) {
        case 1:  return {    0, 0.20, 1, haut_c};
        case 2:  return rang == 0 ? Place{-250, 0.22, 1, haut_g}
                                  : Place{ 250, 0.22, 1, haut_d};
        case 3:  switch (rang) {
                     case 0:  return {-470, 0.46, 1, gauche};
                     case 1:  return {   0, 0.18, 1, haut_c};
                     default: return { 470, 0.46, 1, droite};
                 }
        case 4:  switch (rang) {
                     case 0:  return {-480, 0.48, 1, gauche};
                     case 1:  return {-250, 0.20, 1, haut_g};
                     case 2:  return { 250, 0.20, 1, haut_d};
                     default: return { 480, 0.48, 1, droite};
                 }
        default: switch (rang) {
                     case 0:  return {-490, 0.50, 1, gauche};
                     case 1:  return {-290, 0.20, 1, haut_g};
                     case 2:  return {   0, 0.16, 1, haut_c};
                     case 3:  return { 290, 0.20, 1, haut_d};
                     default: return { 490, 0.50, 1, droite};
                 }
    }
}

bool ScenePlateau::ville_depliee(unsigned int joueur) const {
    if (epinglees.contains(joueur)) return true;
    // Hors resolution, rien n'est deplie : le plateau est au repos.
    if (la_phase == Phase::Attente || la_phase == Phase::Des) return false;
    Partie* partie = Partie::get_instance();
    if (la_phase == Phase::Achat) return joueur == partie->get_joueur_actuel();
    // Pendant une etape de revenus, la ville se deplie si l'etape peut y activer
    // quelque chose : c'est exactement ce que dit concernee_par_la_phase().
    for (const Tuile& t : ville_de(partie->get_tab_joueurs()[joueur]))
        if (concernee_par_la_phase(t.batiment, joueur)) return true;
    return false;
}

void ScenePlateau::basculer_epingle(unsigned int joueur) {
    if (epinglees.contains(joueur)) epinglees.removeAll(joueur);
    else epinglees.append(joueur);
    poser_adversaires();
    poser_ville_active();
    appliquer_retrait();
}

void ScenePlateau::mousePressEvent(QGraphicsSceneMouseEvent* e) {
    // Les plaques sont de simples rectangles transparents : c'est la scene qui
    // les reconnait, plutot que d'introduire un item de plus pour un seul clic.
    for (QGraphicsItem* it : items(e->scenePos())) {
        if (!plaques.contains(it)) continue;
        basculer_epingle(it->data(0).toUInt());
        e->accept();
        return;
    }
    QGraphicsScene::mousePressEvent(e);
}

void ScenePlateau::poser_cartes(unsigned int indice_joueur, qreal u_centre, qreal v,
                                int largeur, int rangees_max, qreal bande,
                                QList<QGraphicsItem*>& sortie) {
    Partie* partie = Partie::get_instance();
    const Joueur* j = partie->get_tab_joueurs()[indice_joueur];
    const std::vector<Tuile> tuiles = ville_de(j);
    if (tuiles.empty()) return;

    const int n = (int) tuiles.size();

    // Une ville s'etale en une ou deux rangees, et les cartes se chevauchent
    // horizontalement des qu'il y en a trop — comme une main posee en eventail.
    // Empiler les rangees en profondeur, comme le faisait la version precedente,
    // enterrait les cartes du fond sous celles de devant.
    const int rangees = (n > 2 * rangees_max || rangees_max == 1)
                      ? std::min(rangees_max, (n + 7) / 8) : 1;
    const int colonnes = (n + rangees - 1) / rangees;
    const qreal pas_u = colonnes <= 1 ? 0
                      : std::min<qreal>(largeur + ECART, (bande - largeur) / (colonnes - 1));
    const qreal pas_v = 0.075;

    for (int i = 0; i < n; i++) {
        const Tuile& t = tuiles[i];
        const int r = i / colonnes, c = i % colonnes;
        const int dans_rangee = std::min(colonnes, n - r * colonnes);
        // La rangee 0 est la plus au fond.
        const qreal vc = v - (rangees - 1 - r) * pas_v;
        const qreal demi = std::max(0.0, Perspective::demi_largeur(vc)
                                         - (dans_rangee - 1) * pas_u / 2.0 - largeur / 2.0 - 12);
        const qreal centre = std::clamp(u_centre, -demi, demi);
        const qreal u = centre + (c - (dans_rangee - 1) / 2.0) * pas_u;

        auto* ic = new ItemCarte(t.batiment, ItemCarte::Role::Ville, largeur);
        ic->set_exemplaires(t.exemplaires);
        ic->set_ferme(t.ferme);
        ic->set_jetons(j->get_jetons(t.batiment->get_nom()));
        ic->set_proprietaire((int) indice_joueur);
        ic->poser(u, vc);
        // Dans un eventail, c'est la carte de droite qui passe devant sa voisine.
        ic->setZValue(ic->zValue() + c * 0.01);
        connect(ic, &ItemCarte::cliquee, this, &ScenePlateau::carte_cliquee);
        connect(ic, &ItemCarte::survolee, this, &ScenePlateau::carte_survolee);
        addItem(ic);
        sortie.append(ic);
    }
}

void ScenePlateau::poser_plaque(unsigned int joueur, const Place& p, bool actif,
                                QList<QGraphicsItem*>& sortie) {
    Partie* partie = Partie::get_instance();
    const Joueur* j = partie->get_tab_joueurs()[joueur];
    const bool deplie = actif || ville_depliee(joueur);

    const qreal larg = actif ? 306 : 236;
    const qreal haut = actif ? 94 : 68;
    const QRectF cadre(p.plaque, QSizeF(larg, haut));

    // C'est vers ce point que voleront les pieces gagnees ou perdues.
    ancres_joueurs[joueur] = cadre.center();

    auto* panneau = addPath(forme_panneau(cadre, 14),
                            QPen(actif ? Decor::Palette::or_piece()
                                       : (deplie ? QColor(255, 255, 255, 235)
                                                 : QColor(255, 255, 255, 140)),
                                 actif ? 3 : 2),
                            QBrush(QColor(253, 250, 243, deplie ? 246 : 214)));
    panneau->setZValue(300);
    sortie.append(panneau);

    const qreal hb = actif ? 32 : 26;
    QPainterPath bandeau;
    bandeau.addRoundedRect(QRectF(cadre.left(), cadre.top(), cadre.width(), hb), 14, 14);
    bandeau.addRect(QRectF(cadre.left(), cadre.top() + hb - 14, cadre.width(), 14));
    auto* b = addPath(bandeau.simplified(), QPen(Qt::NoPen),
                      QBrush(Decor::Palette::joueur(joueur)));
    b->setZValue(301);
    sortie.append(b);

    const int ta = actif ? 62 : 46;
    auto* av = addPixmap(Decor::avatar(joueur, ta));
    av->setPos(cadre.left() + 8, cadre.top() - ta * 0.34);
    av->setZValue(303);
    sortie.append(av);

    const QFont fnom = police(actif ? 16 : 13, true);
    auto* nom = addSimpleText(QString::fromStdString(j->get_nom()), fnom);
    nom->setBrush(Qt::white);
    {
        QFontMetrics fmn(fnom);
        nom->setPos(cadre.left() + ta + 14, cadre.top() + (hb - fmn.height()) / 2);
    }
    nom->setZValue(303);
    sortie.append(nom);

    auto* piece = addPixmap(Decor::jeton(actif ? 26 : 20));
    piece->setPos(cadre.left() + ta + 14, cadre.top() + hb + 6);
    piece->setZValue(303);
    sortie.append(piece);

    auto* sous = addSimpleText(QString::number(j->get_argent()), police(actif ? 24 : 18, true));
    sous->setBrush(Decor::Palette::encre());
    sous->setPos(cadre.left() + ta + 46, cadre.top() + hb + 2);
    sous->setZValue(303);
    sortie.append(sous);

    // Les monuments : condition de victoire **et** condition de plusieurs cartes.
    // Ils restent visibles chez tout le monde, en permanence.
    int nb_mon = 0;
    for (const auto& m : j->get_liste_monument())
        if (!Joueur::est_monument_de_depart(m.first->get_nom())) nb_mon++;
    const int lm = nb_mon > 0
            ? std::clamp((int) ((cadre.width() - ta - 100) / nb_mon) - 3, 14, actif ? 34 : 24) : 14;
    qreal mx = cadre.left() + ta + 92;
    for (const auto& m : j->get_liste_monument()) {
        if (Joueur::est_monument_de_depart(m.first->get_nom())) continue;
        auto* im = new ItemCarte(m.first, ItemCarte::Role::Monument, lm);
        im->set_construit(m.second);
        im->set_proprietaire((int) joueur);
        // Les monuments ne sont pas poses sur la table : ils accompagnent la
        // plaque, hors du tapis, donc sans perspective.
        im->poser_hors_table(QPointF(mx, cadre.top() + hb + 2));
        im->setZValue(303);
        connect(im, &ItemCarte::cliquee, this, &ScenePlateau::carte_cliquee);
        connect(im, &ItemCarte::survolee, this, &ScenePlateau::carte_survolee);
        addItem(im);
        sortie.append(im);
        mx += lm + 3;
    }

    const QString progres = QString("%1/%2").arg(j->nb_monuments_construits())
                                    .arg(partie->get_nb_monuments_win());
    auto* compteur = addSimpleText(progres, police(12, true));
    compteur->setBrush(QColor(255, 255, 255, 225));
    QFontMetrics fmp(police(12, true));
    compteur->setPos(cadre.right() - 10 - fmp.horizontalAdvance(progres), cadre.top() + 5);
    compteur->setZValue(303);
    sortie.append(compteur);

    if (actif) {
        auto* qui = addSimpleText(j->get_est_ia() ? "joue…" : "à vous de jouer", police(12, true));
        qui->setBrush(Decor::Palette::or_sombre());
        qui->setPos(cadre.left() + ta + 14, cadre.bottom() - 22);
        qui->setZValue(303);
        sortie.append(qui);
    }

    // Cliquer un joueur epingle sa ville depliee : on l'examine pendant que le
    // tour continue. La plaque sert de cible, il n'y a pas de bouton a chercher.
    auto* cible = addRect(cadre, QPen(Qt::NoPen), QBrush(Qt::transparent));
    cible->setZValue(304);
    cible->setAcceptedMouseButtons(Qt::LeftButton);
    cible->setCursor(QCursor(Qt::PointingHandCursor));
    cible->setData(0, joueur);
    sortie.append(cible);
    plaques.append(cible);

    if (epinglees.contains(joueur)) {
        auto* epingle = addSimpleText("épinglée", police(10, true));
        epingle->setBrush(Decor::Palette::or_sombre());
        epingle->setPos(cadre.right() - 58, cadre.bottom() - 18);
        epingle->setZValue(303);
        sortie.append(epingle);
    }
}

void ScenePlateau::poser_adversaires() {
    vider(items_adversaires);
    plaques.clear();
    Partie* partie = Partie::get_instance();
    const std::vector<Joueur*>& joueurs = partie->get_tab_joueurs();
    const unsigned int nb = joueurs.size();
    const unsigned int actuel = partie->get_joueur_actuel();
    if (nb < 2) return;

    for (unsigned int k = 1; k < nb; k++) {
        const unsigned int i = (actuel + k) % nb;
        const Place p = place_de(k - 1, nb - 1);
        const bool deplie = ville_depliee(i);
        // Depliee, la ville avance vers le joueur et ses cartes grandissent :
        // c'est le geste de « pousser son jeu au milieu de la table ».
        const qreal v = p.v + (deplie ? 0.09 : 0.0);
        poser_cartes(i, p.u, v, deplie ? 76 : 62, 1, deplie ? 520 : 400, items_adversaires);
        poser_plaque(i, p, false, items_adversaires);
    }
}

void ScenePlateau::poser_ville_active() {
    vider(items_actif);
    Partie* partie = Partie::get_instance();
    const unsigned int actuel = partie->get_joueur_actuel();

    // Le joueur dont c'est le tour occupe tout le bord proche de la table : sa
    // ville est la plus grande, c'est lui qui doit decider.
    // Sa plaque se pose en bas a gauche, hors du tapis : au bord proche la table
    // se retrecit, et un panneau pose sur le feutre couvrirait ses propres cartes.
    const Place p{0, MA_VILLE_V, 0, QPointF(18, HAUTEUR - 112)};
    poser_cartes(actuel, 0, MA_VILLE_V, 92, 2, 940, items_actif);

    poser_plaque(actuel, p, true, items_actif);
}

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

    viser_bulle(item);
    if (achetable) {
        carte_selectionnee = carte;
        carte_visee = item;
        rendre_panneau();
        return;
    }

    // Cliquer une carte qu'on ne peut pas construire la donne a lire : c'est le
    // geste naturel pour examiner la ville d'un adversaire. Si on est en phase de
    // construction, on dit aussi pourquoi elle n'est pas constructible — sinon
    // le bouton manque sans explication.
    carte_selectionnee = nullptr;
    carte_visee = item;
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

void ScenePlateau::viser_bulle(const ItemCarte* item) {
    // La bulle se pose a cote de la carte concernee : on suit du regard, on ne
    // cherche pas un panneau a l'autre bout de l'ecran.
    if (item == nullptr) { ancre_bulle = QPointF(LARGEUR / 2.0, HAUTEUR * 0.62); return; }
    ancre_bulle = item->sceneBoundingRect().center();
}

void ScenePlateau::carte_survolee(ItemCarte* item, bool entre) {
    // Le survol ne fait que donner a lire, et ne touche a rien tant qu'une carte
    // est choisie ou qu'une autre est en train de produire son effet.
    if (carte_selectionnee != nullptr || !cartes_projetees.isEmpty()) return;
    if (!entre) { rendre_panneau(); return; }
    if (item == nullptr || item->carte() == nullptr) return;
    viser_bulle(item);
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

    auto position = [&](int indice) -> QPointF {
        // La banque, c'est la table elle-meme : les pieces en viennent et y
        // retournent. Un joueur, c'est sa plaque, la ou son magot est affiche.
        if (indice < 0) return Perspective::projeter(0, 0.50);
        auto it = ancres_joueurs.find((unsigned int) indice);
        return it == ancres_joueurs.end() ? Perspective::projeter(0, 0.50) : it.value();
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
    carte_visee = nullptr;
    ancres_joueurs.clear();

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
        if (ic != nullptr) {
            cartes_projetees.removeAll(ic);
            if (ic == carte_visee) carte_visee = nullptr;
        }
        removeItem(it);
        delete it;
    }
    liste.clear();
}
