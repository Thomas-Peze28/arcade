#pragma once

#include "../../abstracts/AGameModule.hpp"
#include "../../shared/LibType.hpp"

namespace arcade {

    static constexpr int BOARD_W     = 16;
    static constexpr int BOARD_H     = 16;
    static constexpr int MINE_COUNT  = 40;
    static constexpr int BOARD_OFF_X = 2;
    static constexpr int BOARD_OFF_Y = 3;

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

        void init(IDisplayModule *display)   override;
        bool update(float deltaTime, const std::vector<Event> &events) override;
        void render(IDisplayModule *display) override;

    private:
        void placeMines(Vec2i safeCell);
        void computeAdjacents();
        void revealCell(Vec2i pos);
        void floodReveal(Vec2i pos);
        void revealAllMines();
        bool isInBounds(Vec2i pos) const;
        Tile tileForCell(Vec2i pos) const;

        Cell  _cells[BOARD_H][BOARD_W] = {};
        Vec2i _cursor                  = {BOARD_W / 2, BOARD_H / 2};
        int   _minesLeft               = MINE_COUNT;
        bool  _firstClick              = true;
        bool  _gameOver                = false;
        bool  _won                     = false;
    };

} // namespace arcade

extern "C" arcade::IGameModule *createGame();
extern "C" arcade::LibType      getLibType();
