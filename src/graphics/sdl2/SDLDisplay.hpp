#pragma once

#include "../../abstracts/ADisplayModule.hpp"
#include "../../shared/LibType.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

namespace arcade {

    class SDLDisplay final : public ADisplayModule {
    public:
        SDLDisplay()  = default;
        ~SDLDisplay() override = default;

        SDLDisplay(const SDLDisplay &) = delete;
        SDLDisplay &operator=(const SDLDisplay &) = delete;

        std::string getName() const override;

        void open()    override;
        void close()   override;
        void clear()   override;
        void display() override;

        void  setGridSize(Vec2i size) override;

        bool pollEvent(Event &event) override;

        void drawTile(Vec2i pos, const Tile &tile)                     override;
        void drawSprite(Vec2i pos, const std::string &spritePath)      override;
        void drawText(const std::string &text, Vec2i pos, Color color) override;

    private:
        SDL_Color toSDLColor(Color color) const;
        KeyCode   mapSDLKey(SDL_Keycode sym) const;

        SDL_Window   *_window   = nullptr;
        SDL_Renderer *_renderer = nullptr;
        TTF_Font     *_font     = nullptr;
        int           _tileW    = 24;
        int           _tileH    = 24;
    };

} // namespace arcade

extern "C" arcade::IDisplayModule *createDisplay();
extern "C" arcade::LibType         getLibType();
