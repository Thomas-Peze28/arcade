/*
** EPITECH PROJECT, 2026
** Arcade
** File description:
** Allegro5Display
*/

#include "Allegro5Display.hpp"
#include <stdexcept>
#include <iostream>
#include <stdexcept>

namespace Arcade {

    Allegro5Display::Allegro5Display() : _name("allegro5"), _display(nullptr), _event_queue(nullptr), _font(nullptr), _scale(20)
    {
    }

    Allegro5Display::~Allegro5Display()
    {
    }

    void Allegro5Display::init()
    {
        al_init();
        al_init_primitives_addon();
        al_init_font_addon();
        al_init_ttf_addon();
        al_install_keyboard();

        _display = al_create_display(1200, 800);
        if (!_display) {
            throw std::runtime_error("Failed to create Allegro display");
        }
        
        _event_queue = al_create_event_queue();
        if (!_event_queue) {
            throw std::runtime_error("Failed to create Allegro event queue");
        }
        
        al_register_event_source(_event_queue, al_get_display_event_source(_display));
        al_register_event_source(_event_queue, al_get_keyboard_event_source());
        
        _font = al_load_ttf_font("assets/Roboto-Black.ttf", 18, 0);
        if (!_font) {
            throw std::runtime_error("Failed to load font: assets/Roboto-Black.ttf");
        }
    }

    void Allegro5Display::stop()
    {
        if (_font)
            al_destroy_font(_font);
        for (auto &[path, bmp] : _bitmaps)
            if (bmp)
                al_destroy_bitmap(bmp);
        _bitmaps.clear();
        if (_event_queue)
            al_destroy_event_queue(_event_queue);
        if (_display)
            al_destroy_display(_display);
    }

    const std::string &Allegro5Display::getName() const
    {
        return _name;
    }

    void Allegro5Display::clear()
    {
        al_clear_to_color(al_map_rgb(0, 0, 0));
    }

    void Allegro5Display::display()
    {
        al_flip_display();
    }

    ALLEGRO_COLOR Allegro5Display::translateColor(const Arcade::Color &color)
    {
        switch (color) {
            case Color::RED:
                return al_map_rgb(255, 0, 0);
            case Color::GREEN:
                return al_map_rgb(0, 255, 0);
            case Color::BLUE:
                return al_map_rgb(0, 0, 255);
            case Color::WHITE:
                return al_map_rgb(255, 255, 255);
            case Color::BLACK:
                return al_map_rgb(0, 0, 0);
            default:
                return al_map_rgb(255, 255, 255);
        }
    }

    void Allegro5Display::drawObject(const Arcade::Object &object)
    {
        if (!object.sprite_path.empty()) {
            if (_bitmaps.find(object.sprite_path) == _bitmaps.end())
                _bitmaps[object.sprite_path] = al_load_bitmap(object.sprite_path.c_str());
            ALLEGRO_BITMAP *bmp = _bitmaps[object.sprite_path];
            if (bmp) {
                al_draw_scaled_bitmap(bmp, 0, 0,
                    al_get_bitmap_width(bmp), al_get_bitmap_height(bmp),
                    object.pos.x * _scale, object.pos.y * _scale,
                    _scale, _scale, 0);
                return;
            }
        }
        if (_font) {
            char buf[2] = {object.character, '\0'};
            al_draw_text(_font, translateColor(object.color),
                         object.pos.x * _scale, object.pos.y * _scale,
                         ALLEGRO_ALIGN_LEFT, buf);
        }
    }

    void Allegro5Display::drawText(const std::string &text, const Arcade::Position &pos, const Arcade::Color &color)
    {
        if (_font) {
            al_draw_text(_font, translateColor(color), pos.x * (_scale / 1.5f), pos.y * _scale, ALLEGRO_ALIGN_LEFT, text.c_str());
        }
    }

    Input Allegro5Display::getInput()
    {
        ALLEGRO_EVENT event;

        if (al_get_next_event(_event_queue, &event)) {
            if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                return Input::EXIT;

            if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
                switch (event.keyboard.keycode) {
                    case ALLEGRO_KEY_UP:
                        return Input::UP;
                    case ALLEGRO_KEY_DOWN:
                        return Input::DOWN;
                    case ALLEGRO_KEY_LEFT:
                        return Input::LEFT;
                    case ALLEGRO_KEY_RIGHT:
                        return Input::RIGHT;
                    case ALLEGRO_KEY_Z:
                        return Input::Z;
                    case ALLEGRO_KEY_Q:
                        return Input::Q;
                    case ALLEGRO_KEY_S:
                        return Input::S;
                    case ALLEGRO_KEY_D:
                        return Input::D;
                    case ALLEGRO_KEY_SPACE:
                        return Input::SPACE;
                    case ALLEGRO_KEY_ENTER:
                        return Input::ACTION;
                    case ALLEGRO_KEY_R:
                        return Input::RESTART;
                    case ALLEGRO_KEY_M:
                        return Input::MENU;
                    case ALLEGRO_KEY_ESCAPE:
                        return Input::EXIT;
                    case ALLEGRO_KEY_F1:
                        return Input::NEXT_GRAPHIC;
                    case ALLEGRO_KEY_F2:
                        return Input::PREV_GRAPHIC;
                    case ALLEGRO_KEY_F3:
                        return Input::NEXT_GAME;
                    case ALLEGRO_KEY_F4:
                        return Input::PREV_GAME;
                    default:
                        return Input::NONE;
                }
            }
        }
        return Input::NONE;
    }

}

extern "C" {
    Arcade::IDisplay *entryPoint()
    {
        return new Arcade::Allegro5Display();
    }

    const char *getType() {
        return "display";
    }
}
