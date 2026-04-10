/*
** EPITECH PROJECT, 2026
** Arcade
** File description:
** Arcade
*/

#include "Arcade.hpp"

Arcade::Core::Core(const std::string &initialLibrary) : currentLibrary(initialLibrary), _isRunning(false), _isMenu(true), _isGameOver(false), _isWin(false), _playerName("Player1"), _selectedGraphic(0), _selectedGame(0)
{
    DLLoader<void> lib(initialLibrary);
            
    if (lib.isOpen()) {
        std::string type = lib.getLibraryType();
        
        if (type != "display") {
            throw std::runtime_error("Initial library is not a display module: " + initialLibrary);
        }
    } else {
        throw std::runtime_error("Failed to load initial library: " + initialLibrary);
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
            std::string filepath = entry.path().string();
            std::string filename = entry.path().filename().string();
            
            DLLoader<void> lib(filepath);
            
            if (lib.isOpen()) {
                std::string type = lib.getLibraryType();
                
                if (type == "game") {
                    _gameLibs.push_back(filename);
                } 
                else if (type == "display") {
                    _graphicLibs.push_back(filename);
                }
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

void Arcade::Core::loadGame(const std::string &libPath)
{
    if (_game) {
        _game->stop();
        _game.reset();
    }

    _gameLoader = std::make_unique<DLLoader<IGame>>(libPath);
    
    if (!_gameLoader->isOpen()) {
        throw std::runtime_error("Failed to load game library: " + libPath);
    }

    _game.reset(_gameLoader->getInstance("entryPoint"));
    if (!_game) {
        throw std::runtime_error("Failed to load game module from library: " + libPath);
    }
    _game->init();
    _isGameOver = false;
    _isWin = false;
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
            loadGame("./lib/" + _gameLibs[_selectedGame]);
            _isMenu = false;
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

void Arcade::Core::displayGameOver()
{
    _display->drawText("GAME OVER", {20, 10}, Arcade::Color::RED);
    _display->drawText("Returning to menu...", {15, 12}, Arcade::Color::WHITE);
}

void Arcade::Core::displayWin()
{
    _display->drawText("YOU WIN!", {20, 10}, Arcade::Color::GREEN);
    _display->drawText("Returning to menu...", {15, 12}, Arcade::Color::WHITE);
}

void Arcade::Core::handleInput(Arcade::Input input)
{
    if (input == Arcade::Input::EXIT)
            _isRunning = false;
     if (input == Arcade::Input::MENU) {
        _isMenu = true;
        if (_game) {
            _game->stop();
            _game.reset();
        }
    }
    if (input == Arcade::Input::NEXT_GRAPHIC) {
        if (!_graphicLibs.empty()) {
            _selectedGraphic = (_selectedGraphic + 1) % _graphicLibs.size();
            switchGraphicalLibrary("./lib/" + _graphicLibs[_selectedGraphic]);
        }
    }
    if (input == Arcade::Input::PREV_GRAPHIC) {
        if (!_graphicLibs.empty()) {
            _selectedGraphic = (_selectedGraphic + _graphicLibs.size() - 1) % _graphicLibs.size();
            switchGraphicalLibrary("./lib/" + _graphicLibs[_selectedGraphic]);
        }
    }
    if (input == Arcade::Input::RESTART) {
        if (_game) {
            _game->stop();
            _game.reset();
            loadGame("./lib/" + _gameLibs[_selectedGame]);
        }
     }
}

bool Arcade::Core::run()
{
    _isRunning = true;

    if (!_display) {
        return false;
    }

    while (_isRunning) {
        Arcade::Input input = _display->getInput();
    
        handleInput(input);
    
        _display->clear();

        if (_isMenu) {
            displayMenu();
            handleMenuInput(input);
        } else if (_isGameOver) {
            displayGameOver();
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - _gameOverStart).count() >= 3) {
                _isMenu = true;
                _isGameOver = false;
                _game.reset();
            }
        } else if (_isWin) {
            displayWin();
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - _gameOverStart).count() >= 3) {
                _isMenu = true;
                _isWin = false;
                _game.reset();
            }
        } else {
            if (_game) {
                _game->handleInput(input);
                _game->update();
                if (!_game->isRunning()) {
                    if (_game->is_GameOver()) {
                        _isGameOver = true;
                        _gameOverStart = std::chrono::steady_clock::now();
                    } else if (_game->isGameWon()) {
                        _isWin = true;
                        _gameOverStart = std::chrono::steady_clock::now();
                    } else {
                        _isMenu = true;
                        _game.reset();
                    }
                } else {
                    const auto &entities = _game->getEntities();
                    for (const auto &obj : entities) {
                        _display->drawObject(obj);
                    }
                }
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

    try {
        Arcade::Core core(av[1]);

        int result = core.run();
        if (!result)
            return 84;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 84;
    }

    return 0;
}