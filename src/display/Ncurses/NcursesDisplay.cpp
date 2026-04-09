/*
** EPITECH PROJECT, 2026
** Arcade
** File description:
** NcursesDisplay
*/

#include "NcursesDisplay.hpp"

namespace Arcade {

    NcursesDisplay::NcursesDisplay() : _name("ncurses")
    {
    }

    NcursesDisplay::~NcursesDisplay()
    {
    }

    void NcursesDisplay::init()
    {
        initscr();
        noecho();
        cbreak();
        keypad(stdscr, TRUE);
        curs_set(0);
        nodelay(stdscr, TRUE);
        
        if (has_colors()) {
            start_color();
            init_pair(1, COLOR_RED, COLOR_BLACK);
            init_pair(2, COLOR_GREEN, COLOR_BLACK);
            init_pair(3, COLOR_BLUE, COLOR_BLACK);
            init_pair(4, COLOR_WHITE, COLOR_BLACK);
            init_pair(5, COLOR_BLACK, COLOR_BLACK);
        }
    }

    void NcursesDisplay::stop()
    {
        endwin();
    }

    const std::string &NcursesDisplay::getName() const
    {
        return _name;
    }

    void NcursesDisplay::clear()
    {
        erase();
    }

    void NcursesDisplay::display()
    {
        refresh();
    }

    int NcursesDisplay::translateColor(const Arcade::Color &color)
    {
        switch (color) {
            case Color::RED:
                return 1;
            case Color::GREEN:
                return 2;
            case Color::BLUE:
                return 3;
            case Color::WHITE:
                return 4;
            case Color::BLACK:
                return 5;
            default:
                return 4;
        }
    }

    void NcursesDisplay::drawObject(const Arcade::Object &object)
    {
        attron(COLOR_PAIR(translateColor(object.color)));
        mvaddch(object.pos.y, object.pos.x, object.character);
        attroff(COLOR_PAIR(translateColor(object.color)));
    }

    void NcursesDisplay::drawText(const std::string &text, const Arcade::Position &pos, const Arcade::Color &color)
    {
        attron(COLOR_PAIR(translateColor(color)));
        mvprintw(pos.y, pos.x, "%s", text.c_str());
        attroff(COLOR_PAIR(translateColor(color)));
    }

    Input NcursesDisplay::getInput()
    {
        int ch = getch();

        switch (ch) {
            case KEY_UP:
                return Input::UP;
            case KEY_DOWN:
                return Input::DOWN;
            case KEY_LEFT:
                return Input::LEFT;
            case KEY_RIGHT:
                return Input::RIGHT;
            case 'z': case 'Z':
                return Input::Z;
            case 'q': case 'Q':
                return Input::Q;
            case 's': case 'S':
                return Input::S;
            case 'd': case 'D':
                return Input::D;
            case ' ':
                return Input::SPACE;
            case '\n':
                return Input::ACTION;
            case 'r': case 'R':
                return Input::RESTART;
            case 'm': case 'M':
                return Input::MENU;
            case 27:
                return Input::EXIT;
            case ERR:
                return Input::NONE;
            case KEY_F(1):
                return Input::NEXT_GRAPHIC;
            case KEY_F(2):
                return Input::PREV_GRAPHIC;
            case KEY_F(3):
                return Input::NEXT_GAME;
            case KEY_F(4):
                return Input::PREV_GAME;
            default:
                return Input::NONE;
        }
    }

}

extern "C" {
    Arcade::IDisplay *entryPoint()
    {
        return new Arcade::NcursesDisplay();
    }

    const char *getType() {
        return "display";
    }
}