#include "Decor.h"

#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QMap>
#include <QtMath>

namespace {
    /// Les caches ne peuvent pas etre des statiques de fichier : un QPixmap
    /// construit avant QApplication fait echouer Qt au demarrage. On les range
    /// donc dans une structure creee a la premiere utilisation.
    struct Caches {
        QPixmap ciel;
        QPixmap tapis;
        QSize   taille_ciel;
        QSize   taille_tapis;
        QMap<int, QPixmap> jetons;
    };
    Caches& caches() {
        static Caches c;
        return c;
    }

    /// Generateur deterministe : le relief des montagnes doit etre le meme d'un
    /// lancement a l'autre, sinon le fond « bouge » entre deux redimensionnements.
    struct Suite {
        unsigned int e;
        explicit Suite(unsigned int graine) : e(graine) {}
        double suivant() {
            e = e * 1103515245u + 12345u;
            return ((e >> 16) & 0x7fff) / 32767.0;
        }
    };

    /// Une chaine de montagnes, du plus loin au plus pres.
    void peindre_montagnes(QPainter& p, const QSize& t, double hauteur,
                           const QColor& couleur, unsigned int graine, bool avec_neige) {
        Suite s(graine);
        QPainterPath chemin;
        const double base = t.height() * hauteur;
        chemin.moveTo(-40, t.height());
        chemin.lineTo(-40, base);
        double x = -40;
        QList<QPointF> sommets;
        while (x < t.width() + 40) {
            const double largeur = t.width() * (0.10 + s.suivant() * 0.10);
            const double pointe  = base - t.height() * (0.07 + s.suivant() * 0.16);
            const QPointF sommet(x + largeur / 2, pointe);
            chemin.lineTo(sommet);
            sommets.append(sommet);
            x += largeur;
            chemin.lineTo(x, base - t.height() * 0.02 * s.suivant());
        }
        chemin.lineTo(t.width() + 40, t.height());
        chemin.closeSubpath();
        p.fillPath(chemin, couleur);

        if (!avec_neige) return;
        // Une calotte claire sur les sommets les plus hauts.
        for (const QPointF& sommet : sommets) {
            if (sommet.y() > base - t.height() * 0.13) continue;
            QPainterPath calotte;
            const double d = t.height() * 0.035;
            calotte.moveTo(sommet);
            calotte.lineTo(sommet.x() + d, sommet.y() + d * 1.6);
            calotte.lineTo(sommet.x() + d * 0.35, sommet.y() + d * 1.15);
            calotte.lineTo(sommet.x() - d * 0.3, sommet.y() + d * 1.7);
            calotte.lineTo(sommet.x() - d, sommet.y() + d * 1.6);
            calotte.closeSubpath();
            p.fillPath(calotte, Decor::Palette::neige());
        }
    }

    /// Une silhouette de ville au pied des montagnes : c'est le sujet du jeu.
    void peindre_ville(QPainter& p, const QSize& t, const QColor& couleur) {
        Suite s(7);
        const double base = t.height() * 0.555;
        double x = -20;
        while (x < t.width() + 20) {
            const double l = 18 + s.suivant() * 34;
            const double h = 16 + s.suivant() * 58;
            QRectF bat(x, base - h, l, h);
            p.fillRect(bat, couleur);
            if (s.suivant() > 0.6) {
                // Un toit pointu de temps en temps, pour casser la regularite.
                QPainterPath toit;
                toit.moveTo(bat.left() - 3, bat.top());
                toit.lineTo(bat.center().x(), bat.top() - l * 0.45);
                toit.lineTo(bat.right() + 3, bat.top());
                toit.closeSubpath();
                p.fillPath(toit, couleur);
            }
            x += l + 3 + s.suivant() * 10;
        }
    }
}

namespace Decor {

    QColor Palette::joueur(unsigned int indice) {
        static const QColor couleurs[6] = {
            QColor("#e8823c"),  // orange brique
            QColor("#4e94c8"),  // bleu ardoise
            QColor("#67a95a"),  // vert pousse
            QColor("#c05b7a"),  // framboise
            QColor("#8f6fc0"),  // violet
            QColor("#3fb0a8"),  // turquoise
        };
        return couleurs[indice % 6];
    }

