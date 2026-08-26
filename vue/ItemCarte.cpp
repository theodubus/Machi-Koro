#include "ItemCarte.h"
#include "Decor.h"
#include "Perspective.h"
#include "Carte.h"
#include "Batiment.h"
#include "Monument.h"

#include <QPainter>
#include <QPropertyAnimation>
#include <QPolygonF>
#include <QGraphicsSceneMouseEvent>
#include <QHash>
#include <algorithm>

namespace {
    /// Les 39 visuels font 400x620 : les redimensionner a chaque repeint couterait
    /// bien plus cher que de garder le resultat. La cle melange le chemin et la
    /// largeur voulue, une meme carte pouvant etre affichee a deux tailles.
    QPixmap image_carte(const std::string& chemin, int largeur) {
        static QHash<QString, QPixmap> cache;
        const QString cle = QString::fromStdString(chemin) + "@" + QString::number(largeur);
        auto it = cache.find(cle);
        if (it != cache.end()) return it.value();

        QPixmap px(QString::fromStdString(chemin));
        if (px.isNull()) {
            // Visuel introuvable : on dessine un carton neutre plutot que de laisser
            // un trou. Le jeu reste jouable meme lance depuis le mauvais repertoire.
            px = QPixmap(400, 620);
            px.fill(QColor("#d8d2c4"));
            QPainter q(&px);
            q.setPen(QPen(QColor("#a89f8c"), 6));
            q.drawRoundedRect(8, 8, 384, 604, 14, 14);
            q.end();
        }
        px = px.scaledToWidth(largeur, Qt::SmoothTransformation);
        cache.insert(cle, px);
        return px;
    }

    /// Ombre portee, mise en cache par taille : la recalculer par carte ferait
    /// s'effondrer la scene des la quinzaine d'elements.
    QPixmap ombre_pour(const QSize& t) {
        static QHash<QString, QPixmap> cache;
        const QString cle = QString::number(t.width()) + "x" + QString::number(t.height());
        auto it = cache.find(cle);
        if (it != cache.end()) return it.value();
        QPixmap px = Decor::ombre_carte(t, 10);
        cache.insert(cle, px);
        return px;
    }
}

ItemCarte::ItemCarte(const Carte* carte, Role role, int largeur, QGraphicsItem* parent)
        : QGraphicsObject(parent), la_carte(carte), le_role(role) {
    // La carte agrandie du panneau de droite ne reagit pas : elle est deja lisible,
    // et la survoler la ferait grandir par-dessus le reste du panneau.
    setAcceptHoverEvents(role != Role::Detail);
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);

    // Un monument commence toujours par sa face « en travaux » : c'est
    // set_construit() qui bascule sur la face batie.
    image = image_carte(carte != nullptr ? carte->get_path_image() : std::string(), largeur);
    taille = image.size();
    ombre = ombre_pour(taille);
}

QRectF ItemCarte::boundingRect() const {
    // On deborde de l'ombre, et de la marge que prend la carte quand elle se
    // redresse : sans cette marge, le halo serait tronque au repeint.
    const qreal m = 16;
    return QRectF(-m, -m, taille.width() + 2 * m, taille.height() + 2 * m);
}

void ItemCarte::set_releve(qreal v) {
    le_releve = v;
    appliquer_pose();
    update();
}

void ItemCarte::set_halo(qreal v) { le_halo = v; update(); }

void ItemCarte::poser(qreal u, qreal v) {
    le_u = u;
    le_v = v;
    // Ce qui est devant passe devant : l'ordre d'empilement suit la profondeur.
    setZValue(10 + 100 * v);
    appliquer_pose();
}

void ItemCarte::poser_hors_table(const QPointF& coin) {
    sur_table = false;
    coin_fixe = coin;
    appliquer_pose();
}

