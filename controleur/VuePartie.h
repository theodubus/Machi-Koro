#ifndef MACHI_KORO_VUEPARTIE_H
#define MACHI_KORO_VUEPARTIE_H

#include <QWidget>
#include <vector>

#include "VueInfo.h"
#include "ScenePlateau.h"

class VuePlateau;
class Carte;

/// La fenetre de jeu.
///
/// Elle ne dessine plus rien elle-meme : tout le plateau vit dans une
/// ScenePlateau, affichee par une VuePlateau. Ce qui reste ici, ce sont les
/// methodes que le controleur et les cartes appellent depuis toujours —
/// `update_vue_*`, `get_vue_infos()` — pour ne pas avoir a toucher aux quarante-
/// sept cartes ni au deroulement du tour.
class VuePartie : public QWidget
{
    Q_OBJECT
public:
    explicit VuePartie(QWidget *parent = nullptr);
    // Le journal n'est pas un widget : il n'a pas de parent Qt pour le liberer.
    ~VuePartie() override;

    /// Rafraichissement. Toutes ces methodes reconstruisent la scene depuis le
    /// modele : la decoupe en « vue joueur », « vue shop », « vue pioche » datait
    /// d'un temps ou chacune etait un widget distinct, et le controleur les
    /// appelle encore une par une. Les garder distinctes evite de reprendre une
    /// trentaine d'appels pour rien.
    void update_vue_joueur();
    void update_vue_partie();
    void update_vue_shop();
    void update_vue_pioche();
    void update_vue_info();
    void update_des();
    void update_nom_joueur();

    /// Ouvre ou ferme la possibilite de construire.
    void set_bouton_rien_faire(bool b);

    VueInfo* get_vue_infos() const {return infos;}

    /// Ou en est le tour. Le rail du plateau l'affiche, et la scene estompe les
    /// cartes que cette etape ne peut pas activer.
    void set_phase(ScenePlateau::Phase p);

    /// Met une carte en lumiere pendant qu'elle produit son effet, chez chacun
    /// des joueurs cites.
    void projeter(const Carte* carte, const std::vector<unsigned int>& possesseurs,
                  const QString& explication);
    void eteindre_projecteur();

    /// Fait voyager des pieces d'un joueur a un autre (source -1 : la banque).
    void animer_piece(int source, int destination, int montant);

private slots:
    void construire(const Carte* carte);
    void ne_rien_construire();

private:
    VuePlateau* plateau;
    VueInfo* infos;
};

#endif //MACHI_KORO_VUEPARTIE_H
