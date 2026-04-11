/*
** EPITECH PROJECT, 2026
** Arcade
** File description:
** Minesweeper
*/

#ifndef MINESWEEPER_HPP_
#define MINESWEEPER_HPP_

#include "IGame.hpp"
#include <vector>
#include <string>

namespace Arcade {

    class Minesweeper final : public IGame {
    public:
        Minesweeper();
        ~Minesweeper() override = default;

        void                               init()                             override;
        void                               stop()                             override;
        const std::string                 &getName()                  const   override;
        bool                               isRunning()                const   override;
        void                               handleInput(Arcade::Input)         override;
        bool                               is_GameOver()              const   override;
        bool                               isGameWon()                const   override;
        void                               update()                           override;
        const std::vector<Arcade::Object> &getEntities()              const   override;

    private:
        enum class CellState { HIDDEN, REVEALED, FLAGGED };
        enum class GameState { SELECTING, PLAYING };

        struct Cell {
            bool      mine          = false;
            CellState state         = CellState::HIDDEN;
            int       adjacentCount = 0;
        };

        struct SolverConstraint {
            std::vector<int> cells;
            int              mines;
        };

        static constexpr int SAFE_ZONE_RADIUS = 2;
        static constexpr int MAX_GEN_ATTEMPTS = 1000;
        static constexpr int BOARD_OFF_X      = 2;
        static constexpr int BOARD_OFF_Y      = 3;

        void applyDifficulty(int diffIndex);
        void resetBoard();
        void scatterMines(Arcade::Position safeCell);
        void placeMines(Arcade::Position safeCell);
        void computeAdjacents();
        bool isInBounds(Arcade::Position pos) const;

        void                          simReveal(std::vector<CellState> &state, Arcade::Position pos) const;
        std::vector<SolverConstraint> buildConstraints(const std::vector<CellState> &state) const;
        bool                          applyConstraint(const SolverConstraint &c, std::vector<CellState> &state) const;
        bool                          applySetReduction(const std::vector<SolverConstraint> &constraints,
                                                         std::vector<CellState> &state) const;
        bool                          solvePass(std::vector<CellState> &state) const;
        bool                          isBoardSolvable(Arcade::Position firstClick) const;

        void floodReveal(Arcade::Position pos);
        void revealAllMines();
        void revealCell(Arcade::Position pos);
        void chordReveal(Arcade::Position pos);
        void toggleFlag(Arcade::Position pos);

        void buildEntities();
        void buildSelectScreen();
        void buildBoard();
        void buildStatusLine();
        void pushText(const std::string &text, int x, int y, Arcade::Color color);

        Arcade::Color numberColor(int n) const;

        std::string                    _name;
        std::vector<Arcade::Object>    _entities;
        std::vector<std::vector<Cell>> _cells;

        int              _boardW;
        int              _boardH;
        int              _mineCount;
        Arcade::Position _cursor;
        int              _minesLeft;
        int              _score;
        bool             _firstClick;
        GameState        _state;
        int              _diffCursor;
        bool             _isRunning;
        bool             _isGameOver;
        bool             _isWin;
    };

}

#endif /* MINESWEEPER_HPP_ */
