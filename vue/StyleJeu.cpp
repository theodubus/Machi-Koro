#include "StyleJeu.h"

const QString& Style::feuille() {
    static const QString f = R"(
QDialog, QMessageBox, QWidget#menu_minivilles {
    background: #eef5f7;
}
QLabel {
    color: #2f3b46;
    font-size: 14px;
}
QLabel#titre {
    font-size: 24px;
    font-weight: bold;
    color: #1e8a63;
}
QLabel#sous_titre {
    font-size: 13px;
    color: #7b8a97;
}

/* Les boutons ordinaires. Les vignettes de carte sont des QPushButton plats :
   la regle VueCarte plus bas les remet a plat, sinon leur visuel disparaitrait
   derriere ce fond sombre. */
QPushButton {
    background: #2f3b46;
    color: white;
    border: none;
    border-radius: 10px;
    padding: 9px 18px;
    font-size: 14px;
    font-weight: bold;
}
QPushButton:hover   { background: #3d4c59; }
QPushButton:pressed { background: #232c34; }
QPushButton:disabled {
    background: #d6d2c8;
    color: #96928a;
}

VueCarte {
    background: transparent;
    border: 3px solid transparent;
    border-radius: 8px;
    padding: 0;
}
VueCarte:hover   { border-color: #f2b134; }
VueCarte:checked { border-color: #1e8a63; }

QLineEdit, QComboBox {
    background: white;
    border: 1px solid #cfdadf;
    border-radius: 8px;
    padding: 6px 26px 6px 10px;
    min-height: 20px;
    font-size: 14px;
    color: #2f3b46;
    selection-background-color: #41b884;
}
QLineEdit:focus, QComboBox:focus {
    border-color: #41b884;
}
/* Le compteur garde les fleches du style Fusion : habiller son cadre force Qt
   a dessiner lui-meme les deux boutons, et il ne sait pas en tracer les fleches
   a partir de bordures CSS — il les remplace par des carres. On se contente donc
   de sa couleur de fond. */
QSpinBox {
    background: white;
    color: #2f3b46;
    font-size: 14px;
    min-height: 22px;
}

QRadioButton, QCheckBox {
    color: #2f3b46;
    font-size: 14px;
    spacing: 8px;
}

QScrollArea { border: none; background: transparent; }
)";
    return f;
}
