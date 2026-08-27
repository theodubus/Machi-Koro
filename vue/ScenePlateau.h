#ifndef MACHI_KORO_SCENEPLATEAU_H
#define MACHI_KORO_SCENEPLATEAU_H

#include <QGraphicsScene>
#include <QList>
#include <QStringList>
#include <QMap>
#include <QPointF>
#include <string>
#include <vector>

class ItemCarte;
class ItemBouton;
class Carte;
class QGraphicsPixmapItem;
class QGraphicsSimpleTextItem;
class QGraphicsTextItem;
class QGraphicsRectItem;
class QGraphicsPathItem;

/// Le plateau, tel qu'on le voit.
///
/// La scene a une taille **logique fixe** de 1600 x 900 : la vue l'ajuste a la
/// fenetre en conservant les proportions. Toute la mise en page est donc calculee
/// une fois, en coordonnees de scene, et ne depend plus de la taille reelle de la
/// fenetre — d'ou venait la moitie des defauts d'affichage de l'ancienne
/// interface, ou chaque widget portait sa taille fixe en pixels.
///
/// Tout se passe **sur une table ovale**, vue de trois quarts, posee dans un
/// paysage. Il n'y a pas de tableau de bord : la boutique est etalee au centre
/// de la table et les villes des joueurs de part et d'autre. Ce qui est loin est
/// plus petit et plus ecrase — voir Perspective, qui porte toute la geometrie.
///
/// La table **deborde du cadre par le bas** : un ovale se pince a ses deux
/// pointes, et la pointe proche est justement la ou le joueur pose ses cartes.
/// La laisser sortir de l'ecran garde large la bande du bord proche.
///
///        ciel, montagnes, silhouette de ville
///     +----------------------------------------------+
///     | rail des phases                     entete   |
///     | journal   ___---------------------___        |
///     |          / [adv]   [adv]    [adv]   \        |
///     | [des]   |                            | [pioche]
///     |         |  boutique etalee au centre |       |
///     |          \                          /        |
///     |            ma ville, au bord proche           |
///     | plaque                            boutons    |
///     +----------------------------------------------+
///
/// Les adversaires s'installent **tous au fond**, cote a cote dans l'ordre du
/// tour : les flancs restent libres pour les des et la pioche, qui n'ont nulle
/// part ou aller ailleurs depuis que la table remplit le cadre.
///
/// La ville concernee par l'etape en cours se **deplie** : ses cartes avancent
/// et grandissent. Celle qui produit son effet se souleve du tapis, se redresse
/// face camera et s'entoure d'un halo. Un clic sur un joueur epingle sa ville
/// depliee, pour l'examiner pendant que le tour continue.
class ScenePlateau : public QGraphicsScene {
    Q_OBJECT
public:
    /// Les temps d'un tour, dans l'ordre du livret. Le rail les affiche, et la
    /// scene s'en sert pour decider quelles cartes elle estompe.
    enum class Phase { Attente, Des, Rouge, Bleu, Vert, Violet, Achat };

    explicit ScenePlateau(QObject* parent = nullptr);

    static constexpr int LARGEUR = 1600;
    static constexpr int HAUTEUR = 900;

    /// Reconstruit tout depuis le modele. Appelee a chaque changement d'etat.
    void rafraichir();

    /// Change le temps du tour affiche sur le rail, et ce que la scene estompe.
    void set_phase(Phase p);
    Phase phase() const { return la_phase; }

    /// Met une carte en lumiere pendant qu'elle produit son effet : elle se leve
    /// et s'entoure d'un halo **chez chacun des joueurs cites**, et une copie
    /// agrandie s'affiche dans le panneau de droite avec l'explication.
    void projeter(const Carte* carte, const std::vector<unsigned int>& possesseurs,
                  const QString& explication);
    /// Eteint le projecteur et rend le panneau de droite a la carte selectionnee.
    void eteindre_projecteur();

    /// Fait voyager des pieces d'un joueur vers un autre, ou depuis la banque
    /// (source = -1). C'est ce qui rend visible « qui paie qui ».
    void animer_piece(int source, int destination, int montant);

    /// Ajoute une ligne au journal.
    void journal(const std::string& texte);

    /// Rafraichit l'affichage des des depuis le modele. Un de a zero n'a pas ete
    /// lance : le socle affiche alors « Dés non lancés » plutot qu'un zero.
    void montrer_des();

    /// Ouvre ou ferme la possibilite de construire.
    void set_moment_achat(bool actif);

    /// Epingle la ville d'un joueur en position depliee, ou la relache.
    void basculer_epingle(unsigned int joueur);

signals:
    /// Le joueur confirme l'achat de cette carte.
    void achat_demande(const Carte*);
    /// Le joueur a choisi de ne rien construire.
    void rien_faire();

protected:
    /// Un clic sur la plaque d'un joueur epingle sa ville ouverte.
    void mousePressEvent(QGraphicsSceneMouseEvent* e) override;

private slots:
    void carte_cliquee(ItemCarte*);
    void carte_survolee(ItemCarte*, bool entre);
    void bouton_acheter_clique();

private:
    void construire_fond();
    void construire_rail();
    void construire_entete();
    void construire_journal();
    void construire_panneau_avant();

