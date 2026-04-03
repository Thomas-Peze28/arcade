#include "ADisplayModule.hpp"

namespace arcade {

    void ADisplayModule::setGridSize(Vec2i size)
    {
        _gridSize = size;
    }

    Vec2i ADisplayModule::getGridSize() const
    {
        return _gridSize;
    }

} // namespace arcade
