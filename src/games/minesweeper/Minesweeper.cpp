#include "Minesweeper.hpp"
#include <algorithm>
#include <cstdlib>
#include <string>

namespace arcade {

    static constexpr int SELECT_GRID_W = 40;
    static constexpr int SELECT_GRID_H = 12;

    struct DifficultyConfig {
        const char *label;
        int         boardW;
        int         boardH;
        int         mineCount;
    };

    static constexpr DifficultyConfig DIFFICULTIES[3] = {
        {"Beginner     (9x9,   10 mines)",  9,  9,  10},
        {"Intermediate (16x16, 40 mines)", 16, 16,  40},
        {"Expert       (30x16, 99 mines)", 30, 16,  99},
    };

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

    void Minesweeper::applyDifficulty(IDisplayModule *display)
    {
        const DifficultyConfig &cfg = DIFFICULTIES[static_cast<int>(_difficulty)];
        _boardW     = cfg.boardW;
        _boardH     = cfg.boardH;
        _mineCount  = cfg.mineCount;
        _minesLeft  = _mineCount;
        _cursor     = {_boardW / 2, _boardH / 2};

        _cells.assign(_boardH, std::vector<Cell>(_boardW, Cell{}));

        int gridW = std::max(_boardW + BOARD_OFF_X * 2, SELECT_GRID_W);
        int gridH = _boardH + BOARD_OFF_Y + 3;
        display->setGridSize({gridW, gridH});
    }

    void Minesweeper::init(IDisplayModule *display)
    {
        _display       = display;
        _running       = true;
        _firstClick    = true;
        _score         = 0;
        _diffCursor    = 0;
        _lastClickCell = {-1, -1};
        _lastClickTime = {};
        _state         = GameState::SELECTING;

        display->setGridSize({SELECT_GRID_W, SELECT_GRID_H});
    }

    void Minesweeper::setDisplay(IDisplayModule *display)
    {
        _display = display;
        if (_state == GameState::SELECTING) {
            display->setGridSize({SELECT_GRID_W, SELECT_GRID_H});
        } else {
            int gridW = std::max(_boardW + BOARD_OFF_X * 2, SELECT_GRID_W);
            int gridH = _boardH + BOARD_OFF_Y + 3;
            display->setGridSize({gridW, gridH});
        }
    }

    bool Minesweeper::isInBounds(Vec2i pos) const
    {
        return pos.x >= 0 && pos.x < _boardW && pos.y >= 0 && pos.y < _boardH;
    }

    void Minesweeper::resetBoard()
    {
        _cells.assign(_boardH, std::vector<Cell>(_boardW, Cell{}));
    }

    void Minesweeper::scatterMines(Vec2i safeCell)
    {
        int placed = 0;
        while (placed < _mineCount) {
            int x = std::rand() % _boardW;
            int y = std::rand() % _boardH;
            if (_cells[y][x].mine)
                continue;
            if (std::abs(x - safeCell.x) <= SAFE_ZONE_RADIUS
                    && std::abs(y - safeCell.y) <= SAFE_ZONE_RADIUS)
                continue;
            _cells[y][x].mine = true;
            placed++;
        }
    }

    void Minesweeper::placeMines(Vec2i safeCell)
    {
        for (int attempt = 0; attempt < MAX_GEN_ATTEMPTS; attempt++) {
            resetBoard();
            scatterMines(safeCell);
            computeAdjacents();
            if (isBoardSolvable(safeCell))
                return;
        }
    }

    void Minesweeper::simReveal(std::vector<CellState> &state, Vec2i pos) const
    {
        if (!isInBounds(pos))
            return;
        int idx = pos.y * _boardW + pos.x;
        if (state[idx] != CellState::HIDDEN || _cells[pos.y][pos.x].mine)
            return;
        state[idx] = CellState::REVEALED;
        if (_cells[pos.y][pos.x].adjacentCount == 0) {
            for (int dy = -1; dy <= 1; dy++)
                for (int dx = -1; dx <= 1; dx++)
                    if (dx != 0 || dy != 0)
                        simReveal(state, {pos.x + dx, pos.y + dy});
        }
    }

