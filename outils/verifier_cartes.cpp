/// Verifie que chaque carte du jeu est conforme a sa carte physique.
///
/// La reference est cartes/donnees_cartes.h, transcrit depuis les visuels du
/// dossier assets/. Ce programme instancie toutes les cartes de toutes les
/// editions et compare, champ par champ : cout, numeros d'activation, couleur,
/// type et chemin d'image. Il signale aussi les cartes presentes d'un cote et
/// absentes de l'autre, ainsi que les fichiers image manquants.
///
/// Utilisation :
///   cmake --build build --target verifier_cartes
///   ./build/verifier_cartes            (depuis un sous-repertoire du depot)
///
/// Rend 0 si tout concorde, 1 sinon.

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "donnees_cartes.h"
#include "EditionDeJeu.h"

using namespace std;

namespace {

    unsigned int erreurs = 0;

    void signaler(const string& carte, const string& champ,
                  const string& attendu, const string& obtenu) {
        cout << "  ECART  " << carte << "\n"
             << "         " << champ << " : la carte dit « " << attendu
             << " », le code dit « " << obtenu << " »\n";
        erreurs++;
    }

    string joindre(const vector<unsigned int>& v) {
        string s;
        for (size_t i = 0; i < v.size(); i++) {
            if (i) s += "-";
            s += to_string(v[i]);
        }
        return s.empty() ? "aucun" : s;
    }

    string nom_couleur(couleur_bat c) {
        switch (c) {
            case Bleu:   return "Bleu";
            case Rouge:  return "Rouge";
            case Vert:   return "Vert";
            case Violet: return "Violet";
        }
        return "?";
    }

    /// Rassemble toutes les cartes instanciees par les editions et extensions.
    /// Une meme carte peut apparaitre dans plusieurs editions : on ne garde
    /// qu'un exemplaire, l'objet etant identique.
    void collecter(map<string, Batiment*>& bats, map<string, Monument*>& mons,
                   vector<EditionDeJeu*>& proprietaires) {
        for (const char* nom : {"Standard", "Deluxe", "Custom", "Marina", "GreenValley"}) {
            auto* ed = new EditionDeJeu(nom);
            proprietaires.push_back(ed);
            for (auto& b : ed->get_batiment())
                bats.emplace(b.first->get_nom(), b.first);
            for (auto* m : ed->get_monument())
                mons.emplace(m->get_nom(), m);
        }
    }

} // namespace

int main() {
    cout << "\nVerification des cartes contre cartes/donnees_cartes.h\n"
         << string(64, '-') << "\n";

    map<string, Batiment*> bats;
    map<string, Monument*> mons;
    vector<EditionDeJeu*> proprietaires;
    collecter(bats, mons, proprietaires);

    set<string> vus;

    // --- Batiments ---
    for (const auto& attendu : donnees_cartes::batiments()) {
        vus.insert(attendu.nom);
        auto it = bats.find(attendu.nom);
        if (it == bats.end()) {
            cout << "  ABSENTE  " << attendu.nom
                 << " figure dans la table mais aucune edition ne la cree\n";
            erreurs++;
            continue;
        }
        const Batiment* obtenu = it->second;

        if (obtenu->get_prix() != attendu.prix)
            signaler(attendu.nom, "cout de construction",
                     to_string(attendu.prix), to_string(obtenu->get_prix()));

        vector<unsigned int> des(obtenu->get_num_activation().begin(),
                                 obtenu->get_num_activation().end());
        sort(des.begin(), des.end());
        vector<unsigned int> des_attendus = attendu.des;
        sort(des_attendus.begin(), des_attendus.end());
        if (des != des_attendus)
            signaler(attendu.nom, "numeros d'activation",
                     joindre(des_attendus), joindre(des));

        if (nom_couleur(obtenu->get_couleur()) != attendu.couleur)
            signaler(attendu.nom, "couleur",
                     attendu.couleur, nom_couleur(obtenu->get_couleur()));

        if (nom_type(obtenu->get_type()) != attendu.type)
            signaler(attendu.nom, "type (icone de la carte)",
                     attendu.type, nom_type(obtenu->get_type()));

        const string chemin_attendu =
                string("../assets/batiments/") + attendu.couleur + "/" + attendu.image;
        if (obtenu->get_path_image() != chemin_attendu)
            signaler(attendu.nom, "chemin d'image",
                     chemin_attendu, obtenu->get_path_image());
        else if (!filesystem::exists(obtenu->get_path_image()))
            signaler(attendu.nom, "fichier image",
                     "present", "introuvable depuis le repertoire courant");
    }

    for (const auto& b : bats)
        if (!vus.count(b.first)) {
            cout << "  INCONNUE  " << b.first
                 << " est creee par une edition mais absente de la table\n";
            erreurs++;
        }

    // --- Monuments ---
    vus.clear();
    for (const auto& attendu : donnees_cartes::monuments()) {
        vus.insert(attendu.nom);
        auto it = mons.find(attendu.nom);
        if (it == mons.end()) {
            cout << "  ABSENT  " << attendu.nom
                 << " figure dans la table mais aucune edition ne le cree\n";
            erreurs++;
            continue;
        }
        const Monument* obtenu = it->second;

        if (obtenu->get_prix() != attendu.prix)
            signaler(attendu.nom, "cout de construction",
                     to_string(attendu.prix), to_string(obtenu->get_prix()));

        const string chemin_attendu = string("../assets/monuments/") + attendu.image;
        if (obtenu->get_path_image() != chemin_attendu)
            signaler(attendu.nom, "chemin d'image",
                     chemin_attendu, obtenu->get_path_image());
        else if (!filesystem::exists(obtenu->get_path_image()))
            signaler(attendu.nom, "fichier image",
                     "present", "introuvable depuis le repertoire courant");
    }

    for (const auto& m : mons)
        if (!vus.count(m.first)) {
            cout << "  INCONNU  " << m.first
                 << " est cree par une edition mais absent de la table\n";
            erreurs++;
        }

    cout << string(64, '-') << "\n"
         << "  " << donnees_cartes::batiments().size() << " batiments et "
         << donnees_cartes::monuments().size() << " monuments verifies\n";
    if (erreurs == 0)
        cout << "  Aucun ecart.\n\n";
    else
        cout << "  " << erreurs << " ecart(s) a corriger.\n\n";

    for (auto* ed : proprietaires) delete ed;
    return erreurs == 0 ? 0 : 1;
}
