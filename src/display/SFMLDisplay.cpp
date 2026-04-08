/*
** EPITECH PROJECT, 2026
** Arcade
** File description:
** SFMLDisplay
*/

#include "SFMLDisplay.hpp"

namespace Arcade {

    SFMLDisplay::SFMLDisplay() : _name("sfml"), _scale(20)
    {
    }

    SFMLDisplay::~SFMLDisplay()
    {
        stop();
    }

    void SFMLDisplay::init()
    {
        _window = std::make_unique<sf::RenderWindow>(sf::VideoMode(1200, 800), "Arcade - SFML");
        _window->setFramerateLimit(60);

        if (_font.loadFromFile("assets/Roboto-Black.ttf")) {
            _fontLoaded = true;
        } else {
            std::cout << "[SFML] Warning: Unable to load font" << std::endl;
        }
    }

    void SFMLDisplay::stop()
    {
       if (_window && _window->isOpen()) {
            _window->close();
        }
    }

    const std::string &SFMLDisplay::getName() const
    {
        return _name;
    }

    void SFMLDisplay::clear()
    {
        if (_window && _window->isOpen()) {
            _window->clear(sf::Color::Black);
        }
    }

    void SFMLDisplay::display()
    {
        if (_window && _window->isOpen()) {
            _window->display();
        }
    }

    sf::Color SFMLDisplay::translateColor(const Arcade::Color &color) {
        switch (color) {
            case Color::RED:
                return sf::Color::Red;
            case Color::GREEN:
                return sf::Color::Green;
            case Color::BLUE:
                return sf::Color::Blue;
            case Color::WHITE:
                return sf::Color::White;
            case Color::BLACK:
                return sf::Color::Black;
            default:
                return sf::Color::White;
        }
    }

    void SFMLDisplay::drawObject(const Arcade::Object &object)
    {
        if (!_window || !_window->isOpen())
            return;

        sf::RectangleShape rect(sf::Vector2f(_scale, _scale));
    
        rect.setPosition(object.pos.x * _scale, object.pos.y * _scale);
        rect.setFillColor(translateColor(object.color));
        _window->draw(rect);
    }

    void SFMLDisplay::drawText(const std::string &text, const Arcade::Position &pos, const Arcade::Color &color)
    {
        if (!_window || !_window->isOpen() || !_fontLoaded)
            return;

        sf::Text sfText;
    
        sfText.setFont(_font);
        sfText.setString(text);
        sfText.setCharacterSize(24);
        sfText.setFillColor(translateColor(color));
        sfText.setPosition(pos.x * (_scale / 1.5), pos.y * _scale); 

        _window->draw(sfText);
    }

    Input SFMLDisplay::getInput()
    {
        if (!_window || !_window->isOpen())
            return Input::NONE;

        sf::Event event;
    
        _window->pollEvent(event);

        if (event.type == sf::Event::Closed) {
            return Input::EXIT;
        }
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::Up:
                    return Input::UP;
                case sf::Keyboard::Down:
                    return Input::DOWN;
                case sf::Keyboard::Left:
                    return Input::LEFT;
                case sf::Keyboard::Right:
                    return Input::RIGHT;
                case sf::Keyboard::Enter:
                    return Input::ACTION;
                case sf::Keyboard::Escape:
                    return Input::EXIT;
                case sf::Keyboard::M:
                    return Input::MENU;
                default:
                    break;
            }
        }
        return Input::NONE;
    }
}

extern "C" {
    Arcade::IDisplay *entryPoint()
    {
        return new Arcade::SFMLDisplay();
    }
}