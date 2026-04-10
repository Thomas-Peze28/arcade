# 🖥️ Créer un Module Graphique

La borne d'arcade peut basculer dynamiquement d'une technologie d'affichage à une autre (à chaud). Pour créer un nouveau moteur d'affichage (ex: `VulkanDisplay`, `SDL2Display`, ou `NcursesDisplay`), vous devez implémenter l'interface stricte `IDisplay.hpp`.

## 1. Implémenter les méthodes de l'interface

Votre classe doit hériter publiquement de `Arcade::IDisplay` et définir toutes les méthodes virtuelles pures suivantes :

- `void init()` : Ouvre la fenêtre ou initialise le terminal. C'est ici qu'on charge les polices d'écriture générales si besoin.
- `void stop()` : Détruit la fenêtre et libère proprement la mémoire (textures, polices, renderers).
- `void clear()` : Nettoie l'écran (généralement appelé au tout début d'une frame).
- `void display()` : Affiche le rendu final à l'écran (Swap buffers).
- `Arcade::Input getInput()` : Traduit l'événement natif de la librairie cible (ex: `sf::Event::KeyPressed`) en un ENUM `Arcade::Input` compréhensible par le Core. Doit renvoyer `Arcade::Input::NONE` si aucune touche n'est pressée.
- `void drawObject(const Arcade::Object &obj)` : La méthode de rendu principale pour les éléments de jeu.
- `void drawText(const std::string &text, const Arcade::Position &pos, const Arcade::Color &color)` : Méthode utilisée principalement pour afficher les éléments de l'interface et du menu principal.

## 2. Gérer les Sprites et le "Fallback" (Couleurs)

Dans la méthode `drawObject`, l'objet fourni par le Core (venant du jeu) contient à la fois une couleur, un caractère, et potentiellement un chemin vers un sprite (`sprite_path`).

**Règle d'or de l'affichage :** Votre module doit être résilient. Il doit d'abord essayer de charger et d'afficher le sprite via le `sprite_path`. Si le chemin est vide, si l'image est introuvable, ou si votre librairie (ex: Ncurses) ne gère pas les images, vous **devez** basculer sur un affichage de secours (Fallback) en utilisant la couleur et/ou le caractère fournis.

*Note de performance : Veillez à utiliser un système de "Cache" (comme une `std::map`) pour vos textures afin de ne pas recharger les images depuis le disque dur à chaque frame.*

## 3. La Signature de la Librairie (Point d'entrée dynamique)

Pour que votre fichier `.so` compilé soit reconnu par le Core comme un module d'affichage valide lors du chargement avec `dlopen`, vous devez **impérativement** ajouter ce bloc de code C à la fin de votre fichier `.cpp` :

```cpp
extern "C" {
    // Instancie votre affichage et le renvoie au Core sous forme d'interface
    Arcade::IDisplay *entryPoint() {
        return new Arcade::VotreAffichage();
    }

    // Indique au Core qu'il s'agit bien d'une librairie d'affichage
    const char *getType() {
        return "display";
    }
}
```

## 4. Modules graphiques déja présents
Voici la liste des modules graphiques présents fourni avec le programme:

- `NCURSES`
- `SFML`
- `SDL2`
- `Allegro5`

---

⬅️ **[Retour au sommaire principal](../README.md)**


