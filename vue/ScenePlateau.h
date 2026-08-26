#ifndef MACHI_KORO_SCENEPLATEAU_H
#define MACHI_KORO_SCENEPLATEAU_H

#include <QGraphicsScene>
#include <QList>
#include <QStringList>
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
/// Cinq zones, toujours au meme endroit :
///
///     +--------------------------------------------------+-----------+
///     | rail des phases du tour                          | entete    |
///     +--------------------------------------------------+-----------+
///     | villes des adversaires                           |           |
///     +--------------------------------------------------+ journal   |
///     | boutique, posee sur le tapis          [ des ]    |           |
///     +--------------------------------------------------+-----------+
///     | ville du joueur dont c'est le tour               | carte mise |
///     |                                                  | en avant   |
///     +--------------------------------------------------+-----------+
///
/// Le joueur dont c'est le tour occupe toute la bande du bas, avec sa ville
/// entiere et ses monuments : c'est lui qui doit decider, il doit tout voir.
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

signals:
    /// Le joueur confirme l'achat de cette carte.
    void achat_demande(const Carte*);
    /// Le joueur a choisi de ne rien construire.
    void rien_faire();

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

    /// Range les cartes d'une ville dans un rectangle donne, du plus petit au
    /// plus grand numero d'activation, en adaptant leur taille au nombre a poser.
    /// Rend la hauteur reellement occupee.
    qreal poser_cartes(const QRectF& zone, unsigned int indice_joueur,
                       QList<QGraphicsItem*>& sortie, int largeur_max);

    /// Affiche une carte en grand dans le panneau de droite.
    void mettre_en_avant(const Carte* carte, const QString& explication,
                         bool proposer_achat);
    /// Rend le panneau de droite a la carte selectionnee, ou a son message
    /// d'attente s'il n'y en a aucune.
    void rendre_panneau();
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

    ItemBouton* bouton_rien = nullptr;
    ItemBouton* bouton_acheter = nullptr;
    QGraphicsTextItem* texte_avant = nullptr;
    QGraphicsSimpleTextItem* titre_avant = nullptr;

    /// La carte selectionnee dans la boutique, en attente de confirmation.
    const Carte* carte_selectionnee = nullptr;
    /// Les cartes actuellement allumees par le projecteur.
    QList<ItemCarte*> cartes_projetees;

    Phase la_phase = Phase::Attente;
    bool moment_achat = false;
};

#endif //MACHI_KORO_SCENEPLATEAU_H
