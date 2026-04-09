/*
** EPITECH PROJECT, 2026
** Arcade
** File description:
** NcursesDisplay
*/

#ifndef NCURSESDISPLAY_HPP_
#define NCURSESDISPLAY_HPP_

#include "IDisplay.hpp"
#include <ncurses.h>

namespace Arcade {
    class NcursesDisplay : public IDisplay {
        public:
            NcursesDisplay();
            ~NcursesDisplay() override;

            void init() override;
            void stop() override;
            const std::string &getName() const override;

            Input getInput() override;
            void clear() override;
            void display() override;
            void drawObject(const Arcade::Object &object) override;
            void drawText(const std::string &text, const Arcade::Position &pos, const Arcade::Color &color) override;

        private:
            std::string _name;
            int translateColor(const Arcade::Color &color);
    };
}

#endif /* NCURSESDISPLAY_HPP_ */