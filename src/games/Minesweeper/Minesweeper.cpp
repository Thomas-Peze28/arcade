/*
** EPITECH PROJECT, 2026
** Arcade
** File description:
** Minesweeper
*/

#include "Minesweeper.hpp"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <string>

namespace Arcade {

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

    Minesweeper::Minesweeper()
        : _name("Minesweeper"), _boardW(9), _boardH(9), _mineCount(10),
          _cursor({4, 4}), _minesLeft(10), _score(0), _firstClick(true),
          _state(GameState::SELECTING), _diffCursor(0),
          _isRunning(false), _isGameOver(false), _isWin(false)
    {
        std::srand(std::time(nullptr));
    }

    void Minesweeper::init()
    {
        _isRunning  = true;
        _isGameOver = false;
        _isWin      = false;
        _firstClick = true;
        _score      = 0;
        _diffCursor = 0;
        _state      = GameState::SELECTING;
        _cells.clear();
        buildEntities();
    }

    void Minesweeper::stop()
    {
        _isRunning = false;
    }

    const std::string &Minesweeper::getName() const
    {
        return _name;
    }

    bool Minesweeper::isRunning() const
    {
        return _isRunning;
    }

    bool Minesweeper::is_GameOver() const
    {
        return _isGameOver;
    }

    bool Minesweeper::isGameWon() const
    {
        return _isWin;
    }

    void Minesweeper::applyDifficulty(int diffIndex)
    {
        const DifficultyConfig &cfg = DIFFICULTIES[diffIndex];
        _boardW     = cfg.boardW;
        _boardH     = cfg.boardH;
        _mineCount  = cfg.mineCount;
        _minesLeft  = _mineCount;
        _cursor     = {_boardW / 2, _boardH / 2};
        _firstClick = true;
        _cells.assign(_boardH, std::vector<Cell>(_boardW, Cell{}));
    }

    bool Minesweeper::isInBounds(Arcade::Position pos) const
    {
        return pos.x >= 0 && pos.x < _boardW && pos.y >= 0 && pos.y < _boardH;
    }

    void Minesweeper::resetBoard()
    {
        _cells.assign(_boardH, std::vector<Cell>(_boardW, Cell{}));
    }

    void Minesweeper::scatterMines(Arcade::Position safeCell)
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

