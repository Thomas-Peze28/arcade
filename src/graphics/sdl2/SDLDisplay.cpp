#include "SDLDisplay.hpp"
#include <array>
#include <stdexcept>

namespace arcade {

    static constexpr int FONT_SIZE = 16;

    static const std::array<const char *, 4> FONT_PATHS = {
        "/usr/share/fonts/TTF/DejaVuSansMono.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
        "/usr/share/fonts/TTF/JetBrainsMonoNerdFontMono-Regular.ttf",
        "/usr/share/fonts/liberation/LiberationMono-Regular.ttf",
    };

    std::string SDLDisplay::getName() const
    {
        return "SDL2";
    }

    void SDLDisplay::open()
    {
        if (SDL_Init(SDL_INIT_VIDEO) != 0)
            throw std::runtime_error(std::string("SDL_Init: ") + SDL_GetError());
        if (TTF_Init() != 0)
            throw std::runtime_error(std::string("TTF_Init: ") + TTF_GetError());

        _window = SDL_CreateWindow("Arcade",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            _gridSize.x * _tileW, _gridSize.y * _tileH,
            SDL_WINDOW_SHOWN);
        if (!_window)
            throw std::runtime_error(std::string("SDL_CreateWindow: ") + SDL_GetError());

        _renderer = SDL_CreateRenderer(_window, -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!_renderer)
            throw std::runtime_error(std::string("SDL_CreateRenderer: ") + SDL_GetError());

        for (const char *path : FONT_PATHS) {
            _font = TTF_OpenFont(path, FONT_SIZE);
            if (_font)
                break;
        }

        _isOpen = true;
    }

    void SDLDisplay::close()
    {
        if (_font) {
            TTF_CloseFont(_font);
            _font = nullptr;
        }
        if (_renderer) {
            SDL_DestroyRenderer(_renderer);
            _renderer = nullptr;
        }
        if (_window) {
            SDL_DestroyWindow(_window);
            _window = nullptr;
        }
        TTF_Quit();
        SDL_Quit();
        _isOpen = false;
    }

    void SDLDisplay::clear()
    {
        SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
        SDL_RenderClear(_renderer);
    }

    void SDLDisplay::display()
    {
        SDL_RenderPresent(_renderer);
    }

    void SDLDisplay::setGridSize(Vec2i size)
    {
        _gridSize = size;
        if (_window)
            SDL_SetWindowSize(_window, _gridSize.x * _tileW, _gridSize.y * _tileH);
    }

    SDL_Color SDLDisplay::toSDLColor(Color color) const
    {
        return {color.r, color.g, color.b, color.a};
    }

    KeyCode SDLDisplay::mapSDLKey(SDL_Keycode sym) const
    {
        switch (sym) {
            case SDLK_UP:       return KeyCode::UP;
            case SDLK_DOWN:     return KeyCode::DOWN;
            case SDLK_LEFT:     return KeyCode::LEFT;
            case SDLK_RIGHT:    return KeyCode::RIGHT;
            case SDLK_ESCAPE:   return KeyCode::ESCAPE;
            case SDLK_RETURN:
            case SDLK_KP_ENTER: return KeyCode::ENTER;
            case SDLK_SPACE:    return KeyCode::SPACE;
            case SDLK_F1:       return KeyCode::F1;
            case SDLK_F2:       return KeyCode::F2;
            case SDLK_F3:       return KeyCode::F3;
            case SDLK_F4:       return KeyCode::F4;
            case SDLK_r:        return KeyCode::R;
            case SDLK_m:        return KeyCode::M;
            default:            return KeyCode::UNKNOWN;
        }
    }

    bool SDLDisplay::pollEvent(Event &event)
    {
        SDL_Event sdlEvent;
        while (SDL_PollEvent(&sdlEvent)) {
            if (sdlEvent.type == SDL_QUIT) {
                event.type = EventType::KEYBOARD;
                event.key  = KeyCode::ESCAPE;
                return true;
            }
            if (sdlEvent.type == SDL_KEYDOWN) {
                event.type = EventType::KEYBOARD;
                event.key  = mapSDLKey(sdlEvent.key.keysym.sym);
                return true;
            }
            if (sdlEvent.type == SDL_MOUSEBUTTONDOWN) {
                event.type     = EventType::MOUSE;
                event.key      = KeyCode::UNKNOWN;
                event.mousePos = {sdlEvent.button.x / _tileW, sdlEvent.button.y / _tileH};
                if (sdlEvent.button.button == SDL_BUTTON_LEFT)
                    event.mouseButton = MouseButton::LEFT;
                else if (sdlEvent.button.button == SDL_BUTTON_RIGHT)
                    event.mouseButton = MouseButton::RIGHT;
                else
                    event.mouseButton = MouseButton::MIDDLE;
                return true;
            }
        }
        return false;
    }

    void SDLDisplay::drawTile(Vec2i pos, const Tile &tile)
    {
        SDL_Rect rect = {pos.x * _tileW, pos.y * _tileH, _tileW, _tileH};

        SDL_Color bg = toSDLColor(tile.bg);
        SDL_SetRenderDrawColor(_renderer, bg.r, bg.g, bg.b, bg.a);
        SDL_RenderFillRect(_renderer, &rect);

        if (!_font || tile.symbol == ' ')
            return;

        char buf[2] = {tile.symbol, '\0'};
        SDL_Color fg = toSDLColor(tile.fg);
        SDL_Surface *surface = TTF_RenderText_Blended(_font, buf, fg);
        if (!surface)
            return;
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_renderer, surface);
        SDL_FreeSurface(surface);
        if (!texture)
            return;

        int tw, th;
        SDL_QueryTexture(texture, nullptr, nullptr, &tw, &th);
        SDL_Rect dst = {
            pos.x * _tileW + (_tileW - tw) / 2,
            pos.y * _tileH + (_tileH - th) / 2,
            tw, th
        };
        SDL_RenderCopy(_renderer, texture, nullptr, &dst);
        SDL_DestroyTexture(texture);
    }

    void SDLDisplay::drawSprite(Vec2i, const std::string &)
    {
    }

    void SDLDisplay::drawText(const std::string &text, Vec2i pos, Color color)
    {
        if (!_font || text.empty())
            return;

        SDL_Color sdlColor = toSDLColor(color);
        SDL_Surface *surface = TTF_RenderText_Blended(_font, text.c_str(), sdlColor);
        if (!surface)
            return;
        SDL_Texture *texture = SDL_CreateTextureFromSurface(_renderer, surface);
        SDL_FreeSurface(surface);
        if (!texture)
            return;

        int tw, th;
        SDL_QueryTexture(texture, nullptr, nullptr, &tw, &th);
        SDL_Rect dst = {pos.x * _tileW, pos.y * _tileH, tw, th};
        SDL_RenderCopy(_renderer, texture, nullptr, &dst);
        SDL_DestroyTexture(texture);
    }

} // namespace arcade

arcade::IDisplayModule *createDisplay()
{
    return new arcade::SDLDisplay();
}

arcade::LibType getLibType()
{
    return arcade::LibType::GRAPHICAL;
}
