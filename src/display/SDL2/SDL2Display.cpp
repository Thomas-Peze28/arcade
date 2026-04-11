/*
** EPITECH PROJECT, 2026
** Arcade
** File description:
** SDL2Display
*/

#include "SDL2Display.hpp"

namespace Arcade {

    SDL2Display::SDL2Display() : _name("sdl2"), _window(nullptr), _renderer(nullptr), _font(nullptr), _tileSize(20)
    {
    }

    SDL2Display::~SDL2Display()
    {
        stop();
    }

    void SDL2Display::init()
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            return;
        }

        if (TTF_Init() == -1) {
            std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
            return;
        }

        _window = SDL_CreateWindow("Arcade - SDL2 Display", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1200, 800, SDL_WINDOW_SHOWN);
        if (!_window) {
            std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return;
        }

        _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
        if (!_renderer) {
            std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return;
        }

        _font = TTF_OpenFont("assets/Roboto-Black.ttf", 24);
        if (!_font) {
            std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
            return;
        }
        SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
    }

    void SDL2Display::stop()
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

        if (TTF_WasInit()) {
            TTF_Quit();
        }

        for (auto const& [key, texture] : _textures) {
            if (texture) {
                SDL_DestroyTexture(texture);
            }
        }
        _textures.clear();
        
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }

    const std::string &SDL2Display::getName() const
    {
        return _name;
    }

    void SDL2Display::clear()
    {
        SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
        SDL_RenderClear(_renderer);
    }

    void SDL2Display::display()
    {
        SDL_RenderPresent(_renderer);
    }

    void SDL2Display::translateColor(const Arcade::Color &color)
    {
        switch (color) {
            case Color::RED:   
                SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 255); break;
            case Color::GREEN: 
                SDL_SetRenderDrawColor(_renderer, 0, 255, 0, 255); break;
            case Color::BLUE:  
                SDL_SetRenderDrawColor(_renderer, 0, 0, 255, 255); break;
            case Color::WHITE: 
                SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255); break;
            case Color::BLACK: 
                SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255); break;
        }
    }

    void Arcade::SDL2Display::drawObject(const Arcade::Object &object)
    {
        if (!_renderer)
            return;

        bool spriteDraw = false;

        if (!object.sprite_path.empty()) {

            if (_textures.find(object.sprite_path) == _textures.end()) {
                SDL_Surface* surface = IMG_Load(object.sprite_path.c_str());

                if (surface) {
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
                    _textures[object.sprite_path] = texture;
                    SDL_FreeSurface(surface);
                } else {
                    std::cerr << "[SDL2] Error: Failed to load texture " << object.sprite_path << std::endl;
                    _textures[object.sprite_path] = nullptr;
                }
            }

            SDL_Texture* tex = _textures[object.sprite_path];
            if (tex) {
                SDL_Rect destRect;
                destRect.x = object.pos.x * _tileSize;
                destRect.y = object.pos.y * _tileSize;
                destRect.w = _tileSize;
                destRect.h = _tileSize;

                SDL_RenderCopy(_renderer, tex, NULL, &destRect);

                spriteDraw = true;
            }
        }

        if (!spriteDraw) {
            if (!_font)
                return;
            SDL_Color textColor = {255, 255, 255, 255};
            switch (object.color) {
                case Color::RED:   textColor = {255, 0,   0,   255}; break;
                case Color::GREEN: textColor = {0,   255, 0,   255}; break;
                case Color::BLUE:  textColor = {0,   0,   255, 255}; break;
                case Color::BLACK: textColor = {50,  50,  50,  255}; break;
                case Color::WHITE: textColor = {255, 255, 255, 255}; break;
            }
            char buf[2] = {object.character, '\0'};
            SDL_Surface *surface = TTF_RenderText_Solid(_font, buf, textColor);
            if (!surface)
                return;
            SDL_Texture *texture = SDL_CreateTextureFromSurface(_renderer, surface);
            if (texture) {
                SDL_Rect destRect = {object.pos.x * _tileSize, object.pos.y * _tileSize,
                                     _tileSize, _tileSize};
                SDL_RenderCopy(_renderer, texture, nullptr, &destRect);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
    }

    void SDL2Display::drawText(const std::string &text, const Arcade::Position &pos, const Arcade::Color &color)
    {
        if (!_font)
            return;

        SDL_Color textColor = {255, 255, 255, 255};
        switch (color) {
            case Color::RED:   
                textColor = {255, 0, 0, 255}; break;
            case Color::GREEN: 
                textColor = {0, 255, 0, 255}; break;
            case Color::BLUE:  
                textColor = {0, 0, 255, 255}; break;
            case Color::BLACK: 
                textColor = {0, 0, 0, 255}; break;
            case Color::WHITE: 
                textColor = {255, 255, 255, 255}; break;
        }

        SDL_Surface* surface = TTF_RenderText_Solid(_font, text.c_str(), textColor);
        if (!surface) return;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
        if (!texture) {
            SDL_FreeSurface(surface);
            return;
        }

        SDL_Rect destRect;
        destRect.x = pos.x * _tileSize;
        destRect.y = pos.y * _tileSize;
        destRect.w = surface->w;
        destRect.h = surface->h;

        SDL_RenderCopy(_renderer, texture, nullptr, &destRect);

        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);   
    }

    Input SDL2Display::getInput()
    {
        SDL_Event event;
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) {
            return Input::EXIT;
        }
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:     
                    return Input::UP;
                case SDLK_DOWN:   
                    return Input::DOWN;
                case SDLK_LEFT:   
                    return Input::LEFT;
                case SDLK_RIGHT:  
                    return Input::RIGHT;
                case SDLK_z:      
                    return Input::Z;
                case SDLK_q:      
                    return Input::Q;
                case SDLK_s:      
                    return Input::S;
                case SDLK_d:      
                    return Input::D;
                case SDLK_SPACE:  
                    return Input::SPACE;
                case SDLK_RETURN: 
                    return Input::ACTION;
                case SDLK_r:      
                    return Input::RESTART;
                case SDLK_m:      
                    return Input::MENU;
                case SDLK_ESCAPE: 
                    return Input::EXIT;
                case SDLK_F1:
                    return Input::NEXT_GRAPHIC;
                case SDLK_F2:
                    return Input::PREV_GRAPHIC;
                case SDLK_F3:
                    return Input::NEXT_GAME;
                case SDLK_F4:
                    return Input::PREV_GAME;
                default:          
                    return Input::NONE;
            }
        }
        return Input::NONE;
    }
}

extern "C" {
    Arcade::IDisplay *entryPoint()
    {
        return new Arcade::SDL2Display();
    }

    const char *getType() {
        return "display";
    }
}