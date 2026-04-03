#pragma once

#include "../../abstracts/ADisplayModule.hpp"
#include "../../shared/LibType.hpp"
#include <map>
#include <ncurses.h>
#include <utility>

namespace arcade {

    class NCursesDisplay final : public ADisplayModule {
    public:
        NCursesDisplay()  = default;
        ~NCursesDisplay() override = default;

        std::string getName() const override;

        void open()    override;
        void close()   override;
        void clear()   override;
        void display() override;

        bool pollEvent(Event &event) override;

        void drawTile(Vec2i pos, const Tile &tile)                     override;
        void drawSprite(Vec2i pos, const std::string &spritePath)      override;
        void drawText(const std::string &text, Vec2i pos, Color color) override;

    private:
        int colorPairIndex(Color fg, Color bg);
        int toNcursesColor(Color color) const;

        std::map<std::pair<int,int>, int> _pairCache;
        int _nextPair = 1;
    };

} // namespace arcade

extern "C" arcade::IDisplayModule *createDisplay();
extern "C" arcade::LibType         getLibType();
