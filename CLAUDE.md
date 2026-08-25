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
- `Joueur::get_liste_batiment()` renvoie ses `map` **par valeur**. Itérer dessus
  est sûr même si l'on modifie le joueur pendant la boucle, mais c'est coûteux.
- Le déroulement du tour passe par des `QTimer::singleShot`. Ne pas revenir à des
  appels directs : `jouer_tour → acheter_carte_ia → suite_tour → jouer_tour`
  empilait la partie entière sur la pile d'appels.
- `VuePartie` porte `Qt::WA_DeleteOnClose`. La fermer programme sa destruction :
  ne jamais l'utiliser comme parent après l'avoir fermée.

## Ce qui reste à faire

**Interface** — la vue écrit dans le modèle (`VuePartie::update_des()` appelle
`set_moment_achat(true)`) ; le journal de partie est détruit et recréé vide à
chaque tour ; les rafraîchissements emploient `addWidget()` au lieu de
`replaceWidget()`, ce qui décale les widgets ; environ 460 Ko fuient par partie
dans la reconstruction des vues ; `VueInfo::add_info()` n'oublie jamais un
message ; les pièces posées sur la Startup ne sont pas affichées.

**Architecture**, après l'interface — sortir les effets des cartes du contrôleur.
Dans l'ordre : remplacer les chaînes de type par une énumération ; faire du bonus
du Centre commercial une question posée à la carte ; ajouter un point d'accroche
« à l'achat » sur `Batiment` pour la Banque de Minivilles ; enfin remplacer
`declencher_effet(possesseur, bonus)` par un contexte de déclenchement, seul
changement qui touche les 47 signatures.

## Conventions

- Commits et documentation en français.
- Ne mentionner aucun nom de modèle dans ce qui part sur le dépôt.
- Les messages de commit expliquent le *pourquoi* et citent la source d'une
  décision de règle.
