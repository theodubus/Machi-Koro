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
  éditions ; `Pioche` et `Shop` gèrent la distribution. `VuePartie` est la
  fenêtre de jeu, `VueInfo` le journal.
- `joueur/` — `Joueur`, l'état d'un joueur.
- `vue/` — le plateau, une `QGraphicsScene` de taille logique fixe 1600 x 900.
  `ScenePlateau` fait la mise en page et met en scène le tour, `VuePlateau` est
  la `QGraphicsView` qui l'ajuste à la fenêtre, `ItemCarte` et `ItemBouton` sont
  les éléments, `Decor` dessine tout ce qui n'existe pas dans `assets/`,
  `StyleJeu` habille les menus et les fenêtres de choix.
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

Attention à ce que cet outil **ne** couvre **pas** : il vérifie les *données*
(coût, numéros, couleur, type, image), jamais la *logique* des effets. Les 47
`declencher_effet()` ont été relus une fois à la main, confrontés à la
description que porte chaque carte : aucun écart de comportement. Rien
n'automatise ce contrôle, il faut le refaire à la main après toute modification
d'effet.

## Jouer des parties automatiquement

Il n'y a pas de tests unitaires. Le filet de sécurité est une batterie de
parties entièrement pilotées par l'IA, sous AddressSanitizer et
UndefinedBehaviorSanitizer. Le principe :

- compiler les sources du jeu avec un `main` de remplacement qui crée une partie
  100 % IA et fait tourner la boucle d'évènements ;
- neutraliser **quatre** temporisations de `Partie`, **sur une copie de travail**
  et jamais dans le dépôt : `QTimer::singleShot(2000, …)`, `(1000, …)`, et les
  deux constantes `DELAI_EFFET` et `DELAI_ETAPE` qui cadencent le rejeu du tour.
  Sans ces deux dernières, une partie de cent tours dure plusieurs minutes ;
- détecter la fin de partie par `Partie::get_vue_partie() == nullptr`, et penser
  à `app.setQuitOnLastWindowClosed(false)` sinon Qt quitte avant qu'on l'observe.

Couvrir au minimum : Standard seule, + Green Valley, + Marina, + les deux, puis
Deluxe et Custom, sur plusieurs graines. Une partie dure 60 à 160 tours.

**La batterie ne joue que des IA, et ne passe donc jamais par la scène.** Le
parcours humain — cliquer une carte, lire le panneau, cliquer « Construire » —
n'est exercé par rien d'autre qu'une sonde séparée, qui envoie de vrais
évènements souris à la `QGraphicsView` (`QApplication::sendEvent` sur
`viewport()`, position obtenue par `mapFromScene`). Cette sonde fonctionne sous
le greffon `offscreen` : c'est le seul moyen de vérifier que la chaîne clic →
achat → tour suivant tient encore après un remaniement.

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

Le **Club privé** face au Centre commercial a l'air d'un point litigieux, mais
la question est **sans objet**. La carte prend « toutes les pièces » du joueur
actif ; le supplément du Centre commercial est, pour toutes les cartes rouges,
prélevé sur ce même joueur. Une fois sa bourse vidée, il n'y a pas de pièce
supplémentaire à prendre. Les deux lectures — majorer ou non — donnent
exactement le même résultat quelle que soit sa fortune, y compris zéro. Vérifié
par une sonde comparant les deux à 0, 1, 5, 8 et 20 pièces.

Inutile donc de chercher une source, et inutile d'ajouter `ctx.supplement` dans
`ClubPrive::declencher_effet()` : cela ne changerait rien.

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
- Le tour se **résout d'un bloc**, puis se **rejoue** à l'écran. `Partie::declencher()`
  photographie les bourses avant et après chaque `declencher_effet()` et empile un
  `Declenchement` ; `rejouer_effets()` les repasse un par un, avec minuterie, avant
  d'ouvrir la phase de construction. Conséquences à garder en tête :
  - `phase_achat()` n'est plus la fin de `jouer_tour()` mais la fin du rejeu ;
  - les mouvements de pièces sont **déduits** des variations de bourse, aucune
    des 47 cartes n'a été modifiée pour cela ;
  - deux monuments se déclenchent *après* le rejeu, l'Aéroport et le Parc
    d'attractions : `montrer_dernier_effet()` les affiche à part, et doit rester
    **après** le rafraîchissement de `suite_tour()`, qui détruirait leur mise en
    lumière ;
  - les fenêtres modales des cartes à choix s'ouvrent pendant la résolution, donc
    *avant* le rejeu. C'est assumé : les déplacer demanderait de découper les
    quatre boucles de couleur en étapes asynchrones.
