#include "AGameModule.hpp"

namespace arcade {

    void AGameModule::stop()
    {
        _running = false;
    }

    int AGameModule::getScore() const
    {
        return _score;
    }

} // namespace arcade
