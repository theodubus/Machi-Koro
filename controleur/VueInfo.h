#ifndef MACHI_KORO_VUEINFO_H
#define MACHI_KORO_VUEINFO_H


#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLabel>
#include <string>

class VueInfo : public QVBoxLayout {
    public :
        VueInfo(QWidget *parent = nullptr);
        void add_info(const std::string& info);


    private:
        QLabel *info_permament;

        QWidget *widget_layout_info;
        QScrollArea *scroll_info;
        QVBoxLayout *info_layout;
        std::vector<QLabel*> liste_info;
};

#endif //MACHI_KORO_VUEINFO_H
