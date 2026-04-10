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
#include <filesystem>
#include <map>
#include <chrono>
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

            void displayMenu();
            void displayGameOver();
            void displayWin();
            void loadLibs();
            void switchGraphicalLibrary(const std::string &libPath);

            std::string currentLibrary;
            bool _isRunning;
            bool _isMenu;
            bool _isGameOver;
            bool _isWin;
            std::chrono::steady_clock::time_point _gameOverStart;

            std::map<std::string, std::unique_ptr<DLLoader<IDisplay>>> _displayLoaders;
            std::unique_ptr<DLLoader<IGame>> _gameLoader;

            std::unique_ptr<IDisplay> _display;
            std::unique_ptr<IGame> _game;

            std::vector<std::string> _graphicLibs;
            std::vector<std::string> _gameLibs;
            std::string _playerName;

            void handleMenuInput(Arcade::Input input);
            void handleInput(Arcade::Input input);
            size_t _selectedGraphic;
            size_t _selectedGame;

            void loadGame(const std::string &libPath);
    };
}

#endif /* ARCADE_HPP_ */