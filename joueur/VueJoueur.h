#ifndef MACHI_KORO_VUEJOUEUR_H
#define MACHI_KORO_VUEJOUEUR_H

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QProgressBar>
#include <QLCDNumber>
#include <QString>
#include <QScrollArea>
#include <QMessageBox>
#include "VueCarte.h"
#include "Joueur.h"

class VueJoueur : public QWidget
{
Q_OBJECT
public:
    explicit VueJoueur(Joueur* j,bool e_j_a,  QWidget *parent = nullptr);
    bool get_est_joueur_actuel() const{return est_joueur_actuel;}
    VueCarte* get_carte_choisie() const {return carte_choisie;}

private:
    bool est_joueur_actuel;
    QLabel* text_bat;
    QLabel* nom_joueur; // texte avec le nom du joueur
    QLCDNumber* argent; // affichage de l'argent du joueur
    QHBoxLayout* layout_informations;
    QHBoxLayout* layout_haut_gauche;
    QHBoxLayout* layout_haut_droit;
    QVBoxLayout* layout_droit;
    QVBoxLayout* layout_informations_gauche;
    QGridLayout* layout_batiments; // grille des batiments du joueur
    QGridLayout* layout_monuments; // grille des monuments du joueur
    QGridLayout* layout_batiments_ferme; // grille des batiments ferme du joueur
    vector<VueCarte*>* vue_batiments; // adresses des batiments VueCarte
    vector<VueCarte*>* vue_batiments_ferme; // adresses des batiments fermés VueCarte
    vector<VueCarte*>* vue_monuments; // adresses des monuments VueCarte
    QPushButton *bat_ferme;
    QPushButton* bouton_achat;
    Joueur *joueur;
    VueCarte *carte_choisie;
    QScrollArea* scroll_bat;
    QScrollArea* scroll_mon;
    QWidget *parent;
    QWidget* fenetre_bat_fermes; // Fenetre avec les batiments fermés
    QWidget* widget_scroll_bat;
    QWidget* widget_scroll_mon;

public slots:
    // slots qui gère les clics sur les cartes
    virtual void batimentClique(VueCarte* vc);
    void affichage_bat_ferme();
    void clicked_acheter_event();

    void monumentClique(VueCarte* vc);
};


#endif //MACHI_KORO_VUEJOUEUR_H