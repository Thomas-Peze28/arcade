#pragma once

#include "../../abstracts/AGameModule.hpp"
#include "../../shared/LibType.hpp"
#include <chrono>
#include <vector>

namespace arcade {

    static constexpr int BOARD_OFF_X = 2;
    static constexpr int BOARD_OFF_Y = 3;

    enum class Difficulty { BEGINNER, INTERMEDIATE, EXPERT };

    enum class CellState { HIDDEN, REVEALED, FLAGGED };

    struct Cell {
        bool      mine          = false;
        CellState state         = CellState::HIDDEN;
        int       adjacentCount = 0;
    };

    class Minesweeper final : public AGameModule {
    public:
        Minesweeper()  = default;
        ~Minesweeper() override = default;

        std::string getName() const override;

        void init(IDisplayModule *display)                                       override;
        void setDisplay(IDisplayModule *display)                                 override;
        bool update(float deltaTime, const std::vector<Event> &events)           override;
        void render(IDisplayModule *display)                                     override;

    private:
        enum class GameState { SELECTING, PLAYING, GAME_OVER, WON };

        using Clock     = std::chrono::steady_clock;
        using TimePoint = std::chrono::steady_clock::time_point;

        static constexpr int DOUBLE_CLICK_MS    = 400;
        static constexpr int SAFE_ZONE_RADIUS   = 2;
        static constexpr int MAX_GEN_ATTEMPTS   = 1000;

        struct SolverConstraint {
            std::vector<int> cells;
            int mines;
        };

        void applyDifficulty(IDisplayModule *display);
        void placeMines(Vec2i safeCell);
        void scatterMines(Vec2i safeCell);
        void resetBoard();
        void computeAdjacents();
        void revealCell(Vec2i pos);
        void floodReveal(Vec2i pos);
        void revealAllMines();
        void chordReveal(Vec2i pos);
        bool isInBounds(Vec2i pos) const;
        Tile tileForCell(Vec2i pos) const;
        void renderSelectScreen(IDisplayModule *display) const;
        void renderBoard(IDisplayModule *display) const;

        bool isBoardSolvable(Vec2i firstClick) const;
        void simReveal(std::vector<CellState> &state, Vec2i pos) const;
        bool solvePass(std::vector<CellState> &state) const;
        std::vector<SolverConstraint> buildConstraints(const std::vector<CellState> &state) const;
        bool applyConstraint(const SolverConstraint &c, std::vector<CellState> &state) const;
        bool applySetReduction(const std::vector<SolverConstraint> &constraints,
                               std::vector<CellState> &state) const;

        std::vector<std::vector<Cell>> _cells;
        int        _boardW        = 9;
        int        _boardH        = 9;
        int        _mineCount     = 10;

        Vec2i      _cursor        = {4, 4};
        int        _minesLeft     = 10;
        bool       _firstClick    = true;
        GameState  _state         = GameState::SELECTING;
        Difficulty _difficulty    = Difficulty::BEGINNER;
        int        _diffCursor    = 0;

        Vec2i      _lastClickCell = {-1, -1};
        TimePoint  _lastClickTime = {};
    };

} // namespace arcade

extern "C" arcade::IGameModule *createGame();
extern "C" arcade::LibType      getLibType();
