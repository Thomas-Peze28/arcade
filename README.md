# 🕹️ Arcade - Projet C++

Bienvenue sur le projet Arcade ! Ce projet est une borne d'arcade modulaire développée en C++20. 
L'objectif principal est de fournir une architecture permettant de charger dynamiquement des jeux et des bibliothèques graphiques à l'exécution, sans jamais avoir à recompiler le programme principal grâce à l'utilisation des librairies dynamiques.

## 📚 Sommaire de la documentation

* **[🎮 Contrôles](doc/CONTROLS.md)** : Présentation des contrôles et touches pris en charge.

* **[👾 Créer un Jeu (API IGame)](doc/GAMES.md)** : Indication sur l'implémentation de nouveaux jeux.

* **[📺 Créer un Module Graphique (API IDisplay)](doc/GRAPHICS.md)** : Indication sur l'implémentation de nouveaux modules graphiques.

## 🚀 Compilation

Le projet utilise un `Makefile` standard. À la racine du projet, vous pouvez utiliser les commandes suivantes :

- `make` : Compile l'intégralité du projet (Core, Jeux et Graphismes).
- `make core` : Compile uniquement l'exécutable principal.
- `make games` : Compile les librairies de jeux (ex: Snake) dans le dossier `./lib/`.
- `make graphicals` : Compile les librairies d'affichage (SFML, SDL2, Ncurses) dans le dossier `./lib/`.
- `make re` : Nettoie et recompile tout le projet.

## 🎮 Utilisation

Pour lancer la borne d'arcade, exécutez le binaire en lui passant en paramètre une librairie graphique initiale :

```bash
./arcade [graphic_lib.so]
```

## Auteurs 👥

- [@Romane Cauvez](https://github.com/Romanecauvez)
- [@Thomas Peze](https://github.com/Thomas-Peze28)
- [@Jean-Baptise Lamy-Floret](https://github.com/007TheSpectre)

## Interface partagée 👥

- @Fabien Skotarek
- @Enzo Sénéchal
- [@Florent Windels](mailto:florent.windels@epitech.eu)