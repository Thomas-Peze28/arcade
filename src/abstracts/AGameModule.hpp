#pragma once

#include "../shared/IGameModule.hpp"

namespace arcade {

    class AGameModule : public IGameModule {
    public:
        ~AGameModule() override = default;

        AGameModule(const AGameModule &) = delete;
        AGameModule &operator=(const AGameModule &) = delete;

        void init(IDisplayModule *display)                                override = 0;
        void stop()                                                       override;
        bool update(float deltaTime, const std::vector<Event> &events)   override = 0;
        void render(IDisplayModule *display)                              override = 0;

        int getScore() const override;

    protected:
        AGameModule() = default;

        IDisplayModule *_display = nullptr;
        int             _score   = 0;
        bool            _running = false;
    };

} // namespace arcade
