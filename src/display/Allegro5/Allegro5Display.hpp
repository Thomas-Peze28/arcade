/*
** EPITECH PROJECT, 2026
** Arcade
** File description:
** Allegro5Display
*/

#ifndef ALLEGRO5DISPLAY_HPP_
#define ALLEGRO5DISPLAY_HPP_

#include "IDisplay.hpp"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <map>
#include <string>

namespace Arcade {
    class Allegro5Display : public IDisplay {
        public:
            Allegro5Display();
            ~Allegro5Display() override;

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
            ALLEGRO_DISPLAY *_display;
            ALLEGRO_EVENT_QUEUE *_event_queue;
            ALLEGRO_FONT *_font;
            int _scale;
            std::map<std::string, ALLEGRO_BITMAP *> _bitmaps;
            ALLEGRO_COLOR translateColor(const Arcade::Color &color);
    };
}

#endif /* ALLEGRO5DISPLAY_HPP_ */
