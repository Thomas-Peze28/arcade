/*
** EPITECH PROJECT, 2026
** include
** File description:
** IDisplay
*/

#ifndef IDisplay_H
#define IDisplay_H

#include <string>

#include "Variables.hpp"

namespace Arcade {
    class IDisplay {
        public:
            virtual ~IDisplay() = default;
            virtual void init() = 0;
            virtual void stop() = 0;
            virtual const std::string &getName() const = 0;

            virtual Input getInput() = 0;
            virtual void clear() = 0;
            virtual void display() = 0;
            virtual void drawObject(const Arcade::Object &object) = 0;
            virtual void drawText(const std::string &text, const Arcade::Position &pos, const Arcade::Color &color) = 0;
    };
}
#endif /* IDisplay_H */
