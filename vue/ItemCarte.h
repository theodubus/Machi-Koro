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
/// **Se redresser au survol.** Les cartes sont posees en perspective, ce qui les
/// rend jolies mais peu lisibles. Survolee, une carte se remet d'aplomb, grandit
/// et passe au premier plan : on lit son texte sans quitter le plateau.
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

    /// Allume la carte pendant qu'elle produit son effet.
    void projeter(bool actif, int duree_ms = 220);
    /// Estompe une carte que la phase en cours ne peut pas activer.
    void set_en_retrait(bool r);
    /// Indice du joueur qui la possede, -1 pour une pile de la boutique. Sert a
    /// retrouver « la carte X chez le joueur Y » quand elle produit son effet.
    void set_proprietaire(int j) { le_proprietaire = j; }
    int proprietaire() const { return le_proprietaire; }
    ItemCarte::Role role() const { return le_role; }
    /// Inclinaison de repos, pour poser les cartes en eventail sur le tapis.
    void set_pose(qreal degres);

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
    qreal pose_degres = 0.0;
    qreal le_releve = 0.0;   ///< 0 = posee, 1 = redressee et agrandie
    qreal le_halo = 0.0;     ///< 0 = eteinte, 1 = en pleine lumiere
};

#endif //MACHI_KORO_ITEMCARTE_H
