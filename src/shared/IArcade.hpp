#pragma once

#include <string>

namespace arcade {

    class IArcade {
    public:
        virtual ~IArcade() = default;

        virtual std::string getName() const = 0;
    };

} // namespace arcade
