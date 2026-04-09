/*
** EPITECH PROJECT, 2026
** Arcade
** File description:
** SnakeGame
*/

#include "IGame.hpp"
#include <vector>
#include <deque>
#include <random>
#include <algorithm>

namespace Arcade {

class SnakeGame : public IGame {
public:
    SnakeGame() : _name("Snake"), _running(false), _direction(RIGHT) {
        initMap();
    }

    ~SnakeGame() override = default;

    void init() override {
        _running = true;
        _collisionGameOver = false;
        _direction = RIGHT;
        _snake.clear();
        _snake.push_back({8, 3});
        _snake.push_back({8, 4});
        _snake.push_back({8, 5});
        _snake.push_back({8, 6});
        placeFood();
        _gameWon = false;
        updateMap();
    }

    void stop() override {
        _running = false;
        _collisionGameOver = false;
        _gameWon = false;
    }

    const std::string &getName() const override {
        return _name;
    }

    bool isRunning() const override {
        return _running;
    }

    bool is_GameOver() const override {
        return _collisionGameOver;
    }

    bool isGameWon() const override {
        return _gameWon;
    }

    void update() override {
        if (_running) {
            updateMap();
        }
    }

    void handleInput(Input input) override {
        if (!_running) return;

        bool moved = false;

        if (input == Input::UP && _direction != DOWN) {
            _direction = UP;
            moved = true;
        } else if (input == Input::DOWN && _direction != UP) {
            _direction = DOWN;
            moved = true;
        } else if (input == Input::LEFT && _direction != RIGHT) {
            _direction = LEFT;
            moved = true;
        } else if (input == Input::RIGHT && _direction != LEFT) {
            _direction = RIGHT;
            moved = true;
        } else if (input == Input::RESTART) {
            init();
            return;
        }

        if (moved) {
            moveSnake();

            if (checkCollision()) {
                _collisionGameOver = true;
                _running = false;
                return;
            }

            if (_map[_snake.back().y][_snake.back().x] == '@') {
                growSnake();
                placeFood();
            }

            updateMap();
        }
    }

    const std::vector<Object> &getEntities() const override {
        return _entities;
    }

private:
    enum Direction { UP, DOWN, LEFT, RIGHT };

    std::string _name;
    bool _running;
    bool _collisionGameOver;
    bool _gameWon;
    Direction _direction;
    std::vector<std::vector<char>> _map;
    std::deque<Position> _snake;
    Position _food;
    mutable std::vector<Object> _entities;

    void initMap() {
        _map = std::vector<std::vector<char>>(15, std::vector<char>(15, ' '));
        for (int i = 0; i < 15; ++i) {
            _map[0][i] = '#';
            _map[14][i] = '#';
            _map[i][0] = '#';
            _map[i][14] = '#';
        }
    }

    void placeFood() {
        std::vector<Position> emptyCells;
        for (int y = 1; y < 14; ++y) {
            for (int x = 1; x < 14; ++x) {
                if (_map[y][x] == ' ') {
                    bool inSnake = false;
                    for (const auto &p : _snake) {
                        if (p.x == x && p.y == y) {
                            inSnake = true;
                            break;
                        }
                    }
                    if (!inSnake) {
                        emptyCells.push_back({x, y});
                    }
                }
            }
        }
        if (emptyCells.empty()) {
            _collisionGameOver = false;
            _gameWon = (_snake.size() == 13 * 13);
            _running = false;
            return;
        }
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, emptyCells.size() - 1);
        _food = emptyCells[dis(gen)];
        _map[_food.y][_food.x] = '@';
    }

    void moveSnake() {
        Position head = _snake.back();
        Position newHead = head;
        if (_direction == UP) newHead.y--;
        else if (_direction == DOWN) newHead.y++;
        else if (_direction == LEFT) newHead.x--;
        else if (_direction == RIGHT) newHead.x++;

        _snake.push_back(newHead);
        _snake.pop_front();
    }

    void growSnake() {
        Position tail = _snake.front();
        Position newTail = tail;
        if (_snake.size() > 1) {
            Position prev = _snake[1];
            if (prev.x < tail.x) newTail.x++;
            else if (prev.x > tail.x) newTail.x--;
            else if (prev.y < tail.y) newTail.y++;
            else if (prev.y > tail.y) newTail.y--;
        }

        Position last = _snake.front();
        std::vector<Position> candidates = {{last.x-1, last.y}, {last.x+1, last.y}, {last.x, last.y-1}, {last.x, last.y+1}};
        for (auto &c : candidates) {
            if (c.x >= 0 && c.x < 15 && c.y >= 0 && c.y < 15 && _map[c.y][c.x] == ' ') {
                _snake.push_front(c);
                return;
            }
        }
        _collisionGameOver = false;
        _gameWon = false;
        _running = false;
    }

    bool checkCollision() {
        Position head = _snake.back();
        if (_map[head.y][head.x] == '#') return true;
        for (size_t i = 0; i < _snake.size() - 1; ++i) {
            if (_snake[i].x == head.x && _snake[i].y == head.y) return true;
        }
        return false;
    }

    void updateMap() {
        for (int y = 1; y < 14; ++y) {
            for (int x = 1; x < 14; ++x) {
                if (_map[y][x] != '#' && _map[y][x] != '@') _map[y][x] = ' ';
            }
        }
        if (!_snake.empty()) {
            _map[_snake.front().y][_snake.front().x] = '.';
            for (size_t i = 1; i < _snake.size() - 1; ++i) {
                _map[_snake[i].y][_snake[i].x] = '+';
            }
            _map[_snake.back().y][_snake.back().x] = '0';
        }

        _entities.clear();
        for (int y = 0; y < 15; ++y) {
            for (int x = 0; x < 15; ++x) {
                if (_map[y][x] != ' ') {
                    Color color = Color::WHITE;
                    if (_map[y][x] == '@') color = Color::RED;
                    else if (_map[y][x] == '0') color = Color::GREEN;
                    else if (_map[y][x] == '+') color = Color::BLUE;
                    else if (_map[y][x] == '.') color = Color::BLUE;
                    _entities.push_back({{x, y}, _map[y][x], color, ""});
                }
            }
        }
    }
};

extern "C" {
    IGame *entryPoint() {
        return new SnakeGame();
    }

    const char *getType() {
        return "game";
    }
}
}