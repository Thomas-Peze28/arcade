/*
** EPITECH PROJECT, 2026
** Arcade
** File description:
** Arcade
*/

#include "Arcade.hpp"

Arcade::Core::Core(const std::string &initialLibrary) : currentLibrary(initialLibrary)
{
    _displayLoader = std::make_unique<DLLoader<IDisplayModule>>(initialLibrary);

    if (!_displayLoader->isOpen())
        std::cerr << "Failed to load library: " << initialLibrary << std::endl;

    _display.reset(_displayLoader->getInstance("entryPoint"));

    if (!_display)
        std::cerr << "Failed to get display instance from library: " << initialLibrary << std::endl;
    _display->init();
}

bool Arcade::Core::run()
{
    _isRunning = true;

    while (_isRunning) {
        std::cout << "Running Arcade Core with display: " << _display->getName() << std::endl;
        _display->getInput();
        _display->clear();
        _display->display();
        _isRunning = false;
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