    std::vector<Minesweeper::SolverConstraint>
    Minesweeper::buildConstraints(const std::vector<CellState> &state) const
    {
        std::vector<SolverConstraint> constraints;
        for (int y = 0; y < _boardH; y++) {
            for (int x = 0; x < _boardW; x++) {
                if (state[y * _boardW + x] != CellState::REVEALED)
                    continue;
                int adj = _cells[y][x].adjacentCount;
                if (adj == 0)
                    continue;
                std::vector<int> hidden;
                int flagged = 0;
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        if (dx == 0 && dy == 0) continue;
                        Vec2i nb = {x + dx, y + dy};
                        if (!isInBounds(nb)) continue;
                        int nIdx = nb.y * _boardW + nb.x;
                        if (state[nIdx] == CellState::FLAGGED)       flagged++;
                        else if (state[nIdx] == CellState::HIDDEN)   hidden.push_back(nIdx);
                    }
                }
                if (!hidden.empty()) {
                    std::sort(hidden.begin(), hidden.end());
                    constraints.push_back({hidden, adj - flagged});
                }
            }
        }
        return constraints;
    }

    bool Minesweeper::applyConstraint(const SolverConstraint &c, std::vector<CellState> &state) const
    {
        bool progress = false;
        if (c.mines == 0) {
            for (int idx : c.cells)
                if (state[idx] == CellState::HIDDEN) {
                    simReveal(state, {idx % _boardW, idx / _boardW});
                    progress = true;
                }
        } else if (c.mines == static_cast<int>(c.cells.size())) {
            for (int idx : c.cells)
                if (state[idx] == CellState::HIDDEN) {
                    state[idx] = CellState::FLAGGED;
                    progress = true;
                }
        }
        return progress;
    }

    bool Minesweeper::applySetReduction(
        const std::vector<SolverConstraint> &constraints, std::vector<CellState> &state) const
    {
        auto tryDerive = [&](const SolverConstraint &sub, const SolverConstraint &super) {
            if (sub.cells.size() >= super.cells.size())
                return false;
            if (!std::includes(super.cells.begin(), super.cells.end(),
                               sub.cells.begin(), sub.cells.end()))
                return false;
            SolverConstraint derived;
            std::set_difference(super.cells.begin(), super.cells.end(),
                                sub.cells.begin(), sub.cells.end(),
                                std::back_inserter(derived.cells));
            derived.mines = super.mines - sub.mines;
            return applyConstraint(derived, state);
        };

        for (size_t i = 0; i < constraints.size(); i++)
            for (size_t j = i + 1; j < constraints.size(); j++)
                if (tryDerive(constraints[i], constraints[j])
                        || tryDerive(constraints[j], constraints[i]))
                    return true;
        return false;
    }

    bool Minesweeper::solvePass(std::vector<CellState> &state) const
    {
        auto constraints = buildConstraints(state);
        for (const auto &c : constraints)
            if (applyConstraint(c, state))
                return true;
        return applySetReduction(constraints, state);
    }

    bool Minesweeper::isBoardSolvable(Vec2i firstClick) const
    {
        std::vector<CellState> state(_boardW * _boardH, CellState::HIDDEN);
        simReveal(state, firstClick);
        while (solvePass(state))
            ;
        for (int y = 0; y < _boardH; y++)
            for (int x = 0; x < _boardW; x++)
                if (!_cells[y][x].mine && state[y * _boardW + x] != CellState::REVEALED)
                    return false;
        return true;
    }

    void Minesweeper::computeAdjacents()
    {
        for (int y = 0; y < _boardH; y++) {
            for (int x = 0; x < _boardW; x++) {
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
        for (int y = 0; y < _boardH; y++)
            for (int x = 0; x < _boardW; x++)
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
            _state     = GameState::GAME_OVER;
            _running   = false;
            revealAllMines();
            return;
        }
        floodReveal(pos);

        int revealed = 0;
        for (int y = 0; y < _boardH; y++)
            for (int x = 0; x < _boardW; x++)
                if (_cells[y][x].state == CellState::REVEALED)
                    revealed++;
        if (revealed == _boardW * _boardH - _mineCount) {
            _state   = GameState::WON;
            _running = false;
            _score   = _minesLeft * 10;
        }
    }

    bool Minesweeper::update(float, const std::vector<Event> &events)
    {
        if (!_display)
            return false;

        for (const Event &event : events) {
            if (_state == GameState::SELECTING) {
                if (event.type == EventType::KEYBOARD) {
                    switch (event.key) {
                        case KeyCode::UP:
                            _diffCursor = std::max(0, _diffCursor - 1);
                            break;
                        case KeyCode::DOWN:
                            _diffCursor = std::min(2, _diffCursor + 1);
                            break;
                        case KeyCode::ENTER:
                        case KeyCode::SPACE:
                            _difficulty = static_cast<Difficulty>(_diffCursor);
                            applyDifficulty(_display);
                            _state = GameState::PLAYING;
                            break;
                        default: break;
                    }
                }
                continue;
            }

            if (_state == GameState::GAME_OVER || _state == GameState::WON)
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
                case KeyCode::UP:    _cursor.y = std::max(0, _cursor.y - 1);               break;
                case KeyCode::DOWN:  _cursor.y = std::min(_boardH - 1, _cursor.y + 1);     break;
                case KeyCode::LEFT:  _cursor.x = std::max(0, _cursor.x - 1);               break;
                case KeyCode::RIGHT: _cursor.x = std::min(_boardW - 1, _cursor.x + 1);     break;
                case KeyCode::SPACE: revealCell(_cursor);                                   break;
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
        const Cell &cell    = _cells[pos.y][pos.x];
        bool        isCursor = (pos.x == _cursor.x && pos.y == _cursor.y);

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

    void Minesweeper::renderSelectScreen(IDisplayModule *display) const
    {
        const Color white  = {255, 255, 255, 255};
        const Color yellow = {255, 220, 0,   255};
        const Color gray   = {150, 150, 150, 255};
        const Color cyan   = {0,   220, 220, 255};

        display->drawText("  MINESWEEPER", {0, 1}, cyan);
        display->drawText("  Select difficulty:", {0, 3}, gray);

        for (int i = 0; i < 3; i++) {
            bool        selected = (i == _diffCursor);
            Color       col      = selected ? yellow : white;
            std::string line     = (selected ? "> " : "  ") + std::string(DIFFICULTIES[i].label);
            display->drawText(line, {0, 5 + i}, col);
        }

        display->drawText("  UP/DOWN navigate   ENTER select", {0, 10}, gray);
    }

    void Minesweeper::renderBoard(IDisplayModule *display) const
    {
        for (int y = 0; y < _boardH; y++)
            for (int x = 0; x < _boardW; x++)
                display->drawTile({x + BOARD_OFF_X, y + BOARD_OFF_Y}, tileForCell({x, y}));

        std::string status;
        if (_state == GameState::GAME_OVER)
            status = "  GAME OVER!  Mines: " + std::to_string(_minesLeft);
        else if (_state == GameState::WON)
            status = "  YOU WIN!  Score: " + std::to_string(_score);
        else
            status = "  Mines: " + std::to_string(_minesLeft) + "  SPACE=reveal  ENTER=flag";

        display->drawText(status, {0, 1}, {255, 255, 255, 255});
    }

    void Minesweeper::render(IDisplayModule *display)
    {
        if (_state == GameState::SELECTING)
            renderSelectScreen(display);
        else
            renderBoard(display);
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
