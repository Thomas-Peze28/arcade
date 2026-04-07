/*
** EPITECH PROJECT, 2026
** Arcade
** File description:
** Arcade
*/

#include "Arcade.hpp"

Arcade::Core::Core(const std::string &initialLibrary) : currentLibrary(initialLibrary), _isRunning(false), _isMenu(true), _playerName("Player1")
{
    _displayLoader = std::make_unique<DLLoader<IDisplayModule>>(initialLibrary);

    if (!_displayLoader->isOpen())
        std::cerr << "Failed to load library: " << initialLibrary << std::endl;

    _display.reset(_displayLoader->getInstance("entryPoint"));

    if (!_display)
        std::cerr << "Failed to get display instance from library: " << initialLibrary << std::endl;
    _display->init();
    
    loadLibs();
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

void Arcade::Core::displayMenu()
{
    _display->drawText("=== ARCADE MAIN MENU ===", {10, 2}, Arcade::Color::RED);
    _display->drawText("[ GRAPHICAL LIBRARIES ]", {5, 5}, Arcade::Color::BLUE);
    int yOffset = 7;
    for (size_t i = 0; i < _graphicLibs.size(); ++i) {
        _display->drawText("- " + _graphicLibs[i], {5, yOffset}, Arcade::Color::WHITE);
        yOffset++;
    }
    _display->drawText("[ GAME LIBRARIES ]", {35, 5}, Arcade::Color::BLUE);
    yOffset = 7;
    for (size_t i = 0; i < _gameLibs.size(); ++i) {
        _display->drawText("- " + _gameLibs[i], {35, yOffset}, Arcade::Color::WHITE);
        yOffset++;
    }
    _display->drawText("[Player Name]: " + _playerName, {10, yOffset + 3}, Arcade::Color::GREEN);
}

bool Arcade::Core::run()
{
    _isRunning = true;

    while (_isRunning) {
        Arcade::Input input = _display->getInput();

        if (input == Arcade::Input::EXIT) {
            _isRunning = false;
        }

        _display->clear();

        if (_isMenu) {
            displayMenu();
        } else {
            std::cout << "Game is running... (input: " << static_cast<int>(input) << ")" << std::endl;
        }
        _display->display();

        //_isRunning = false;
    }
    return true;
}

Arcade::Core::~Core()
{
}

int main(int ac, char **av)
{
    if (ac != 2) {
        std::cout << "USAGE" << std::endl;
        std::cout << "\t./arcade ./lib/arcade_[ncurses|sfml|sdl2].so" << std::endl;
        return 84;
    }

    Arcade::Core core(av[1]);
    core.run();
    return 0;
}