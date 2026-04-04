#include "Minesweeper.hpp"
#include <cstdlib>
#include <ctime>
#include <string>

namespace arcade {

    static const Color NUMBER_COLORS[9] = {
        {100, 100, 100, 255},
        {0,   100, 255, 255},
        {0,   180, 0,   255},
        {255, 60,  60,  255},
        {0,   0,   180, 255},
        {180, 0,   0,   255},
        {0,   180, 180, 255},
        {220, 220, 220, 255},
        {128, 128, 128, 255},
    };

    std::string Minesweeper::getName() const
    {
        return "Minesweeper";
    }

    void Minesweeper::init(IDisplayModule *display)
    {
        _display    = display;
        _running    = true;
        _firstClick = true;
        _gameOver   = false;
        _won        = false;
        _minesLeft  = MINE_COUNT;
        _cursor         = {BOARD_W / 2, BOARD_H / 2};
        _score          = 0;
        _lastClickCell  = {-1, -1};
        _lastClickTime  = {};

        for (int y = 0; y < BOARD_H; y++)
            for (int x = 0; x < BOARD_W; x++)
                _cells[y][x] = Cell{};

        std::srand(static_cast<unsigned>(std::time(nullptr)));
        display->setGridSize({BOARD_W + BOARD_OFF_X * 2, BOARD_H + BOARD_OFF_Y + 3});
    }

    bool Minesweeper::isInBounds(Vec2i pos) const
    {
        return pos.x >= 0 && pos.x < BOARD_W && pos.y >= 0 && pos.y < BOARD_H;
    }

    void Minesweeper::placeMines(Vec2i safeCell)
    {
        int placed = 0;
        while (placed < MINE_COUNT) {
            int x = std::rand() % BOARD_W;
            int y = std::rand() % BOARD_H;
            if (_cells[y][x].mine || (x == safeCell.x && y == safeCell.y))
                continue;
            _cells[y][x].mine = true;
            placed++;
        }
        computeAdjacents();
    }

    void Minesweeper::computeAdjacents()
    {
        for (int y = 0; y < BOARD_H; y++) {
            for (int x = 0; x < BOARD_W; x++) {
                if (_cells[y][x].mine) {
                    _cells[y][x].adjacentCount = 0;
                    continue;
                }
                int count = 0;
                for (int dy = -1; dy <= 1; dy++)
                    for (int dx = -1; dx <= 1; dx++) {
                        Vec2i nb = {x + dx, y + dy};
                        if (isInBounds(nb) && _cells[nb.y][nb.x].mine)
                            count++;
                    }
                _cells[y][x].adjacentCount = count;
            }
        }
    }

    void Minesweeper::floodReveal(Vec2i pos)
    {
        if (!isInBounds(pos))
            return;
        Cell &cell = _cells[pos.y][pos.x];
        if (cell.state != CellState::HIDDEN || cell.mine)
            return;
        cell.state = CellState::REVEALED;
        if (cell.adjacentCount == 0) {
            for (int dy = -1; dy <= 1; dy++)
                for (int dx = -1; dx <= 1; dx++)
                    if (dx != 0 || dy != 0)
                        floodReveal({pos.x + dx, pos.y + dy});
        }
    }

    void Minesweeper::revealAllMines()
    {
        for (int y = 0; y < BOARD_H; y++)
            for (int x = 0; x < BOARD_W; x++)
                if (_cells[y][x].mine && _cells[y][x].state != CellState::FLAGGED)
                    _cells[y][x].state = CellState::REVEALED;
    }

    void Minesweeper::chordReveal(Vec2i pos)
    {
        const Cell &cell = _cells[pos.y][pos.x];
        if (cell.state != CellState::REVEALED || cell.adjacentCount == 0)
            return;

        int flagCount = 0;
        for (int dy = -1; dy <= 1; dy++)
            for (int dx = -1; dx <= 1; dx++) {
                if (dx == 0 && dy == 0)
                    continue;
                Vec2i nb = {pos.x + dx, pos.y + dy};
                if (isInBounds(nb) && _cells[nb.y][nb.x].state == CellState::FLAGGED)
                    flagCount++;
            }

        if (flagCount != cell.adjacentCount)
            return;

        for (int dy = -1; dy <= 1; dy++)
            for (int dx = -1; dx <= 1; dx++) {
                if (dx == 0 && dy == 0)
                    continue;
                Vec2i nb = {pos.x + dx, pos.y + dy};
                if (isInBounds(nb) && _cells[nb.y][nb.x].state == CellState::HIDDEN)
                    revealCell(nb);
            }
    }