    void Minesweeper::placeMines(Arcade::Position safeCell)
    {
        for (int attempt = 0; attempt < MAX_GEN_ATTEMPTS; attempt++) {
            resetBoard();
            scatterMines(safeCell);
            computeAdjacents();
            if (isBoardSolvable(safeCell))
                return;
        }
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
                        Arcade::Position nb = {x + dx, y + dy};
                        if (isInBounds(nb) && _cells[nb.y][nb.x].mine)
                            count++;
                    }
                _cells[y][x].adjacentCount = count;
            }
        }
    }

    void Minesweeper::simReveal(std::vector<CellState> &state, Arcade::Position pos) const
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
                int              flagged = 0;
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        if (dx == 0 && dy == 0) continue;
                        Arcade::Position nb = {x + dx, y + dy};
                        if (!isInBounds(nb)) continue;
                        int nIdx = nb.y * _boardW + nb.x;
                        if (state[nIdx] == CellState::FLAGGED)     flagged++;
                        else if (state[nIdx] == CellState::HIDDEN) hidden.push_back(nIdx);
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

    bool Minesweeper::isBoardSolvable(Arcade::Position firstClick) const
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

    void Minesweeper::floodReveal(Arcade::Position pos)
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

    void Minesweeper::revealCell(Arcade::Position pos)
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
            cell.state  = CellState::REVEALED;
            _isGameOver = true;
            _isRunning  = false;
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
            _score     = _minesLeft * 10;
            _isWin     = true;
            _isRunning = false;
        }
    }

    void Minesweeper::chordReveal(Arcade::Position pos)
    {
        if (!isInBounds(pos))
            return;
        const Cell &cell = _cells[pos.y][pos.x];
        if (cell.state != CellState::REVEALED || cell.adjacentCount == 0)
            return;

        int flagCount = 0;
        for (int dy = -1; dy <= 1; dy++)
            for (int dx = -1; dx <= 1; dx++) {
                if (dx == 0 && dy == 0) continue;
                Arcade::Position nb = {pos.x + dx, pos.y + dy};
                if (isInBounds(nb) && _cells[nb.y][nb.x].state == CellState::FLAGGED)
                    flagCount++;
            }
        if (flagCount != cell.adjacentCount)
            return;

        for (int dy = -1; dy <= 1; dy++)
            for (int dx = -1; dx <= 1; dx++) {
                if (dx == 0 && dy == 0) continue;
                Arcade::Position nb = {pos.x + dx, pos.y + dy};
                if (isInBounds(nb) && _cells[nb.y][nb.x].state == CellState::HIDDEN)
                    revealCell(nb);
            }
    }

    void Minesweeper::toggleFlag(Arcade::Position pos)
    {
        if (!isInBounds(pos))
            return;
        Cell &cell = _cells[pos.y][pos.x];
        if (cell.state == CellState::HIDDEN) {
            cell.state = CellState::FLAGGED;
            _minesLeft--;
        } else if (cell.state == CellState::FLAGGED) {
            cell.state = CellState::HIDDEN;
            _minesLeft++;
        }
    }

    void Minesweeper::handleInput(Arcade::Input input)
    {
        if (_state == GameState::SELECTING) {
            if (input == Arcade::Input::UP)
                _diffCursor = std::max(0, _diffCursor - 1);
            else if (input == Arcade::Input::DOWN)
                _diffCursor = std::min(2, _diffCursor + 1);
            else if (input == Arcade::Input::SPACE || input == Arcade::Input::ACTION) {
                applyDifficulty(_diffCursor);
                _state = GameState::PLAYING;
            }
            return;
        }

        if (input == Arcade::Input::UP)
            _cursor.y = std::max(0, _cursor.y - 1);
        else if (input == Arcade::Input::DOWN)
            _cursor.y = std::min(_boardH - 1, _cursor.y + 1);
        else if (input == Arcade::Input::LEFT)
            _cursor.x = std::max(0, _cursor.x - 1);
        else if (input == Arcade::Input::RIGHT)
            _cursor.x = std::min(_boardW - 1, _cursor.x + 1);
        else if (input == Arcade::Input::SPACE)
            revealCell(_cursor);
        else if (input == Arcade::Input::ACTION)
            toggleFlag(_cursor);
        else if (input == Arcade::Input::D)
            chordReveal(_cursor);
    }

    void Minesweeper::update()
    {
        buildEntities();
    }

    const std::vector<Arcade::Object> &Minesweeper::getEntities() const
    {
        return _entities;
    }

    Arcade::Color Minesweeper::numberColor(int n) const
    {
        static const Arcade::Color colors[9] = {
            Arcade::Color::WHITE,
            Arcade::Color::BLUE,
            Arcade::Color::GREEN,
            Arcade::Color::RED,
            Arcade::Color::BLUE,
            Arcade::Color::RED,
            Arcade::Color::GREEN,
            Arcade::Color::WHITE,
            Arcade::Color::WHITE,
        };
        return colors[n];
    }

    void Minesweeper::pushText(const std::string &text, int x, int y, Arcade::Color color)
    {
        for (size_t i = 0; i < text.size(); i++)
            _entities.push_back({{x + static_cast<int>(i), y}, text[i], color, ""});
    }

    void Minesweeper::buildStatusLine()
    {
        std::string status = "Mines: " + std::to_string(_minesLeft)
                           + "  SPACE:reveal  ACTION:flag  D:chord";
        pushText(status, 0, 0, Arcade::Color::WHITE);
        pushText("MINESWEEPER", 0, 1, Arcade::Color::BLUE);
    }

    void Minesweeper::buildBoard()
    {
        buildStatusLine();
        for (int y = 0; y < _boardH; y++) {
            for (int x = 0; x < _boardW; x++) {
                const Cell      &cell     = _cells[y][x];
                bool             isCursor = (x == _cursor.x && y == _cursor.y);
                Arcade::Position pos      = {x + BOARD_OFF_X, y + BOARD_OFF_Y};
                char             ch;
                Arcade::Color    col;

                if (cell.state == CellState::HIDDEN) {
                    ch  = '#';
                    col = isCursor ? Arcade::Color::GREEN : Arcade::Color::WHITE;
                } else if (cell.state == CellState::FLAGGED) {
                    ch  = 'F';
                    col = isCursor ? Arcade::Color::GREEN : Arcade::Color::RED;
                } else if (cell.mine) {
                    ch  = '*';
                    col = Arcade::Color::RED;
                } else if (cell.adjacentCount == 0) {
                    ch  = ' ';
                    col = isCursor ? Arcade::Color::GREEN : Arcade::Color::WHITE;
                } else {
                    ch  = static_cast<char>('0' + cell.adjacentCount);
                    col = isCursor ? Arcade::Color::GREEN : numberColor(cell.adjacentCount);
                }
                _entities.push_back({pos, ch, col, ""});
            }
        }
    }

    void Minesweeper::buildSelectScreen()
    {
        pushText("MINESWEEPER", 2, 0, Arcade::Color::BLUE);
        pushText("Select difficulty:", 2, 2, Arcade::Color::WHITE);

        for (int i = 0; i < 3; i++) {
            bool          selected = (i == _diffCursor);
            Arcade::Color col      = selected ? Arcade::Color::GREEN : Arcade::Color::WHITE;
            std::string   line     = (selected ? "> " : "  ") + std::string(DIFFICULTIES[i].label);
            pushText(line, 2, 4 + i, col);
        }

        pushText("UP/DOWN: navigate   SPACE: select", 2, 8, Arcade::Color::WHITE);
    }

    void Minesweeper::buildEntities()
    {
        _entities.clear();
        if (_state == GameState::SELECTING)
            buildSelectScreen();
        else
            buildBoard();
    }

}

extern "C" {
    Arcade::IGame *entryPoint()
    {
        return new Arcade::Minesweeper();
    }

    const char *getType()
    {
        return "game";
    }
}
