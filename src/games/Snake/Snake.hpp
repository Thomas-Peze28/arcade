/*
** EPITECH PROJECT, 2026
** Arcade
** File description:
** Snake
*/

#ifndef SNAKE_HPP_
    #define SNAKE_HPP_

#include "IGame.hpp"
#include <vector>
#include <string>
#include <chrono>

namespace Arcade {
    class Snake : public IGame {
        public:
            Snake();
            ~Snake() override = default;

            void init() override;
            void stop() override;
            const std::string &getName() const override;
            void handleInput(Arcade::Input input) override;
            void update() override;
            const std::vector<Arcade::Object> &getEntities() const override;

            bool isRunning() const override;
            bool is_GameOver() const override;
            bool isGameWon() const override;

        private:
            
            void spawnApple();
            void buildEntities();
            void buildMap();
            void isCollision();
            void isApple();
            void moveSnake();            

            std::string _name;
            std::vector<Arcade::Object> _entities;

            std::vector<Arcade::Position> _body;
            Arcade::Position _apple;
            Arcade::Input _direction;
            Arcade::Input _nextDirection;
            int _map_size;

            std::chrono::steady_clock::time_point _lastUpdate;
            int _speedMs;

            bool _isRunning;
            bool _isGameOver;
            bool _isWin;

            int _offsetX;
            int _offsetY;
    };
}

#endif /* SNAKE_HPP_ */