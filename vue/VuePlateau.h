#ifndef MACHI_KORO_VUEPLATEAU_H
#define MACHI_KORO_VUEPLATEAU_H

#include <QGraphicsView>

class ScenePlateau;

/// La fenetre par laquelle on regarde le plateau.
///
/// Elle ne fait qu'une chose : ajuster la scene — de taille logique fixe — a la
/// taille reelle de la fenetre, en conservant les proportions. Toute la mise en
/// page est ainsi calculee une seule fois, et un redimensionnement se contente de
/// changer l'echelle. L'ancienne interface refaisait toute sa disposition a
/// chaque fois, avec des tailles fixes en pixels qui coupaient les textes des
/// que la fenetre changeait.
class VuePlateau : public QGraphicsView {
    Q_OBJECT
public:
    explicit VuePlateau(QWidget* parent = nullptr);

    ScenePlateau* plateau() const { return la_scene; }

protected:
    void resizeEvent(QResizeEvent* e) override;
    void showEvent(QShowEvent* e) override;

private:
    void ajuster();
    ScenePlateau* la_scene;
};

#endif //MACHI_KORO_VUEPLATEAU_H