- Les objets de la scène sont détruits et recréés par `ScenePlateau::rafraichir()`.
  Tout pointeur gardé vers un `ItemCarte` doit être oublié à ce moment : c'est ce
  que fait `vider()` pour `cartes_projetees`.
- `QGraphicsPixmapItem` n'est **pas** un `QObject` : `QPropertyAnimation(item, "pos")`
  compile mais n'anime rien. Les jetons sont déplacés par un `QVariantAnimation`
  dont on branche `valueChanged`.
- Une carte survolée grandit d'un cinquième et déborde sur ses voisines : il faut
  la passer devant (`setZValue`) à l'entrée et la remettre à sa place à la sortie,
  sinon elle passe *sous* la suivante de la rangée.
- Habiller un `QSpinBox` par feuille de style force Qt à dessiner lui-même ses deux
  boutons, et il ne sait pas tracer une flèche à partir de bordures CSS : il les
  remplace par des carrés. `StyleJeu` laisse donc le compteur au style Fusion.
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
- `EntrepriseRenovation` ferme le bâtiment *choisi par le joueur* plutôt que
  celui trouvé chez chaque adversaire, et `Fleuriste` comme `CaveAVin` sortent
  de leur boucle de comptage par un `break`. Les trois ne sont corrects que
  parce qu'un nom de carte correspond à un seul pointeur (voir ci-dessus).
- Les parties de la batterie ne sont **pas reproductibles** d'une exécution à
  l'autre, même à graine fixée et sans ASLR : le harnais ferme les fenêtres
  modales sur une minuterie, et l'instant où il les intercepte change l'issue.
  Un remaniement ne peut donc pas être validé en rejouant la même partie avant
  et après. À la place : vérifier que chaque ligne modifiée correspond bien à la
  substitution attendue, et sonder les cartes concernées une à une.
- Le greffon `offscreen` résout les chemins relatifs depuis le **répertoire
  courant du processus**, pas depuis l'exécutable. Une sonde lancée d'ailleurs
  que d'un sous-répertoire direct de la racine affiche des cartes vides — c'est
  le repli de `ItemCarte`, pas un bogue.

## Ce qui reste à faire

**Interface** — refaite entièrement. L'ancienne empilait des widgets à tailles
fixes ; le plateau est maintenant une `QGraphicsScene` de taille logique fixe
que la vue ajuste à la fenêtre. `VuePartie` n'en garde que la façade attendue par
le contrôleur et les cartes. `VueShop`, `VueJoueur`, `VueDes` et `VuePioche` ont
disparu, leur travail étant fait par la scène.

Plus aucune fuite attribuable au code du projet — **0 o**, contre 968 o avant la
refonte : le `QWidget` racine de `jouer_partie()` n'existe plus, et `VuePartie`
détruit son journal. Restent 1268 o en 16 allocations, toutes internes à Qt.

Reste, côté confort :

- la boutique laisse une bande de tapis vide quand elle tient sur une rangée ;
- les avatars sont dessinés au trait et ne distinguent que quatre silhouettes,
  alors que l'édition Custom accepte six joueurs — au-delà, elles se répètent, la
  couleur seule les sépare ;
- le journal du tour est écrit pendant la résolution et se trouve donc en avance
  d'un tour sur le rejeu. Le mettre au pas demanderait de tamponner les messages
  des 47 cartes.

**Documentation** — `uml-qt.puml` date de la remise initiale et ne décrit plus le
code : il était déjà périmé avant la refonte graphique (il montre encore
`declencher_effet(possesseur, bonus)` et un type de bâtiment porté par une
chaîne). Le README le signale. Le régénérer est un chantier à part entière.

**Architecture** — traité, dans l'ordre prévu : énumération `type_bat` à la place
des chaînes ; `beneficie_centre_commercial()` posée à la carte ; point d'accroche
`a_l_achat()` sur `Batiment` ; `ContexteDeclenchement` à la place du couple
`(possesseur, bonus)`.

Reste un nettoyage : plusieurs cartes bleues nomment `joueur_actuel` une
variable locale qui désigne en fait le **possesseur** de la carte, pas le joueur
dont c'est le tour.

`Carte::get_nom()` rend l'**identifiant interne** — « HotelDeVille », « ChampBle »
— qui sert de clé partout : recherche d'un monument, comparaison à l'achat, table
de référence. Ce qu'on montre au joueur, c'est `get_nom_affiche()`, adossé à la
table `nom_lisible()` de `cartes/Carte.cpp`. Une carte ajoutée sans y figurer
retombe sur son identifiant : c'est visible, pas silencieux.

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
