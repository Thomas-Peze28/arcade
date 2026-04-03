/*
** EPITECH PROJECT, 2026
** Arcade
** File description:
** Arcade
*/

#ifndef ARCADE_HPP
#define ARCADE_HPP

#include <string>
#include <memory>
#include "DLLoader.hpp"
#include "IGame.hpp"
#include "IDisplay.hpp"

namespace Arcade {
    class Core {
        public:
            Core(const std::string &initialLibrary);
            ~Core();

            bool run();
            
        private:
            std::string currentLibrary;
            bool _isRunning;

            std::unique_ptr<DLLoader<IDisplayModule>> _displayLoader;
            std::unique_ptr<DLLoader<IGame>> _gameLoader;

            std::unique_ptr<IDisplayModule> _display;
            std::unique_ptr<IGame> _game;
    };
}

#endif /* ARCADE_HPP_ */