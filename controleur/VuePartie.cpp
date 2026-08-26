#include "VuePartie.h"
#include "VuePlateau.h"
#include "Partie.h"

#include <QVBoxLayout>
#include <QMessageBox>

using namespace std;

VuePartie::VuePartie(QWidget *parent) : QWidget(parent) {
    setWindowTitle("Minivilles");

    plateau = new VuePlateau(this);
    auto* structure = new QVBoxLayout(this);
    structure->setContentsMargins(0, 0, 0, 0);
    structure->addWidget(plateau);

    infos = new VueInfo(plateau->plateau());

    connect(plateau->plateau(), &ScenePlateau::achat_demande,
            this, &VuePartie::construire);
    connect(plateau->plateau(), &ScenePlateau::rien_faire,
            this, &VuePartie::ne_rien_construire);

    // La fermeture de la fenetre termine l'application : c'est la seule fenetre
    // de jeu. L'attribut programme aussi sa destruction, ce dont depend
    // Partie::terminer_tour().
    setAttribute(Qt::WA_DeleteOnClose);
    resize(1600, 900);

    plateau->plateau()->rafraichir();
}

VuePartie::~VuePartie() {
    delete infos;
}

// --------------------------------------------------------------- rafraichir
//
// Le controleur appelle ces methodes une par une, heritage du temps ou chaque
// zone etait un widget separe. La scene se reconstruit d'un bloc : elles font
// donc toutes la meme chose.

void VuePartie::update_vue_partie() { plateau->plateau()->rafraichir(); }
void VuePartie::update_vue_joueur() { plateau->plateau()->rafraichir(); }
void VuePartie::update_vue_shop()   { plateau->plateau()->rafraichir(); }
void VuePartie::update_vue_pioche() { plateau->plateau()->rafraichir(); }
void VuePartie::update_nom_joueur() { plateau->plateau()->rafraichir(); }

void VuePartie::update_vue_info() {
    /// Le journal est cumulatif : il se remplit par add_info() et n'a rien a
    /// reconstruire.
}

void VuePartie::update_des() {
    plateau->plateau()->montrer_des();
}

void VuePartie::set_bouton_rien_faire(bool b) {
    plateau->plateau()->set_moment_achat(b);
}

void VuePartie::set_phase(ScenePlateau::Phase p) {
    plateau->plateau()->set_phase(p);
}

void VuePartie::projeter(const Carte* carte, const std::vector<unsigned int>& possesseurs,
                         const QString& explication) {
    plateau->plateau()->projeter(carte, possesseurs, explication);
}

void VuePartie::eteindre_projecteur() {
    plateau->plateau()->eteindre_projecteur();
}

void VuePartie::animer_piece(int source, int destination, int montant) {
    plateau->plateau()->animer_piece(source, destination, montant);
}

// -------------------------------------------------------------------- achat

void VuePartie::construire(const Carte* carte) {
    Partie* partie = Partie::get_instance();
    // La phase d'achat peut avoir ete fermee entre le clic et l'arrivee ici :
    // une minuterie du tour, ou une seconde confirmation trop rapide.
    if (!partie->get_moment_achat() || carte == nullptr) return;
    partie->set_moment_achat(false);

    // Un achat refuse (carte trop chere ou disparue de la boutique) doit laisser
    // au joueur le benefice du « rien construit », dont depend l'effet de
    // l'Aeroport : c'est le resultat de acheter_carte() qui en decide.
    const bool achat_reussi = partie->acheter_carte(carte);

    set_bouton_rien_faire(false);
    update_vue_partie();
    partie->suite_tour(achat_reussi);
}

void VuePartie::ne_rien_construire() {
    Partie* partie = Partie::get_instance();
    if (!partie->get_moment_achat()) return;

    // Passer son tour de construction n'est pas rattrapable : on demande
    // confirmation, comme avant.
    const QMessageBox::StandardButton reponse = QMessageBox::question(
            this, "Ne rien construire",
            "Voulez-vous vraiment terminer votre tour sans rien construire ?",
            QMessageBox::Yes | QMessageBox::No);
    if (reponse != QMessageBox::Yes) return;

    partie->set_moment_achat(false);
    set_bouton_rien_faire(false);
    partie->suite_tour(false);
}
