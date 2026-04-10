/*
** EPITECH PROJECT, 2026
** Arcade
** File description:
** IGame
*/

#ifndef IGAME_H
#define IGAME_H

#include <string>
#include <vector>
#include "Variables.hpp"

namespace Arcade {
    class IGame {
        public:
            virtual ~IGame() = default;
            virtual void init() = 0;
            virtual void stop() = 0;
            virtual const std::string &getName() const = 0;
            virtual bool isRunning() const = 0;
            virtual void handleInput(Arcade::Input input) = 0;
            virtual bool is_GameOver() const = 0;
            virtual bool isGameWon() const = 0;
            virtual void update() = 0;

            virtual const std::vector<Arcade::Object> &getEntities() const = 0;
    };
}

#endif /* IGAME_H */