
#ifndef MACHI_KORO_VUEDES_H
#define MACHI_KORO_VUEDES_H



#include <QtGui>
#include <QStyleFactory>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include "Pioche.h"

class VueDes : public QHBoxLayout{
Q_OBJECT
    QLabel *label_de1;
    QLabel *label_de2;
    QMovie *movie_de1;
    QMovie *movie_de2;
    bool anim1_finie;
    bool anim2_finie;
public:
    VueDes(QWidget *parent = nullptr);
    ~VueDes();
    void launch_des(unsigned int de1, unsigned int de2 = 0);
    bool animations_finies() const {return anim1_finie && anim2_finie;}

public slots:
    void frameChanged_Handler1(int frameNumber);
    void frameChanged_Handler2(int frameNumber);

};


#endif //MACHI_KORO_VUEDES_H

