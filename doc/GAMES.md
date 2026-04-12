# 👾 Créer un Jeu compatible

Cette borne d'arcade est conçue pour accueillir n'importe quel jeu, tant qu'il respecte l'interface stricte `IGame.hpp`.

## 1. Hériter de l'interface
Votre classe de jeu (ex: `Pacman`) doit hériter publiquement de `Arcade::IGame` et redéfinir **toutes** ses méthodes virtuelles pures :

- `void init()` : Appelée au lancement du jeu. Initialisez vos variables et positions ici.
- `void update()` : Appelée en continu par le Core. C'est ici que vous devez gérer **le temps de votre jeu** (avec `std::chrono`) et mettre à jour vos objets.
- `void handleInput(Arcade::Input)` : Reçoit la touche pressée par le joueur.
- `const std::vector<Arcade::Object>& getEntities() const` : Doit renvoyer la liste de tous les objets à dessiner sur la grille.

## 2. Le Système de Grille
Le jeu ne gère pas de pixels. Il renvoie des entités `Arcade::Object` contenant une position `x` et `y` sur une grille abstraite. Le module graphique se chargera de la mise à l'échelle.
Vous pouvez spécifier une couleur de secours (`color`) et un chemin vers une image PNG (`sprite_path`).

## 3. La Signature de la Librairie (Obligatoire)
Pour que votre fichier `.so` soit reconnu par le Core comme un jeu valide, vous devez **impérativement** ajouter ce bloc C à la fin de votre fichier `.cpp` :

```cpp
extern "C" {
    // Instancie votre jeu et le renvoie au Core
    Arcade::IGame *entryPoint() {
        return new Arcade::VotreJeu();
    }

    // Indique au Core qu'il s'agit d'un jeu
    const char *getType() {
        return "game";
    }
}
```

## 4. Jeux déja présents
Voici la liste des jeux présents fourni avec le programme:

- `Snake`
- `Minesweeper`

---

⬅️ **[Retour au sommaire principal](../README.md)**