    void Minesweeper::revealCell(Vec2i pos)
    {
        if (!isInBounds(pos))
            return;
        Cell &cell = _cells[pos.y][pos.x];
        if (cell.state != CellState::HIDDEN)
            return;

        if (_firstClick) {
            _firstClick = false;
            placeMines(pos);
        }

        if (cell.mine) {
            cell.state = CellState::REVEALED;
            _gameOver  = true;
            _running   = false;
            revealAllMines();
            return;
        }
        floodReveal(pos);

        int revealed = 0;
        for (int y = 0; y < BOARD_H; y++)
            for (int x = 0; x < BOARD_W; x++)
                if (_cells[y][x].state == CellState::REVEALED)
                    revealed++;
        if (revealed == BOARD_W * BOARD_H - MINE_COUNT) {
            _won     = true;
            _running = false;
            _score   = _minesLeft * 10;
        }
    }

    bool Minesweeper::update(float, const std::vector<Event> &events)
    {
        if (!_display || (!_running && !_gameOver && !_won))
            return false;

        for (const Event &event : events) {
            if (_gameOver || _won)
                break;
            if (event.type == EventType::MOUSE) {
                Vec2i cell = {event.mousePos.x - BOARD_OFF_X, event.mousePos.y - BOARD_OFF_Y};
                if (!isInBounds(cell))
                    continue;
                _cursor = cell;
                if (event.mouseButton == MouseButton::LEFT) {
                    auto now     = Clock::now();
                    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                                       now - _lastClickTime).count();
                    if (elapsed < DOUBLE_CLICK_MS
                            && cell.x == _lastClickCell.x
                            && cell.y == _lastClickCell.y) {
                        chordReveal(cell);
                        _lastClickCell = {-1, -1};
                    } else {
                        revealCell(cell);
                        _lastClickCell = cell;
                        _lastClickTime = now;
                    }
                } else if (event.mouseButton == MouseButton::RIGHT) {
                    Cell &c = _cells[cell.y][cell.x];
                    if (c.state == CellState::HIDDEN) {
                        c.state = CellState::FLAGGED;
                        _minesLeft--;
                    } else if (c.state == CellState::FLAGGED) {
                        c.state = CellState::HIDDEN;
                        _minesLeft++;
                    }
                }
                continue;
            }
            switch (event.key) {
                case KeyCode::UP:    _cursor.y = std::max(0, _cursor.y - 1);              break;
                case KeyCode::DOWN:  _cursor.y = std::min(BOARD_H - 1, _cursor.y + 1);   break;
                case KeyCode::LEFT:  _cursor.x = std::max(0, _cursor.x - 1);              break;
                case KeyCode::RIGHT: _cursor.x = std::min(BOARD_W - 1, _cursor.x + 1);   break;
                case KeyCode::SPACE: revealCell(_cursor);                                  break;
                case KeyCode::ENTER: {
                    Cell &c = _cells[_cursor.y][_cursor.x];
                    if (c.state == CellState::HIDDEN) {
                        c.state = CellState::FLAGGED;
                        _minesLeft--;
                    } else if (c.state == CellState::FLAGGED) {
                        c.state = CellState::HIDDEN;
                        _minesLeft++;
                    }
                    break;
                }
                default: break;
            }
        }
        return _running;
    }

    Tile Minesweeper::tileForCell(Vec2i pos) const
    {
        const Cell &cell = _cells[pos.y][pos.x];
        bool isCursor = (pos.x == _cursor.x && pos.y == _cursor.y);

        Color bg = isCursor ? Color{180, 180, 0, 255} : Color{30, 30, 30, 255};

        if (cell.state == CellState::HIDDEN)
            return {'#', {200, 200, 200, 255}, bg};
        if (cell.state == CellState::FLAGGED)
            return {'F', {255, 200, 0, 255}, bg};
        if (cell.mine)
            return {'*', {255, 0, 0, 255}, {200, 0, 0, 255}};
        if (cell.adjacentCount == 0)
            return {' ', {100, 100, 100, 255}, {20, 20, 20, 255}};
        char sym = static_cast<char>('0' + cell.adjacentCount);
        return {sym, NUMBER_COLORS[cell.adjacentCount], bg};
    }

    void Minesweeper::render(IDisplayModule *display)
    {
        for (int y = 0; y < BOARD_H; y++)
            for (int x = 0; x < BOARD_W; x++)
                display->drawTile({x + BOARD_OFF_X, y + BOARD_OFF_Y}, tileForCell({x, y}));

        std::string status;
        if (_gameOver)
            status = "  GAME OVER! Mines: " + std::to_string(_minesLeft);
        else if (_won)
            status = "  YOU WIN! Score: " + std::to_string(_score);
        else
            status = "  Mines left: " + std::to_string(_minesLeft) + "  [SPACE=reveal  ENTER=flag]";

        display->drawText(status, {0, 1}, {255, 255, 255, 255});
    }

} // namespace arcade

arcade::IGameModule *createGame()
{
    return new arcade::Minesweeper();
}

arcade::LibType getLibType()
{
    return arcade::LibType::GAME;
}
