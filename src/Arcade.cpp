/*
** EPITECH PROJECT, 2026
** Arcade
** File description:
** Arcade
*/

#include "Arcade.hpp"

Arcade::Core::Core(const std::string &initialLibrary) : currentLibrary(initialLibrary), _isRunning(false), _isMenu(true), _playerName("Player1"), _selectedGraphic(0), _selectedGame(0)
{
    if (initialLibrary.find("ncurses") == std::string::npos && 
        initialLibrary.find("sdl2") == std::string::npos && 
        initialLibrary.find("sfml") == std::string::npos) {
        std::cerr << "Invalid initial library: " << initialLibrary << std::endl;
        return;
    }

    _displayLoaders[initialLibrary] = std::make_unique<DLLoader<IDisplay>>(initialLibrary);

    if (!_displayLoaders[initialLibrary]->isOpen())
        throw std::runtime_error("Failed to load library: " + initialLibrary);

    _display.reset(_displayLoaders[initialLibrary]->getInstance("entryPoint"));

    if (!_display)
        throw std::runtime_error("Failed to load display module from library: " + initialLibrary);
        
    _display->init();
    loadLibs();

    for (size_t i = 0; i < _graphicLibs.size(); i++) {
        if ("./lib/" + _graphicLibs[i] == initialLibrary) {
            _selectedGraphic = i;
            return;
        }
    }
}

Arcade::Core::~Core()
{
}

void Arcade::Core::loadLibs()
{
    _graphicLibs.clear();
    _gameLibs.clear();

    for (const auto & entry : std::filesystem::directory_iterator("./lib")) {
        if (entry.path().extension() == ".so") {
            std::string filename = entry.path().filename().string();
            
            if (filename.find("ncurses") != std::string::npos || 
                filename.find("sdl2") != std::string::npos || 
                filename.find("sfml") != std::string::npos) {
                _graphicLibs.push_back(filename);
            } else {
                _gameLibs.push_back(filename);
            }
        }
    }
}

void Arcade::Core::switchGraphicalLibrary(const std::string &libPath)
{
    if (currentLibrary == libPath)
        return;
    
    if (_display) {
        _display->stop();
        _display.reset();
    }

    if (_displayLoaders.find(libPath) == _displayLoaders.end()) {
        _displayLoaders[libPath] = std::make_unique<DLLoader<IDisplay>>(libPath);
        
        if (!_displayLoaders[libPath]->isOpen()) {
            throw std::runtime_error("Failed to load library: " + libPath);
        }
    }

    _display.reset(_displayLoaders[libPath]->getInstance("entryPoint"));
    if (!_display) {
        throw std::runtime_error("Failed to load display module from library: " + libPath);
    }
    _display->init();
    currentLibrary = libPath;
}

void Arcade::Core::handleMenuInput(Arcade::Input input)
{
    if (input == Arcade::Input::UP && _selectedGame > 0) {
        _selectedGame--;
    }
    if (input == Arcade::Input::DOWN && _selectedGame < _gameLibs.size() - 1) {
        _selectedGame++;
    }

    if (input == Arcade::Input::LEFT && _selectedGraphic > 0) {
        _selectedGraphic--;
    }
    if (input == Arcade::Input::RIGHT && _selectedGraphic < _graphicLibs.size() - 1) {
        _selectedGraphic++;
    }

    if (input == Arcade::Input::ACTION) {
        if (!_graphicLibs.empty()) {
            switchGraphicalLibrary("./lib/" + _graphicLibs[_selectedGraphic]);
        }
        if (!_gameLibs.empty()) {
           std::cout << "Lancement du jeu : " << _gameLibs[_selectedGame] << " avec " << currentLibrary << std::endl;
            loadGame("./lib/" + _gameLibs[_selectedGame]);
        }
    }
}

void Arcade::Core::displayMenu()
{
    _display->drawText("=== ARCADE MAIN MENU ===", {17, 2}, Arcade::Color::RED);
    
    _display->drawText("[ GRAPHICAL LIBRARIES ]", {5, 5}, Arcade::Color::BLUE);
    int yOffset = 7;
    for (size_t i = 0; i < _graphicLibs.size(); ++i) {
        bool isSelected = (i == _selectedGraphic);
        std::string prefix = isSelected ? "-> " : "   ";
        Arcade::Color color = isSelected ? Arcade::Color::RED : Arcade::Color::WHITE;
        _display->drawText(prefix + _graphicLibs[i], {5, yOffset}, color);
        yOffset++;
    }

    _display->drawText("[ GAME LIBRARIES ]", {35, 5}, Arcade::Color::BLUE);
    yOffset = 7;
    for (size_t i = 0; i < _gameLibs.size(); ++i) {
        bool isSelected = (i == _selectedGame);
        std::string prefix = isSelected ? "-> " : "   ";
        Arcade::Color color = isSelected ? Arcade::Color::RED : Arcade::Color::WHITE;
        _display->drawText(prefix + _gameLibs[i], {35, yOffset}, color);
        yOffset++;
    }

    _display->drawText("[Player Name]: " + _playerName, {17, yOffset + 3}, Arcade::Color::GREEN);
    _display->drawText("[ START ]", {25, yOffset + 5}, Arcade::Color::WHITE);

    _display->drawText("RIGHT/LEFT ARROWS: Navigate graphical libraries.", {5, yOffset + 7}, Arcade::Color::WHITE);
    _display->drawText("UP/DOWN ARROWS: Navigate game libraries.", {5, yOffset + 9}, Arcade::Color::WHITE);
    _display->drawText("ENTER: Start game.", {5, yOffset + 11}, Arcade::Color::WHITE);
}

void Arcade::Core::loadGame(const std::string &gamePath)
{
    if (_game) {
        _game->stop();
        _game.reset(); 
    }

    _gameLoader = std::make_unique<DLLoader<IGame>>(gamePath);
    if (!_gameLoader->isOpen()) {
        throw std::runtime_error("Failed to load game library: " + gamePath);
    }
    _game.reset(_gameLoader->getInstance("entryPoint"));
    if (!_game) {
        throw std::runtime_error("Failed to retrieve game instance from library: " + gamePath);
    }

    _game->init();
    _isMenu = false; 
}

bool Arcade::Core::run()
{
    _isRunning = true;

    if (!_display) {
        return false;
    }

    while (_isRunning) {
        Arcade::Input input = _display->getInput();

        if (input == Arcade::Input::EXIT) {
            _isRunning = false;
        }
    
        _display->clear();

        if (_isMenu) {
            displayMenu();
            handleMenuInput(input);
        } else {

            _game->handleInput(input);

            _game->update();

            for (const auto &obj : _game->getEntities()) {
                _display->drawObject(obj);
            }

            if (input == Arcade::Input::MENU) {
                _isMenu = true;
                _game->stop();
                _game.reset();
            }
        }
        _display->display();
    }
    return true;
}

int main(int ac, char **av)
{
    if (ac != 2) {
        std::cout << "USAGE" << std::endl;
        std::cout << "\t./arcade ./lib/arcade_[ncurses|sfml|sdl2].so" << std::endl;
        return 84;
    }

    Arcade::Core core(av[1]);

    int result = core.run();
    if (!result)
        return 84;
    return 0;
}