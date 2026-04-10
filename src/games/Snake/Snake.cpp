/*
** EPITECH PROJECT, 2026
** Arcade
** File description:
** Snake
*/

#include "Snake.hpp"
#include <cstdlib>
#include <ctime>

namespace Arcade {

    Snake::Snake() : _name("Snake"),  _map_size(30), _speedMs(100), _isRunning(false), _isGameOver(false), _isWin(false), _offsetX(15), _offsetY(5)
    {
        std::srand(std::time(nullptr));
    }

    void Snake::init()
    {
        _body.clear();
        _body.push_back({10 + _offsetX, 10 + _offsetY});
        _body.push_back({10 + _offsetX, 11 + _offsetY});
        _body.push_back({10 + _offsetX, 12 + _offsetY});

        _direction = Arcade::Input::UP;
        _nextDirection = Arcade::Input::UP;
        _lastUpdate = std::chrono::steady_clock::now();

        _isRunning = true;
        _isGameOver = false;
        _isWin = false;

        spawnApple();
        buildMap();
        buildEntities();
    }

    void Snake::stop()
    {
        _isRunning = false;
    }

    const std::string &Snake::getName() const
    {
        return _name;
    }

    void Snake::buildMap() {
        for (int x = 0; x <= _map_size; x++) {
            _entities.push_back({{x + _offsetX, 0 + _offsetY}, '#', Arcade::Color::BLUE, "./src/games/Snake/assets/brick.png"});
            _entities.push_back({{x + _offsetX, _map_size + _offsetY}, '#', Arcade::Color::BLUE, "./src/games/Snake/assets/brick.png"});
        }
        for (int y = 0; y < _map_size; y++) {
            _entities.push_back({{0 + _offsetX, y + _offsetY}, '#', Arcade::Color::BLUE, "./src/games/Snake/assets/brick.png"});
            _entities.push_back({{_map_size + _offsetX, y + _offsetY}, '#', Arcade::Color::BLUE, "./src/games/Snake/assets/brick.png"});
        }
    }

    void Snake::spawnApple()
    {
        int newX = (std::rand() % (_map_size - 2)) + 1;
        int newY = (std::rand() % (_map_size - 2)) + 1;
        
        _apple = {newX + _offsetX, newY + _offsetY};
    }

    void Snake::handleInput(Arcade::Input input)
    {
        if (input == Arcade::Input::UP && _direction != Arcade::Input::DOWN)
            _nextDirection = input;
        if (input == Arcade::Input::DOWN && _direction != Arcade::Input::UP)
            _nextDirection = input;
        if (input == Arcade::Input::LEFT && _direction != Arcade::Input::RIGHT)
            _nextDirection = input;
        if (input == Arcade::Input::RIGHT && _direction != Arcade::Input::LEFT)
            _nextDirection = input;
    }

    void Snake::isCollision()
    {
        Arcade::Position head = _body.front();
        for (size_t i = 1; i < _body.size(); i++) {
            if (head.x == _body[i].x && head.y == _body[i].y) {
                _isGameOver = true;
                _isRunning = false;
                return;
            }
        }
        
        if (head.x <= _offsetX || head.x >= _map_size + _offsetX || head.y <= _offsetY || head.y >= _map_size + _offsetY) {
            _isGameOver = true;
            _isRunning = false;
            return;
        }
    }

    void Snake::isApple()
    {
        Arcade::Position head = _body.front();
        if (head.x == _apple.x && head.y == _apple.y) {
            spawnApple();
        } else {
            _body.pop_back();
        }
    }

    void Snake::moveSnake()
    {
        _direction = _nextDirection;
        Arcade::Position nextPos = _body.front();

        if (_direction == Arcade::Input::UP)
            nextPos.y--;
        if (_direction == Arcade::Input::DOWN)
            nextPos.y++;
        if (_direction == Arcade::Input::LEFT)
            nextPos.x--;
        if (_direction == Arcade::Input::RIGHT)
            nextPos.x++;
        _body.insert(_body.begin(), nextPos);
    }

    void Snake::update()
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - _lastUpdate).count();
        if (elapsed < _speedMs) {
            return;
        }
        _lastUpdate = now;

        moveSnake();
        isApple();
        isCollision();
        buildEntities();
    }

    void Snake::buildEntities()
    {
        _entities.clear();
        buildMap();

        Arcade::Object appleObj = {
            _apple, 'O', Arcade::Color::RED, 
            "./src/games/Snake/assets/apple.png"
        };
        _entities.push_back(appleObj);

        for (const auto& part : _body) {
            if (_body.front().x == part.x && _body.front().y == part.y) {
                Arcade::Object headObj = {
                    part, '@', Arcade::Color::GREEN, "./src/games/Snake/assets/head.png"
                };
                _entities.push_back(headObj);
                continue;
            }
            Arcade::Object snakeObj = {
                part, '#', Arcade::Color::GREEN, ""
            };
            _entities.push_back(snakeObj);
        }
    }

    const std::vector<Arcade::Object> &Snake::getEntities() const
    {
        return _entities;
    }

    bool Snake::isRunning() const
    {
        return _isRunning;
    }

    bool Snake::is_GameOver() const
    {
        return _isGameOver;
    }

    bool Snake::isGameWon() const
    {
        return _isWin;
    }
}

extern "C" {
    Arcade::IGame *entryPoint()
    {
        return new Arcade::Snake();
    }

    const char *getType()
    {
        return "game";
    }
}