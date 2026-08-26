#ifndef MACHI_KORO_ITEMCARTE_H
#define MACHI_KORO_ITEMCARTE_H

#include <QGraphicsObject>
#include <QPixmap>

class Carte;

/// Une carte posee sur le plateau.
///
/// C'est la brique de base de la scene : la boutique en aligne une par pile, et
/// chaque ville en aligne une par etablissement possede. Elle sait trois choses
/// que l'ancienne VueCarte ne savait pas faire.
///
/// **Se redresser au survol.** Les cartes sont posees sur la table en
/// perspective : celles du fond sont deux fois plus petites que celles du bord
/// proche. Survolee, une carte se souleve du tapis, perd son inclinaison et
/// rejoint une taille de lecture **identique quelle que soit sa place** : on lit
/// son texte sans quitter le plateau.
///
/// **S'allumer quand elle joue.** Pendant la resolution, la carte qui se
/// declenche se souleve et s'entoure d'un halo. C'est ce qui remplace la ligne
/// « Effet des batiments rouges » du journal.
///
/// **S'eteindre quand elle ne peut rien.** Une carte trop chere ou fermee est
/// grisee, sans disparaitre : le livret precise qu'un etablissement ferme compte
/// toujours pour les autres cartes, il doit donc rester visible.
class ItemCarte : public QGraphicsObject {
    Q_OBJECT
    Q_PROPERTY(qreal releve READ releve WRITE set_releve)
    Q_PROPERTY(qreal halo READ halo WRITE set_halo)

public:
    /// Ce que la carte represente, ce qui change son habillage.
    enum class Role {
        Boutique,   ///< une pile a vendre : porte son prix et le nombre restant
        Ville,      ///< un etablissement possede : porte son numero et ses exemplaires
        Monument,   ///< un monument : construit ou en travaux
        Detail      ///< la carte agrandie du panneau de droite : ni survol ni pastille
    };

    ItemCarte(const Carte* carte, Role role, int largeur, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*) override;

    const Carte* carte() const { return la_carte; }

    /// Nombre d'exemplaires, ou de cartes restant dans la pile. 1 par defaut.
    void set_exemplaires(unsigned int n);
    /// Etablissement ferme : couche a 90 degres, comme le livret le decrit.
    void set_ferme(bool f);
    /// Hors de portee (trop chere, deja possedee) : grisee mais lisible.
    void set_indisponible(bool g);
    /// Monument bati.
    void set_construit(bool c);
    /// Nombre de jetons poses dessus — la Startup en accumule.
    void set_jetons(unsigned int n);

    /// Allume la carte pendant qu'elle produit son effet : elle se souleve de la
    /// table et s'entoure d'un halo.
    void projeter(bool actif, int duree_ms = 220);

    /// Pose la carte sur la table. `u` est l'ecart lateral au centre mesure au
    /// bord proche, `v` la profondeur (0 au fond, 1 devant). Voir Perspective.
    void poser(qreal u, qreal v);
    qreal u() const { return le_u; }
    qreal v() const { return le_v; }

    /// Epingle la carte a une position fixe de l'ecran, hors de la table : c'est
    /// le cas des monuments, qui accompagnent la plaque de leur proprietaire et
    /// ne sont pas etales sur le feutre.
    void poser_hors_table(const QPointF& coin);
    /// Estompe une carte que la phase en cours ne peut pas activer.
    void set_en_retrait(bool r);
    /// Indice du joueur qui la possede, -1 pour une pile de la boutique. Sert a
    /// retrouver « la carte X chez le joueur Y » quand elle produit son effet.
    void set_proprietaire(int j) { le_proprietaire = j; }
    int proprietaire() const { return le_proprietaire; }
    ItemCarte::Role role() const { return le_role; }
    qreal releve() const { return le_releve; }
    void set_releve(qreal v);
    qreal halo() const { return le_halo; }
    void set_halo(qreal v);

signals:
    void cliquee(ItemCarte*);
    void survolee(ItemCarte*, bool entre);

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent*) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent*) override;
    void mousePressEvent(QGraphicsSceneMouseEvent*) override;

private:
    void animer_vers(qreal cible, int duree_ms);
    /// Recalcule la transformation projective depuis (u, v) et le relevement.
    void appliquer_pose();

    const Carte* la_carte;
    Role le_role;
    QSize taille;
    QPixmap image;
    QPixmap ombre;
    int le_proprietaire = -1;
    qreal z_repos = 0.0;
    bool en_retrait = false;
    unsigned int nb_exemplaires = 1;
    unsigned int nb_jetons = 0;
    bool est_ferme = false;
    bool est_indisponible = false;
    bool est_construit = false;
    bool sur_table = true;   ///< faux pour les cartes epinglees a une plaque
    QPointF coin_fixe;       ///< leur coin haut-gauche, en coordonnees de scene
    qreal le_u = 0.0;        ///< ecart lateral au centre, au bord proche
    qreal le_v = 1.0;        ///< profondeur sur la table
    qreal le_releve = 0.0;   ///< 0 = posee a plat, 1 = soulevee et redressee
    qreal le_halo = 0.0;     ///< 0 = eteinte, 1 = en pleine lumiere
};

#endif //MACHI_KORO_ITEMCARTE_H
