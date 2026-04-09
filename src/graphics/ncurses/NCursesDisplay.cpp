#include "NCursesDisplay.hpp"
#include <climits>

namespace arcade {

    static constexpr int NCURSES_COLOR_COUNT = 8;

    static const int NCURSES_COLORS[NCURSES_COLOR_COUNT] = {
        COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_YELLOW,
        COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN, COLOR_WHITE
    };

    static const Color COLOR_REFERENCES[NCURSES_COLOR_COUNT] = {
        {0,   0,   0,   255},
        {255, 0,   0,   255},
        {0,   255, 0,   255},
        {255, 255, 0,   255},
        {0,   0,   255, 255},
        {255, 0,   255, 255},
        {0,   255, 255, 255},
        {255, 255, 255, 255},
    };

    std::string NCursesDisplay::getName() const
    {
        return "NCurses";
    }

    void NCursesDisplay::open()
    {
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        nodelay(stdscr, TRUE);
        curs_set(0);
        mousemask(ALL_MOUSE_EVENTS, nullptr);
        start_color();
        use_default_colors();
        _pairCache.clear();
        _nextPair = 1;
        _isOpen   = true;
    }

    void NCursesDisplay::close()
    {
        endwin();
        _isOpen = false;
    }

    void NCursesDisplay::clear()
    {
        erase();
    }

    void NCursesDisplay::display()
    {
        wnoutrefresh(stdscr);
        doupdate();
    }

    int NCursesDisplay::toNcursesColor(Color color) const
    {
        int bestIndex = 0;
        int bestDist  = INT_MAX;

        for (int i = 0; i < NCURSES_COLOR_COUNT; i++) {
            int dr   = static_cast<int>(color.r) - static_cast<int>(COLOR_REFERENCES[i].r);
            int dg   = static_cast<int>(color.g) - static_cast<int>(COLOR_REFERENCES[i].g);
            int db   = static_cast<int>(color.b) - static_cast<int>(COLOR_REFERENCES[i].b);
            int dist = dr * dr + dg * dg + db * db;
            if (dist < bestDist) {
                bestDist  = dist;
                bestIndex = i;
            }
        }
        return NCURSES_COLORS[bestIndex];
    }

    int NCursesDisplay::colorPairIndex(Color fg, Color bg)
    {
        int ncFg = toNcursesColor(fg);
        int ncBg = toNcursesColor(bg);
        auto key = std::make_pair(ncFg, ncBg);

        auto it = _pairCache.find(key);
        if (it != _pairCache.end())
            return it->second;

        init_pair(_nextPair, ncFg, ncBg);
        _pairCache[key] = _nextPair;
        return _nextPair++;
    }

    bool NCursesDisplay::pollEvent(Event &event)
    {
        int ch = getch();
        if (ch == ERR)
            return false;

        if (ch == KEY_MOUSE) {
            MEVENT me;
            if (getmouse(&me) != OK)
                return false;
            event.type     = EventType::MOUSE;
            event.mousePos = {me.x / 2, me.y};
            if (me.bstate & BUTTON1_PRESSED)
                event.mouseButton = MouseButton::LEFT;
            else if (me.bstate & BUTTON3_PRESSED)
                event.mouseButton = MouseButton::RIGHT;
            else
                return false;
            return true;
        }

        event.type = EventType::KEYBOARD;
        switch (ch) {
            case KEY_UP:    event.key = KeyCode::UP;     break;
            case KEY_DOWN:  event.key = KeyCode::DOWN;   break;
            case KEY_LEFT:  event.key = KeyCode::LEFT;   break;
            case KEY_RIGHT: event.key = KeyCode::RIGHT;  break;
            case 27:        event.key = KeyCode::ESCAPE; break;
            case '\n':
            case KEY_ENTER: event.key = KeyCode::ENTER;  break;
            case ' ':       event.key = KeyCode::SPACE;  break;
            case KEY_F(1):  event.key = KeyCode::F1;     break;
            case KEY_F(2):  event.key = KeyCode::F2;     break;
            case KEY_F(3):  event.key = KeyCode::F3;     break;
            case KEY_F(4):  event.key = KeyCode::F4;     break;
            case 'r': case 'R': event.key = KeyCode::R;  break;
            case 'm': case 'M': event.key = KeyCode::M;  break;
            default:        event.key = KeyCode::UNKNOWN; break;
        }
        return true;
    }

    void NCursesDisplay::drawTile(Vec2i pos, const Tile &tile)
    {
        int pair = colorPairIndex(tile.fg, tile.bg);
        attron(COLOR_PAIR(pair));
        mvaddch(pos.y, pos.x * 2,     tile.symbol);
        mvaddch(pos.y, pos.x * 2 + 1, ' ');
        attroff(COLOR_PAIR(pair));
    }

    void NCursesDisplay::drawSprite(Vec2i, const std::string &)
    {
    }

    void NCursesDisplay::drawText(const std::string &text, Vec2i pos, Color color)
    {
        int pair = colorPairIndex(color, {0, 0, 0, 255});
        attron(COLOR_PAIR(pair));
        mvprintw(pos.y, pos.x, "%s", text.c_str());
        attroff(COLOR_PAIR(pair));
    }

} // namespace arcade

arcade::IDisplayModule *createDisplay()
{
    return new arcade::NCursesDisplay();
}

arcade::LibType getLibType()
{
    return arcade::LibType::GRAPHICAL;
}
