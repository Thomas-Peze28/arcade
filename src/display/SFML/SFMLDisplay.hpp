/*
** EPITECH PROJECT, 2026
** Arcade
** File description:
** SFMLDisplay
*/

#ifndef SFMLDISPLAY_HPP_
#define SFMLDISPLAY_HPP_

#include "IDisplay.hpp"

#include <iostream>
#include <memory>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

namespace Arcade {
    class SFMLDisplay : public IDisplay {
        public:
            SFMLDisplay();
            ~SFMLDisplay() override;

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
            std::unique_ptr<sf::RenderWindow> _window;
            sf::Font _font;
            bool _fontLoaded;
            int _scale;
            sf::Color translateColor(const Arcade::Color &color);

            std::map<std::string, sf::Texture> _textures;
            std::map<std::string, sf::Sprite> _sprites;

    };
}

#endif /* SFMLDISPLAY_HPP_ */