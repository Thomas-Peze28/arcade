/*
** EPITECH PROJECT, 2026
** Arcade
** File description:
** IGame
*/

#ifndef IGAME_H
#define IGAME_H

#include <string>

namespace Arcade {
    class IGame {
        public:
            virtual ~IGame() = default;
            virtual void init() = 0;
            virtual void stop() = 0;
            virtual const std::string &getName() const = 0;
    };
}

#endif /* IGAME_H */