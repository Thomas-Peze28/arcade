/*
** EPITECH PROJECT, 2026
** include
** File description:
** IDisplay
*/

#ifndef IDisplay_H
#define IDisplay_H

#include <string>

namespace Arcade {
    class IDisplayModule {
        public:
            virtual ~IDisplayModule() = default;
            virtual void init() = 0;
            virtual void stop() = 0;
            virtual const std::string &getName() const = 0;

    };
}
#endif /* IDisplay_H */
