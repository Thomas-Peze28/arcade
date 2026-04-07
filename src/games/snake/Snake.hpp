#pragma once

#include "../../abstracts/AGameModule.hpp"
#include "../../shared/LibType.hpp"
#include <deque>

namespace arcade {

    static constexpr int   GRID_W     = 20;
    static constexpr int   GRID_H     = 15;
    static constexpr int   HUD_H      = 2;
    static constexpr int   BORDER     = 1;
    static constexpr float TICK_BASE  = 0.15f;
    static constexpr float TICK_MIN   = 0.06f;

    enum class Direction { UP, DOWN, LEFT, RIGHT };

    class Snake final : public AGameModule {
    public:
        Snake()  = default;
        ~Snake() override = default;

        std::string getName() const override;

        void init(IDisplayModule *display)                                   override;
        bool update(float deltaTime, const std::vector<Event> &events)       override;
        void render(IDisplayModule *display)                                  override;

    private:
        void      spawnFood();
        void      step();
        bool      isOnSnake(Vec2i pos) const;
        void      queueDirection(Direction d);
        float     tickInterval() const;

        std::deque<Vec2i> _body;
        Direction         _dir        = Direction::RIGHT;
        Direction         _pendingDir = Direction::RIGHT;
        Vec2i             _food       = {0, 0};
        float             _accum      = 0.f;
        bool              _paused     = false;
        bool              _gameOver   = false;
    };

} // namespace arcade

extern "C" arcade::IGameModule *createGame();
extern "C" arcade::LibType      getLibType();
