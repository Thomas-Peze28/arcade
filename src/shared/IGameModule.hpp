#pragma once

#include "IArcade.hpp"
#include "IDisplayModule.hpp"

namespace arcade {

    class IGameModule : public IArcade {
    public:
        ~IGameModule() override = default;

        virtual void init(IDisplayModule *display)   = 0;
        virtual void stop()                          = 0;
        virtual bool update(float deltaTime)         = 0;
        virtual void render(IDisplayModule *display) = 0;

        virtual int getScore() const = 0;
    };

} // namespace arcade
