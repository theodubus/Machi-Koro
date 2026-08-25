# Notes de travail sur ce dépôt

Contexte accumulé au fil des sessions de correction. À lire avant de toucher au
code : plusieurs décisions ci-dessous ont été prises sources en main, et les
refaire autrement serait une régression.

## Le projet

Implémentation C++23 / Qt6 / CMake du jeu de société *Minivilles* (*Machi Koro*),
projet de LO21 à l'UTC. ~62 en-têtes et 62 sources, plus les visuels des cartes.

- `cartes/` — modèle des cartes. Un fichier par carte, rangé par couleur.
  `Carte` est la classe de base abstraite, `Batiment` et `Monument` en dérivent.
- `controleur/` — `Partie` (singleton) mène le jeu ; `EditionDeJeu` compose les
  éditions ; `Pioche` et `Shop` gèrent la distribution. Les `Vue*.cpp` du même
  dossier sont les vues Qt.
- `joueur/` — `Joueur` (état) et `VueJoueur` (sa ville à l'écran).
- `assets/` — visuels des cartes, des monuments et des dés.

## Compiler et lancer

```bash
cmake -S . -B build -G Ninja
cmake --build build
cd build && ./Machi_Koro
```

**Le binaire doit être lancé depuis un sous-répertoire direct de la racine.**
Les visuels sont référencés en relatif sous la forme `../assets/...` : lancé
d'ailleurs, le jeu démarre mais toutes les cartes s'affichent vides.

## Vérifier les données des cartes

```bash
cmake --build build --target verifier_cartes
cd build && ./verifier_cartes
```

`cartes/donnees_cartes.h` est la transcription des 39 bâtiments et 8 monuments
depuis leurs visuels : coût, numéros d'activation, couleur, type, image.
L'outil instancie toutes les cartes du jeu et compare champ par champ. Il rend 1
en cas d'écart, donc il est utilisable en intégration continue.

**En cas de divergence, c'est la carte qui a raison.** On corrige le code, pas
la table — sauf si la transcription elle-même est fautive, auquel cas il faut
regarder le visuel dans `assets/` avant de trancher.

## Jouer des parties automatiquement

Il n'y a pas de tests unitaires. Le filet de sécurité est une batterie de
parties entièrement pilotées par l'IA, sous AddressSanitizer et
UndefinedBehaviorSanitizer. Le principe :

- compiler les sources du jeu avec un `main` de remplacement qui crée une partie
  100 % IA et fait tourner la boucle d'évènements ;
- neutraliser les deux temporisations de `Partie` (`QTimer::singleShot(2000, …)`
  et `(1000, …)`) **sur une copie de travail**, jamais dans le dépôt ;
- détecter la fin de partie par `Partie::get_vue_partie() == nullptr`, et penser
  à `app.setQuitOnLastWindowClosed(false)` sinon Qt quitte avant qu'on l'observe.

Couvrir au minimum : Standard seule, + Green Valley, + Marina, + les deux, puis
Deluxe et Custom, sur plusieurs graines. Une partie dure 60 à 160 tours.

## Décisions de règles, avec leur source

Ne pas revenir dessus sans une source au moins équivalente.

| Point | Valeur | Source |
|---|---|---|
| Ordre de résolution | rouge, puis bleu/vert, puis violet | livret |
| Revenu minimum si on n'achète rien | n'existe pas | livret |
| Victoire Standard | 4 monuments | livret |
| Victoire Standard + Marina | 6 monuments | livret |
| Victoire Deluxe | 5 monuments, 2 à 5 joueurs | éditeur |
| Joueurs, base et extensions | 2 à 4 | livret |
| Jet du Chalutier | un seul par tour, partagé par tous | livret |
| Bonus du Port | +2 au total, jamais +1 par dé | livret |
| Établissements fermés | comptent toujours pour les autres cartes | livret |
| Réouverture | bleu : tour de n'importe qui ; rouge : tour des autres ; vert et violet : son propre tour | livret |
| Cartes liées au Port | exigent un Port **construit** | livret et cartes |
| Hôtel de ville, Fabrique du Père Noël | monuments offerts construits, hors condition de victoire | livret et fiche produit |
| Fabrique : lancé cassé | 3 pièces **et** relance du même nombre de dés | règle de la carte |

Deux valeurs ne reposent sur aucune source publiée, et ne le peuvent pas :

- **l'édition Custom** (2 à 6 joueurs, 6 monuments) est une création du projet ;
  les valeurs d'origine des auteurs ont été conservées ;
- **le « lancé cassé »** de la Fabrique du Père Noël simule un dé qui bascule sur
  une arête, impossible en numérique : quatre dés séparés totalisant 16, soit
  environ 9,6 % des tours.

## Pièges connus

- `Partie::possede_monument()` retrouve la **carte** d'un monument, construit ou
  non. Pour savoir s'il est bâti, c'est `Joueur::monument_construit()`.
- `Joueur::get_monument_jouables()` inclut les monuments offerts. Pour la
  victoire et pour les cartes qui comptent les monuments, c'est
  `Joueur::nb_monuments_construits()`.
- Les objets `Batiment` sont **partagés entre les joueurs** : un même pointeur
  sert à tous. Aucun état propre à un joueur ne peut y vivre. C'est pourquoi les
  jetons de la Startup sont stockés dans `Joueur`.
- `Joueur::get_liste_batiment()` renvoie ses `map` **par valeur**, et il faut que
  ça reste ainsi. Les boucles de résolution des effets itèrent dessus tout en
  déclenchant des cartes qui modifient la ville du joueur : l'Entreprise de
  déménagement, **verte**, retire un bâtiment pendant la boucle verte ;
  l'Entreprise de rénovation et le MGA Game Center en ferment pendant la boucle
  violette. Rendre une référence invaliderait l'itérateur de la boucle en cours.
  L'audit avait classé cette copie en défaut de qualité : c'est une erreur, la
  corriger introduirait un comportement indéfini.
- Le déroulement du tour passe par des `QTimer::singleShot`. Ne pas revenir à des
  appels directs : `jouer_tour → acheter_carte_ia → suite_tour → jouer_tour`
  empilait la partie entière sur la pile d'appels.
- `VuePartie` porte `Qt::WA_DeleteOnClose`. La fermer programme sa destruction :
  ne jamais l'utiliser comme parent après l'avoir fermée.
- `QWidget::setStyle()` **ne prend pas** la propriété du style qu'on lui passe,
  contrairement à `QApplication::setStyle()`. Le style Fusion est posé une fois
  dans `main()` et tous les widgets en héritent : un `setStyle()` par widget
  construit un second objet identique que personne ne libère.
- `QLayout::replaceWidget()` rend l'élément de disposition qui contenait
  l'ancien widget. Il appartient à l'appelant, qui doit le détruire.
- Ne rien libérer dans le destructeur de `VueDes` : ses `QLabel` appartiennent à
  la disposition et ses `QMovie` à `VuePartie`, détruite dans un ordre non
  garanti.
- Une taille imposée par `setFixedSize()` sur une étiquette coupe le texte dès
  qu'il s'allonge (nom de joueur saisi, extensions cumulées). Préférer
  `setMinimumSize()` et `setWordWrap()`.
- Le greffon Qt `offscreen` simule un écran de 800x600 : une fenêtre maximisée y
  reste enfermée et tout se chevauche. Pour juger une mise en page, sortir de
  l'état maximisé puis `resize(1920, 1080)` avant `grab()`.
- `Partie::ajout_batiment()` **dédoublonne par nom** avant de cloner : dans une
  partie donnée, un nom de carte correspond à exactement un `Batiment*`. Comme
  `Joueur::ajouter_batiment()` compare les pointeurs, un joueur n'a jamais deux
  entrées pour la même carte. Plusieurs cartes en dépendent, dont le `break` du
  Fleuriste. Un test qui construit ses cartes avec `new` casse cette hypothèse
  et mesure autre chose que le jeu.
- Les parties de la batterie ne sont **pas reproductibles** d'une exécution à
  l'autre, même à graine fixée et sans ASLR : le harnais ferme les fenêtres
  modales sur une minuterie, et l'instant où il les intercepte change l'issue.
  Un remaniement ne peut donc pas être validé en rejouant la même partie avant
  et après. À la place : vérifier que chaque ligne modifiée correspond bien à la
  substitution attendue, et sonder les cartes concernées une à une.

## Ce qui reste à faire

**Interface** — traité. Il reste 968 o qui fuient une fois par partie
(`Partie::jouer_partie()`, le `QWidget` racine qui porte `VuePartie`) : la
quantité ne croît pas avec la durée de la partie, et changer la propriété de la
fenêtre de plus haut niveau demande de valider le comportement graphique réel,
hors de portée d'un essai sans écran.

Reste aussi, côté confort : la colonne de gauche (dés, pioche) laisse une large
zone vide, et l'image d'entête est affichée à sa taille native sans s'adapter à
la fenêtre.

**Architecture** — traité, dans l'ordre prévu : énumération `type_bat` à la place
des chaînes ; `beneficie_centre_commercial()` posée à la carte ; point d'accroche
`a_l_achat()` sur `Batiment` ; `ContexteDeclenchement` à la place du couple
`(possesseur, bonus)`.

Reste un nettoyage : plusieurs cartes bleues nomment `joueur_actuel` une
variable locale qui désigne en fait le **possesseur** de la carte, pas le joueur
dont c'est le tour.

Le contrôleur ne branche plus sur le nom d'un **bâtiment** pour lui appliquer un
effet. Il cite encore dix cartes par leur nom, mais pour autre chose : les six
monuments, dont il ordonne l'activation au fil du tour (`CentreCommercial`,
`Gare`, `TourRadio`, `Port`, `ParcAttraction`, `Aeroport`), les deux monuments
offerts (`HotelDeVille`, `FabriqueDuPereNoel`) et la main de départ (`ChampBle`,
`Boulangerie`). Sortir la séquence d'activation des monuments du contrôleur
serait le chantier suivant.

## Conventions

- Commits et documentation en français.
- Ne mentionner aucun nom de modèle dans ce qui part sur le dépôt.
- Les messages de commit expliquent le *pourquoi* et citent la source d'une
  décision de règle.