    const QPixmap& ciel(const QSize& t) {
        if (!caches().ciel.isNull() && caches().taille_ciel == t) return caches().ciel;
        caches().taille_ciel = t;
        caches().ciel = QPixmap(t);
        QPainter p(&caches().ciel);
        p.setRenderHint(QPainter::Antialiasing);

        QLinearGradient fond(0, 0, 0, t.height());
        fond.setColorAt(0.0, Palette::ciel_haut());
        fond.setColorAt(0.62, Palette::ciel_bas());
        fond.setColorAt(1.0, QColor("#eaf6ef"));
        p.fillRect(QRect(QPoint(0, 0), t), fond);

        // Quelques nuages ronds, tres pales.
        Suite s(3);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255, 255, 255, 54));
        for (int i = 0; i < 7; i++) {
            const double cx = s.suivant() * t.width();
            const double cy = t.height() * (0.03 + s.suivant() * 0.15);
            const double r  = t.height() * (0.03 + s.suivant() * 0.035);
            for (int b = 0; b < 3; b++)
                p.drawEllipse(QPointF(cx + b * r * 0.75, cy - (b == 1 ? r * 0.35 : 0)),
                              r * (b == 1 ? 1.15 : 0.85), r * (b == 1 ? 1.15 : 0.85));
        }

        peindre_montagnes(p, t, 0.50, Palette::montagne_loin(), 11, true);
        peindre_montagnes(p, t, 0.53, Palette::montagne_pres(), 29, true);
        peindre_ville(p, t, QColor(255, 255, 255, 70));
        return caches().ciel;
    }

    const QPixmap& tapis(const QSize& t) {
        if (!caches().tapis.isNull() && caches().taille_tapis == t) return caches().tapis;
        caches().taille_tapis = t;
        caches().tapis = QPixmap(t);
        caches().tapis.fill(Qt::transparent);
        QPainter p(&caches().tapis);
        p.setRenderHint(QPainter::Antialiasing);

        // Une ellipse, et non un rectangle : c'est une table vue de trois quarts,
        // pas un tapis de souris. Les cartes sont posees dessus en perspective,
        // et le bord proche sort du cadre — comme une vraie table devant soi.
        const QRectF ovale(t.width() * 0.035, t.height() * 0.055,
                           t.width() * 0.930, t.height() * 1.02);

        // Une ombre dessous, pour qu'elle repose sur le decor.
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(20, 60, 45, 55));
        p.drawEllipse(ovale.adjusted(-6, 10, 6, 10));

        QRadialGradient feutre(ovale.center(), ovale.width() * 0.62);
        feutre.setColorAt(0.0, Palette::tapis_centre());
        feutre.setColorAt(1.0, Palette::tapis_bord());
        p.setBrush(feutre);
        p.setPen(QPen(Palette::tapis_lisere(), qMax(4.0, t.height() * 0.007)));
        p.drawEllipse(ovale);

        // Un lisere interieur plus clair, comme une couture.
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor(255, 255, 255, 46), 2, Qt::DashLine));
        p.drawEllipse(ovale.adjusted(ovale.width() * 0.028, ovale.height() * 0.028,
                                     -ovale.width() * 0.028, -ovale.height() * 0.028));
        return caches().tapis;
    }

    QPixmap avatar(unsigned int indice, int taille) {
        QPixmap px(taille, taille);
        px.fill(Qt::transparent);
        QPainter p(&px);
        p.setRenderHint(QPainter::Antialiasing);
        const QColor c = Palette::joueur(indice);
        const double m = taille * 0.11;              // marge
        const QRectF corps(m, m, taille - 2 * m, taille - 2 * m);

        // Quatre silhouettes distinctes : on doit pouvoir les differencier meme
        // sans la couleur, pour les daltoniens comme sur une capture en gris.
        QPainterPath forme;
        switch (indice % 4) {
            case 0:                                   // goutte ronde
                forme.addEllipse(corps);
                break;
            case 1: {                                 // coussin a oreilles
                forme.addRoundedRect(corps, corps.width() * 0.30, corps.height() * 0.30);
                QPainterPath o1, o2;
                o1.addEllipse(QPointF(corps.left() + corps.width() * 0.22, corps.top()), m * 0.9, m * 1.1);
                o2.addEllipse(QPointF(corps.right() - corps.width() * 0.22, corps.top()), m * 0.9, m * 1.1);
                forme = forme.united(o1).united(o2);
                break;
            }
            case 2: {                                 // dome
                forme.moveTo(corps.left(), corps.bottom());
                forme.arcTo(corps, 180, -180);
                forme.lineTo(corps.right(), corps.bottom());
                forme.closeSubpath();
                break;
            }
            default: {                                // hexagone adouci
                const QPointF ctr = corps.center();
                const double r = corps.width() / 2;
                for (int i = 0; i < 6; i++) {
                    const double a = M_PI / 3 * i - M_PI / 2;
                    const QPointF s(ctr.x() + r * qCos(a), ctr.y() + r * qSin(a) * 1.04);
                    if (i == 0) forme.moveTo(s); else forme.lineTo(s);
                }
                forme.closeSubpath();
                break;
            }
        }

        p.setPen(Qt::NoPen);
        p.setBrush(c.darker(118));
        p.translate(0, taille * 0.035);
        p.drawPath(forme);                            // une ombre, decalee
        p.translate(0, -taille * 0.035);
        p.setBrush(c);
        p.drawPath(forme);

        // Un contour sombre : sans lui, la silhouette se perd sur un panneau de la
        // meme couleur que le joueur.
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(c.darker(145), qMax(1.5, taille * 0.022)));
        p.drawPath(forme);
        p.setPen(Qt::NoPen);

        // Une joue plus claire, et un sourire.
        p.setBrush(c.lighter(122));
        p.drawEllipse(QPointF(corps.center().x(), corps.center().y() + corps.height() * 0.26),
                      corps.width() * 0.26, corps.height() * 0.19);
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(Palette::encre(), qMax(1.5, taille * 0.028), Qt::SolidLine, Qt::RoundCap));
        {
            const QRectF bouche(corps.center().x() - corps.width() * 0.135,
                                corps.center().y() + corps.height() * 0.10,
                                corps.width() * 0.27, corps.height() * 0.22);
            p.drawArc(bouche, 200 * 16, 140 * 16);
        }
        p.setPen(Qt::NoPen);

        // Les yeux : c'est ce qui rend la forme vivante.
        const double ry = taille * 0.085, rp = taille * 0.042;
        const double ey = corps.center().y() - corps.height() * 0.10;
        for (int s = -1; s <= 1; s += 2) {
            const double ex = corps.center().x() + s * corps.width() * 0.19;
            p.setBrush(Qt::white);
            p.drawEllipse(QPointF(ex, ey), ry, ry);
            p.setBrush(Palette::encre());
            p.drawEllipse(QPointF(ex + s * ry * 0.12, ey + ry * 0.14), rp, rp);
            p.setBrush(QColor(255, 255, 255, 220));
            p.drawEllipse(QPointF(ex + s * ry * 0.12 - rp * 0.35, ey + ry * 0.14 - rp * 0.38),
                          rp * 0.32, rp * 0.32);
        }
        return px;
    }

    const QPixmap& jeton(int taille) {
        auto it = caches().jetons.find(taille);
        if (it != caches().jetons.end()) return it.value();

        QPixmap px(taille, taille);
        px.fill(Qt::transparent);
        QPainter p(&px);
        p.setRenderHint(QPainter::Antialiasing);
        const QRectF r(1, 1, taille - 2, taille - 2);
        p.setPen(QPen(Palette::or_sombre(), qMax(1.5, taille * 0.075)));
        QRadialGradient g(r.center() - QPointF(r.width() * 0.16, r.height() * 0.18), r.width() * 0.85);
        g.setColorAt(0.0, QColor("#ffe08a"));
        g.setColorAt(1.0, Palette::or_piece());
        p.setBrush(g);
        p.drawEllipse(r);
        p.setPen(QPen(QColor(255, 255, 255, 130), qMax(1.0, taille * 0.045)));
        p.setBrush(Qt::NoBrush);
        p.drawArc(r.adjusted(r.width() * 0.20, r.height() * 0.20,
                             -r.width() * 0.20, -r.height() * 0.20), 45 * 16, 150 * 16);
        return caches().jetons.insert(taille, px).value();
    }

    QPixmap emplacement(const QSize& t) {
        QPixmap px(t);
        px.fill(Qt::transparent);
        QPainter p(&px);
        p.setRenderHint(QPainter::Antialiasing);
        QPen trait(QColor(255, 255, 255, 130), 2, Qt::DashLine);
        trait.setDashPattern({5, 4});
        p.setPen(trait);
        p.setBrush(QColor(255, 255, 255, 22));
        p.drawRoundedRect(QRectF(1, 1, t.width() - 2, t.height() - 2), 5, 5);
        return px;
    }

    QPixmap ombre_carte(const QSize& t, int flou) {
        QPixmap px(t.width() + flou * 2, t.height() + flou * 2);
        px.fill(Qt::transparent);
        QPainter p(&px);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        // Plusieurs passes de plus en plus larges et transparentes : un flou
        // gaussien couterait bien plus cher pour un resultat equivalent a cette
        // taille.
        for (int i = flou; i >= 1; i--) {
            const int a = 46 * (flou - i + 1) / (flou * 3);
            p.setBrush(QColor(0, 0, 0, a));
            p.drawRoundedRect(QRectF(flou - i, flou - i + 2,
                                     t.width() + i * 2, t.height() + i * 2), 6 + i, 6 + i);
        }
        return px;
    }

    QPixmap icone_famille(int type, int taille, const QColor& couleur) {
        QPixmap px(taille, taille);
        px.fill(Qt::transparent);
        QPainter p(&px);
        p.setRenderHint(QPainter::Antialiasing);
        const double u = taille / 24.0;              // grille de 24, comme les icones du web
        QPen trait(couleur, qMax(1.4, 1.9 * u));
        trait.setCapStyle(Qt::RoundCap);
        trait.setJoinStyle(Qt::RoundJoin);
        p.setPen(trait);
        p.setBrush(Qt::NoBrush);
        p.scale(u, u);

        switch (type) {
            case 0:                                   // champ : un epi de ble
                p.drawLine(QPointF(12, 21), QPointF(12, 6));
                for (int i = 0; i < 3; i++) {
                    const double y = 7.5 + i * 3.6;
                    p.drawLine(QPointF(12, y + 2.2), QPointF(7.8, y));
                    p.drawLine(QPointF(12, y + 2.2), QPointF(16.2, y));
                }
                break;
            case 1:                                   // elevage : un museau
                p.drawRoundedRect(QRectF(4.5, 8, 15, 9.5), 5, 5);
                p.drawPoint(QPointF(9, 12)); p.drawPoint(QPointF(15, 12));
                break;
            case 2:                                   // engrenage
                p.drawEllipse(QPointF(12, 12), 3.6, 3.6);
                for (int i = 0; i < 8; i++) {
                    const double a = M_PI / 4 * i;
                    p.drawLine(QPointF(12 + 5.6 * qCos(a), 12 + 5.6 * qSin(a)),
                               QPointF(12 + 8.2 * qCos(a), 12 + 8.2 * qSin(a)));
                }
                break;
            case 3:                                   // bateau
                p.drawLine(QPointF(3.5, 17), QPointF(20.5, 17));
                p.drawLine(QPointF(5.5, 17), QPointF(7, 20.5));
                p.drawLine(QPointF(18.5, 17), QPointF(17, 20.5));
                p.drawLine(QPointF(12, 16.5), QPointF(12, 5));
                p.drawLine(QPointF(12, 6), QPointF(18, 12));
                p.drawLine(QPointF(18, 12), QPointF(12, 12));
                break;
            case 4:                                   // restaurant : une tasse
                p.drawRoundedRect(QRectF(6, 8.5, 10, 9), 2.4, 2.4);
                p.drawArc(QRectF(15, 10, 5, 5.6), 90 * 16, -180 * 16);
                break;
            case 5:                                   // commerce : un pain
                p.drawRoundedRect(QRectF(4, 9.5, 16, 8), 4, 4);
                p.drawLine(QPointF(8, 9.5), QPointF(7, 7));
                p.drawLine(QPointF(12, 9.5), QPointF(12, 6.6));
                p.drawLine(QPointF(16, 9.5), QPointF(17, 7));
                break;
            case 6:                                   // entreprise : une valise
                p.drawRoundedRect(QRectF(3.5, 7.5, 17, 11), 2.2, 2.2);
                p.drawLine(QPointF(9, 7.5), QPointF(9, 5.5));
                p.drawLine(QPointF(9, 5.5), QPointF(15, 5.5));
                p.drawLine(QPointF(15, 5.5), QPointF(15, 7.5));
                break;
            case 7:                                   // usine : toit en dents de scie
                p.drawLine(QPointF(4, 20), QPointF(4, 9));
                p.drawLine(QPointF(4, 9), QPointF(8, 5.5));
                p.drawLine(QPointF(8, 5.5), QPointF(8, 20));
                p.drawLine(QPointF(12, 20), QPointF(12, 12));
                p.drawLine(QPointF(12, 12), QPointF(20, 8.5));
                p.drawLine(QPointF(20, 8.5), QPointF(20, 20));
                break;
            case 8:                                   // marche : un cageot
                p.drawRoundedRect(QRectF(4, 8, 16, 11), 1.6, 1.6);
                p.drawLine(QPointF(4, 12), QPointF(20, 12));
                p.drawLine(QPointF(12, 8), QPointF(12, 19));
                break;
            default:                                  // majeur : une tour
                p.drawLine(QPointF(5, 20), QPointF(5, 9));
                p.drawLine(QPointF(5, 9), QPointF(12, 4));
                p.drawLine(QPointF(12, 4), QPointF(19, 9));
                p.drawLine(QPointF(19, 9), QPointF(19, 20));
                p.drawLine(QPointF(12, 4), QPointF(12, 9.5));
                break;
        }
        return px;
    }

}
