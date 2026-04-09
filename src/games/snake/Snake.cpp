#include "Snake.hpp"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <string>

namespace arcade {

    std::string Snake::getName() const
    {
        return "Snake";
    }

    void Snake::init(IDisplayModule *display)
    {
        _display    = display;
        _running    = true;
        _score      = 0;
        _gameOver   = false;
        _paused     = false;
        _accum      = 0.f;
        _dir        = Direction::RIGHT;
        _pendingDir = Direction::RIGHT;

        _body.clear();
        int cx = GRID_W / 2;
        int cy = GRID_H / 2;
        _body.push_back({cx,     cy});
        _body.push_back({cx - 1, cy});
        _body.push_back({cx - 2, cy});

        std::srand(static_cast<unsigned>(std::time(nullptr)));
        spawnFood();
        display->setGridSize({GRID_W + 2 * BORDER, GRID_H + HUD_H + 2 * BORDER});
    }

    bool Snake::isOnSnake(Vec2i pos) const
    {
        for (const Vec2i &seg : _body)
            if (seg.x == pos.x && seg.y == pos.y)
                return true;
        return false;
    }

    void Snake::spawnFood()
    {
        Vec2i candidate;
        do {
            candidate = {std::rand() % GRID_W, std::rand() % GRID_H};
        } while (isOnSnake(candidate));
        _food = candidate;
    }

    void Snake::queueDirection(Direction d)
    {
        bool sameAxisOpposite =
            (_dir == Direction::UP    && d == Direction::DOWN)  ||
            (_dir == Direction::DOWN  && d == Direction::UP)    ||
            (_dir == Direction::LEFT  && d == Direction::RIGHT) ||
            (_dir == Direction::RIGHT && d == Direction::LEFT);
        if (!sameAxisOpposite)
            _pendingDir = d;
    }

    float Snake::tickInterval() const
    {
        float interval = TICK_BASE - (_score / 50.f) * 0.015f;
        return std::max(TICK_MIN, interval);
    }

    void Snake::step()
    {
        _dir = _pendingDir;

        Vec2i next = _body.front();
        if (_dir == Direction::UP)    next.y -= 1;
        if (_dir == Direction::DOWN)  next.y += 1;
        if (_dir == Direction::LEFT)  next.x -= 1;
        if (_dir == Direction::RIGHT) next.x += 1;

        if (next.x < 0 || next.x >= GRID_W || next.y < 0 || next.y >= GRID_H) {
            _running  = false;
            _gameOver = true;
            return;
        }

        for (size_t i = 0; i < _body.size() - 1; i++) {
            if (_body[i].x == next.x && _body[i].y == next.y) {
                _running  = false;
                _gameOver = true;
                return;
            }
        }

        _body.push_front(next);

        if (next.x == _food.x && next.y == _food.y) {
            _score += 10;
            spawnFood();
        } else {
            _body.pop_back();
        }
    }

    bool Snake::update(float deltaTime, const std::vector<Event> &events)
    {
        if (!_running)
            return false;

        for (const Event &event : events) {
            switch (event.key) {
                case KeyCode::UP:    queueDirection(Direction::UP);    break;
                case KeyCode::DOWN:  queueDirection(Direction::DOWN);  break;
                case KeyCode::LEFT:  queueDirection(Direction::LEFT);  break;
                case KeyCode::RIGHT: queueDirection(Direction::RIGHT); break;
                case KeyCode::SPACE: _paused = !_paused;               break;
                default: break;
            }
        }

        if (_paused)
            return _running;

        _accum += deltaTime;
        while (_accum >= tickInterval()) {
            _accum -= tickInterval();
            step();
            if (!_running)
                break;
        }
        return _running;
    }

    void Snake::render(IDisplayModule *display)
    {
        std::string hud;
        if (_gameOver)
            hud = "  GAME OVER - Score: " + std::to_string(_score);
        else if (_paused)
            hud = "  PAUSED - Score: " + std::to_string(_score);
        else
            hud = "  Score: " + std::to_string(_score) + "  [arrows=move  SPACE=pause]";
        display->drawText(hud, {0, 1}, {255, 255, 255, 255});

        static const Color BORDER_COLOR = {80, 80, 180, 255};
        static const Color BORDER_BG    = {15, 15, 15, 255};
        int top    = HUD_H;
        int bottom = HUD_H + BORDER + GRID_H;
        int left   = 0;
        int right  = BORDER + GRID_W;

        for (int x = left; x <= right; x++) {
            char sym = (x == left || x == right) ? '+' : '-';
            display->drawTile({x, top},    {sym, BORDER_COLOR, BORDER_BG});
            display->drawTile({x, bottom}, {sym, BORDER_COLOR, BORDER_BG});
        }
        for (int y = top + 1; y < bottom; y++) {
            display->drawTile({left,  y}, {'|', BORDER_COLOR, BORDER_BG});
            display->drawTile({right, y}, {'|', BORDER_COLOR, BORDER_BG});
        }

        for (size_t i = 0; i < _body.size(); i++) {
            Vec2i screenPos = {_body[i].x + BORDER, _body[i].y + HUD_H + BORDER};
            if (i == 0)
                display->drawTile(screenPos, {'@', {0, 255, 0, 255}, {20, 20, 20, 255}});
            else
                display->drawTile(screenPos, {'o', {0, 180, 0, 255}, {20, 20, 20, 255}});
        }

        display->drawTile({_food.x + BORDER, _food.y + HUD_H + BORDER}, {'*', {255, 60, 60, 255}, {20, 20, 20, 255}});
    }

} // namespace arcade

arcade::IGameModule *createGame()
{
    return new arcade::Snake();
}

arcade::LibType getLibType()
{
    return arcade::LibType::GAME;
}