void ItemCarte::appliquer_pose() {
    /// La carte est **couchee dans le plan de la table** : elle n'est pas dressee
    /// face a nous, elle repose sur le feutre, et la camera l'ecrase d'autant plus
    /// qu'elle est loin. Soulevee, elle quitte le plan et se redresse.
    ///
    /// Largeur a laquelle une carte doit arriver pour se lire, quelle que soit
    /// sa taille de repos. Une vignette de monument fait quinze pixels de large.
    static const qreal LARGEUR_LISIBLE = 148;

    if (!sur_table) {
        // Epinglee a une plaque : elle grandit sur place, sans quitter son cadre.
        const qreal e = 1.0 + (LARGEUR_LISIBLE / std::max(1, taille.width()) - 1.0) * le_releve;
        QTransform t;
        t.translate(coin_fixe.x() + taille.width() / 2.0,
                    coin_fixe.y() + taille.height() / 2.0);
        t.scale(e, e);
        t.translate(-taille.width() / 2.0, -taille.height() / 2.0);
        setTransform(t);
        return;
    }

    // Posee : la carte occupe un quadrilatere du plan de la table, que la camera
    // deforme. C'est une vraie projection, pas une carte dressee qu'on aurait
    // retrecie : le bord du fond est plus court que le bord proche.
    const QPolygonF pose = Perspective::quad_carte(le_u, le_v, taille.width());

    QPolygonF cible = pose;
    if (le_releve > 0.001) {
        // Soulevee, elle quitte le plan : elle se redresse face a nous et rejoint
        // une taille de lecture identique ou qu'elle se trouve sur la table.
        const QPointF centre = (pose[0] + pose[1] + pose[2] + pose[3]) / 4.0;
        const qreal l = LARGEUR_LISIBLE, h = l * 1.55;
        const QPointF hg(centre.x() - l / 2, centre.y() - h * 0.66);
        QPolygonF droit;
        droit << hg << hg + QPointF(l, 0) << hg + QPointF(l, h) << hg + QPointF(0, h);
        for (int i = 0; i < 4; i++)
            cible[i] = pose[i] * (1.0 - le_releve) + droit[i] * le_releve;
    }

    QPolygonF source;
    source << QPointF(0, 0) << QPointF(taille.width(), 0)
           << QPointF(taille.width(), taille.height()) << QPointF(0, taille.height());
    QTransform t;
    if (QTransform::quadToQuad(source, cible, t)) setTransform(t);
}

void ItemCarte::set_en_retrait(bool r) {
    if (en_retrait == r) return;
    en_retrait = r;
    update();
}

void ItemCarte::set_exemplaires(unsigned int n) { nb_exemplaires = n; update(); }
void ItemCarte::set_ferme(bool f)               { est_ferme = f; update(); }
void ItemCarte::set_indisponible(bool g)        { est_indisponible = g; update(); }
void ItemCarte::set_construit(bool c) {
    if (est_construit == c) return;
    est_construit = c;
    // Un monument a deux faces : « en travaux » et batie. On recharge le visuel
    // correspondant plutot que de griser la face en travaux.
    if (la_carte != nullptr && la_carte->est_monument()) {
        const auto* m = static_cast<const Monument*>(la_carte);
        image = image_carte(c ? m->get_path_image_actif() : m->get_path_image(), taille.width());
    }
    update();
}
void ItemCarte::set_jetons(unsigned int n)      { nb_jetons = n; update(); }

void ItemCarte::animer_vers(qreal cible, int duree_ms) {
    auto* a = new QPropertyAnimation(this, "releve");
    a->setDuration(duree_ms);
    a->setStartValue(le_releve);
    a->setEndValue(cible);
    a->setEasingCurve(QEasingCurve::OutCubic);
    a->start(QAbstractAnimation::DeleteWhenStopped);
}

void ItemCarte::projeter(bool actif, int duree_ms) {
    auto* a = new QPropertyAnimation(this, "halo");
    a->setDuration(duree_ms);
    a->setStartValue(le_halo);
    a->setEndValue(actif ? 1.0 : 0.0);
    a->setEasingCurve(QEasingCurve::OutCubic);
    a->start(QAbstractAnimation::DeleteWhenStopped);
    animer_vers(actif ? 1.0 : 0.0, duree_ms);
}

void ItemCarte::hoverEnterEvent(QGraphicsSceneHoverEvent*) {
    // Soulevee, la carte deborde largement sur ses voisines : il faut donc la
    // passer devant, sans quoi elle passe *dessous* celles du premier rang et
    // devient encore moins lisible qu'a plat.
    z_repos = zValue();
    setZValue(600);
    animer_vers(1.0, 130);
    emit survolee(this, true);
}

void ItemCarte::hoverLeaveEvent(QGraphicsSceneHoverEvent*) {
    setZValue(z_repos);
    if (le_halo < 0.5) animer_vers(0.0, 160);   // une carte qui joue reste levee
    emit survolee(this, false);
}

void ItemCarte::mousePressEvent(QGraphicsSceneMouseEvent* e) {
    if (e->button() == Qt::LeftButton) emit cliquee(this);
    e->accept();
}