    void poser_boutique();
    void poser_adversaires();
    void poser_ville_active();
    void poser_des();

    /// Ou s'installe un joueur autour de la table : le centre de sa ville sur le
    /// tapis, la largeur de sa plaque et celle de la bande ou sa ville s'etale,
    /// et le coin ou sa plaque se pose — hors du tapis.
    struct Place { qreal u; qreal v; qreal largeur_plaque; qreal bande; QPointF plaque; };
    Place place_de(unsigned int rang, unsigned int nb_adversaires) const;

    /// Etale les cartes d'une ville sur la table, autour de (u, v), rangees par
    /// numero d'activation. `bande` est la largeur qui lui est allouee et
    /// `largeur_max` la plus grande carte qu'on accepte d'y poser : c'est la
    /// methode qui choisit la taille reelle et le nombre de rangees, bornees par
    /// `rangees_max`, de facon que chaque carte laisse voir sa lisiere.
    void poser_cartes(unsigned int indice_joueur, qreal u_centre, qreal v,
                      int largeur_max, int rangees_max, qreal bande,
                      QList<QGraphicsItem*>& sortie);

    /// La plaque d'un joueur — avatar, nom, bourse, monuments — posee au bord du
    /// tapis, hors de la table.
    void poser_plaque(unsigned int joueur, const Place& p, bool actif,
                      QList<QGraphicsItem*>& sortie);

    /// Vrai si la ville de ce joueur doit etre depliee : l'etape en cours la
    /// concerne, ou le joueur l'a epinglee.
    bool ville_depliee(unsigned int joueur) const;

    /// Affiche une carte en grand dans le panneau de droite.
    void mettre_en_avant(const Carte* carte, const QString& explication,
                         bool proposer_achat);
    /// Rend la bulle a la carte selectionnee, ou l'efface s'il n'y en a aucune.
    void rendre_panneau();
    /// Cale la bulle a cote de cette carte.
    void viser_bulle(const ItemCarte* item);
    /// Numeros d'activation, prix et texte de la carte, mis en forme.
    QString decrire(const Carte* carte) const;
    /// Vrai si le joueur courant peut construire cette carte maintenant.
    bool constructible(const Carte* carte, int proprietaire) const;
    /// Ce qui empeche de la construire, en une phrase. Vide hors phase d'achat.
    QString pourquoi_pas(const Carte* carte, int proprietaire) const;

    /// Vrai si la phase en cours peut activer cette carte chez ce joueur.
    bool concernee_par_la_phase(const Carte* carte, unsigned int joueur) const;
    void appliquer_retrait();

    void vider(QList<QGraphicsItem*>& liste);

    QGraphicsPixmapItem* fond = nullptr;
    QGraphicsPixmapItem* le_tapis = nullptr;

    QList<QGraphicsItem*> items_boutique;
    QList<QGraphicsItem*> items_adversaires;
    QList<QGraphicsItem*> items_actif;
    QList<QGraphicsItem*> items_des;
    QList<QGraphicsItem*> items_rail;
    QList<QGraphicsItem*> items_avant;

    QGraphicsSimpleTextItem* texte_edition = nullptr;
    QGraphicsSimpleTextItem* texte_tour = nullptr;
    QGraphicsTextItem* texte_journal = nullptr;
    QStringList lignes_journal;

    /// Largeur de la bulle qui explique la carte mise en avant.
    static constexpr int LARGEUR_BULLE = 330;

    ItemBouton* bouton_rien = nullptr;
    ItemBouton* bouton_acheter = nullptr;
    QGraphicsPathItem* bulle = nullptr;
    QGraphicsTextItem* texte_avant = nullptr;
    QGraphicsSimpleTextItem* titre_avant = nullptr;

    /// La carte selectionnee dans la boutique, en attente de confirmation.
    const Carte* carte_selectionnee = nullptr;
    /// Les cartes actuellement allumees par le projecteur.
    QList<ItemCarte*> cartes_projetees;
    /// Les zones cliquables des plaques de joueur, porteuses de leur indice.
    QList<QGraphicsItem*> plaques;
    /// Ou se trouve la plaque de chaque joueur : les pieces y voyagent.
    QMap<unsigned int, QPointF> ancres_joueurs;
    /// Le point dont la bulle doit se tenir a cote : la carte mise en avant.
    QPointF ancre_bulle{800, 500};
    /// La carte que la bulle commente, pour la retrouver apres un projecteur.
    const ItemCarte* carte_visee = nullptr;

    Phase la_phase = Phase::Attente;
    bool moment_achat = false;
    /// Villes que le joueur a epinglees ouvertes.
    QList<unsigned int> epinglees;
};

#endif //MACHI_KORO_SCENEPLATEAU_H
