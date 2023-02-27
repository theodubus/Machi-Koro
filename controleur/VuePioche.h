#ifndef MACHI_KORO_VUEPIOCHE_H
#define MACHI_KORO_VUEPIOCHE_H


#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include "Pioche.h"

class VuePioche : public QVBoxLayout{
    QLabel* pioche_exception;
    QProgressBar* barre_pioche;
public:
    VuePioche(Pioche *pioche, QWidget *parent = nullptr);
};


#endif //MACHI_KORO_VUEPIOCHE_H
