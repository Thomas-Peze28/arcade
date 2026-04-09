/*
** EPITECH PROJECT, 2026
** Arcade
** File description:
** SDL2Display
*/

#ifndef SDL2DISPLAY_HPP_
#define SDL2DISPLAY_HPP_

#include "IDisplay.hpp"

#include <iostream>
#include <memory>
#include <map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

namespace Arcade {
    class SDL2Display : public IDisplay {
        public:
            SDL2Display();
            ~SDL2Display() override;

            void init() override;
            void stop() override;
            const std::string &getName() const override;

            Input getInput() override;
            void clear() override;
            void display() override;
            void drawObject(const Arcade::Object &object) override;
            void drawText(const std::string &text, const Arcade::Position &pos, const Arcade::Color &color) override;

        private:
            void translateColor(const Arcade::Color &color);
            
            std::string _name;
            SDL_Window *_window;
            SDL_Renderer *_renderer;
            TTF_Font *_font;
            int _tileSize;
            std::map<std::string, SDL_Texture*> _textures;
    };
}

#endif /* SDL2DISPLAY_HPP_ */