void ItemCarte::paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*) {
    p->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    const QRectF r(0, 0, taille.width(), taille.height());

    // Un etablissement ferme se couche a 90 degres. Le livret : « on incline la
    // carte de 90 degres a droite pour se souvenir de son etat ».
    if (est_ferme) {
        p->translate(r.center());
        p->rotate(90);
        p->translate(-r.center());
    }

    p->drawPixmap(QPointF(-10, -8), ombre);

    if (le_halo > 0.01) {
        // Le halo du projecteur, dessine sous la carte pour deborder autour.
        QColor lueur = Decor::Palette::or_piece();
        lueur.setAlphaF(0.55 * le_halo);
        p->setPen(QPen(lueur, 7 * le_halo));
        p->setBrush(Qt::NoBrush);
        p->drawRoundedRect(r.adjusted(-4, -4, 4, 4), 8, 8);
    }

    p->drawPixmap(QPointF(0, 0), image);

    if (est_indisponible || est_ferme || en_retrait) {
        // Voile pale plutot que disparition : la carte doit rester identifiable.
        // Le livret est explicite sur ce point pour les etablissements fermes, qui
        // « comptent toujours » pour les cartes qui denombrent.
        // Assez pale pour qu'on voie du premier coup d'oeil ce qui est hors de
        // portee, assez leger pour que la carte reste lisible : avec un voile
        // opaque, une bourse vide effacait toute la boutique.
        int opacite = 96;
        if (est_ferme)   opacite = 120;
        if (en_retrait)  opacite = 105;
        p->setPen(Qt::NoPen);
        p->setBrush(QColor(246, 242, 232, opacite));
        p->drawRoundedRect(r, 5, 5);
    }

    if (le_halo > 0.01) {
        QColor bord = Decor::Palette::or_piece();
        bord.setAlphaF(le_halo);
        p->setPen(QPen(bord, 3));
        p->setBrush(Qt::NoBrush);
        p->drawRoundedRect(r.adjusted(1.5, 1.5, -1.5, -1.5), 5, 5);
    }

    // --- Les pastilles d'information ---
    if (le_role == Role::Detail) return;   // la carte est deja lisible en grand
    const int d = qMax(17, taille.width() / 5);      // diametre d'une pastille
    QFont f = p->font();
    f.setBold(true);
    f.setPixelSize(qMax(9, d * 55 / 100));
    p->setFont(f);

    auto pastille = [&](const QPointF& centre, const QString& texte,
                        const QColor& fond, const QColor& encre) {
        p->setPen(QPen(QColor(255, 255, 255, 210), qMax(1.5, d * 0.10)));
        p->setBrush(fond);
        p->drawEllipse(centre, d / 2.0, d / 2.0);
        p->setPen(encre);
        p->drawText(QRectF(centre.x() - d, centre.y() - d / 2.0, d * 2, d),
                    Qt::AlignCenter, texte);
    };

    if (le_role == Role::Boutique) {
        // Le prix, en haut a droite — c'est ce qu'on cherche quand on achete.
        pastille(QPointF(r.right() - d * 0.25, r.top() + d * 0.25),
                 QString::number(la_carte ? la_carte->get_prix() : 0),
                 Decor::Palette::encre(), Qt::white);
        if (nb_exemplaires > 0) {
            // Le nombre restant dans la pile, discret, en bas a gauche.
            const QString n = "x" + QString::number(nb_exemplaires);
            QFont pf = f; pf.setPixelSize(qMax(8, d * 45 / 100)); p->setFont(pf);
            const QRectF fondu(r.left() + 3, r.bottom() - d * 0.80, d * 1.25, d * 0.62);
            p->setPen(Qt::NoPen);
            p->setBrush(QColor(0, 0, 0, 120));
            p->drawRoundedRect(fondu, 4, 4);
            p->setPen(Qt::white);
            p->drawText(fondu, Qt::AlignCenter, n);
            p->setFont(f);
        }
    } else if (le_role == Role::Ville) {
        if (nb_exemplaires > 1)
            pastille(QPointF(r.right() - d * 0.25, r.bottom() - d * 0.25),
                     QString::number(nb_exemplaires),
                     Decor::Palette::encre(), Qt::white);
        if (nb_jetons > 0)
            // Les pieces posees sur la Startup : sans elles, on ne peut pas
            // savoir ce que la carte va rapporter.
            pastille(QPointF(r.right() - d * 0.25, r.top() + d * 0.25),
                     QString::number(nb_jetons),
                     Decor::Palette::or_piece(), Decor::Palette::encre());
    } else if (le_role == Role::Monument && !est_construit) {
        // En travaux : un liere pointille, et son prix.
        p->setPen(QPen(QColor(255, 255, 255, 190), 2, Qt::DashLine));
        p->setBrush(Qt::NoBrush);
        p->drawRoundedRect(r.adjusted(2, 2, -2, -2), 5, 5);
        pastille(QPointF(r.right() - d * 0.25, r.top() + d * 0.25),
                 QString::number(la_carte ? la_carte->get_prix() : 0),
                 Decor::Palette::or_piece(), Decor::Palette::encre());
    }
}
