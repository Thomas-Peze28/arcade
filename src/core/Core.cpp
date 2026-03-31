#include "Core.hpp"
#include <algorithm>
#include <dlfcn.h>
#include <dirent.h>
#include <stdexcept>
#include <thread>

namespace arcade {

    static constexpr Vec2i MENU_GRID_SIZE         = {40, 25};
    static constexpr auto  TARGET_FRAME_DURATION  = std::chrono::milliseconds(16);

    static bool endsWith(const std::string &str, const std::string &suffix)
    {
        return str.size() >= suffix.size() &&
               str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

    Core::Core(const std::string &initialDisplayPath)
        : _prevTime(Clock::now())
    {
        scanLibraries();

        auto it = std::find_if(_displayLibs.begin(), _displayLibs.end(),
            [&](const LibInfo &li) { return li.path == initialDisplayPath; });

        if (it == _displayLibs.end())
            throw std::runtime_error("Not a valid display library: " + initialDisplayPath);

        _displayIndex = static_cast<size_t>(std::distance(_displayLibs.begin(), it));
        loadDisplay(_displayIndex);
    }

    LibType Core::probeLibType(const std::string &path) const
    {
        void *handle = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
        if (!handle)
            throw std::runtime_error(std::string("dlopen failed: ") + dlerror());

        using GetLibTypeFn = LibType(*)();
        void *sym = dlsym(handle, "getLibType");
        if (!sym) {
            dlclose(handle);
            throw std::runtime_error("Missing getLibType in: " + path);
        }
        LibType type = reinterpret_cast<GetLibTypeFn>(sym)();
        dlclose(handle);
        return type;
    }

    std::string Core::probeLibName(const std::string &path, const std::string &factorySymbol) const
    {
        void *handle = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
        if (!handle)
            return path;

        using FactoryFn = IArcade *(*)();
        void *sym = dlsym(handle, factorySymbol.c_str());
        if (!sym) {
            dlclose(handle);
            return path;
        }
        IArcade *instance = reinterpret_cast<FactoryFn>(sym)();
        std::string name  = instance->getName();
        delete instance;
        dlclose(handle);
        return name;
    }

    void Core::scanLibraries()
    {
        DIR *dir = opendir("./lib");
        if (!dir)
            throw std::runtime_error("Cannot open ./lib directory");

        struct dirent *entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string filename = entry->d_name;
            if (!endsWith(filename, ".so"))
                continue;
            std::string fullPath = "./lib/" + filename;
            try {
                LibType type = probeLibType(fullPath);
                if (type == LibType::GRAPHICAL) {
                    std::string name = probeLibName(fullPath, "createDisplay");
                    _displayLibs.push_back({name, fullPath});
                } else {
                    std::string name = probeLibName(fullPath, "createGame");
                    _gameLibs.push_back({name, fullPath});
                }
            } catch (const std::exception &) {
            }
        }
        closedir(dir);
    }

    void Core::loadDisplay(size_t index)
    {
        if (_displayLoader.isLoaded())
            _displayLoader.get()->close();
        _displayLoader.unload();
        _displayLoader.load(_displayLibs[index].path, "createDisplay");
        _displayLoader.get()->open();
    }

    void Core::loadGame(size_t index)
    {
        _gameLoader.unload();
        _gameLoader.load(_gameLibs[index].path, "createGame");
        _gameLoader.get()->init(_displayLoader.get());
        _gameOverFrames = 0;
        _prevTime       = Clock::now();
    }

    void Core::switchDisplay(int direction)
    {
        int count     = static_cast<int>(_displayLibs.size());
        _displayIndex = static_cast<size_t>((static_cast<int>(_displayIndex) + direction + count) % count);
        loadDisplay(_displayIndex);
        if (_inGame)
            _gameLoader.get()->init(_displayLoader.get());
    }

    void Core::switchGame(int direction)
    {
        int count  = static_cast<int>(_gameLibs.size());
        _gameIndex = static_cast<size_t>((static_cast<int>(_gameIndex) + direction + count) % count);
        loadGame(_gameIndex);
    }

    void Core::runMenu()
    {
        IDisplayModule *display = _displayLoader.get();
        display->setGridSize(MENU_GRID_SIZE);

        Event event;
        while (display->pollEvent(event)) {
            switch (event.key) {
                case KeyCode::ESCAPE: _running = false; return;
                case KeyCode::F1:     switchDisplay(-1); display = _displayLoader.get(); break;
                case KeyCode::F2:     switchDisplay(+1); display = _displayLoader.get(); break;
                case KeyCode::UP:
                    if (_menuCursor > 0) _menuCursor--;
                    break;
                case KeyCode::DOWN:
                    if (_menuCursor < static_cast<int>(_gameLibs.size()) - 1)
                        _menuCursor++;
                    break;
                case KeyCode::ENTER:
                    if (!_gameLibs.empty()) {
                        _gameIndex = static_cast<size_t>(_menuCursor);
                        loadGame(_gameIndex);
                        _inGame = true;
                        return;
                    }
                    break;
                default: break;
            }
        }

        auto frameStart = Clock::now();

        display->clear();

        const Color white  = {255, 255, 255, 255};
        const Color yellow = {255, 220, 0,   255};
        const Color gray   = {150, 150, 150, 255};
        const Color cyan   = {0,   220, 220, 255};

        display->drawText("  ___    ____     ____    _    ____  _____ ", {0, 0}, cyan);
        display->drawText(" / _ \\  / ___|   / ___|  / \\  |  _ \\| ____|", {0, 1}, cyan);
        display->drawText("| | | || |      | |     / _ \\ | | | |  _|  ", {0, 2}, cyan);
        display->drawText("| |_| || |___   | |___ / ___ \\| |_| | |___ ", {0, 3}, cyan);
        display->drawText(" \\___/  \\____|   \\____/_/   \\_\\____/|_____|", {0, 4}, cyan);

        display->drawText("Select a game:", {13, 6}, gray);

        for (int i = 0; i < static_cast<int>(_gameLibs.size()); i++) {
            bool      selected = (i == _menuCursor);
            Color     col      = selected ? yellow : white;
            std::string line   = (selected ? "> " : "  ") + _gameLibs[i].name;
            display->drawText(line, {12, 8 + i}, col);
        }

        if (_gameLibs.empty())
            display->drawText("No games found in ./lib/", {8, 8}, gray);

        display->drawText("UP/DOWN  navigate      ENTER  launch", {3, 20}, gray);
        display->drawText("F1/F2    switch display  ESC   quit",  {3, 21}, gray);
        display->drawText("Display: " + _displayLibs[_displayIndex].name, {3, 23}, gray);

        display->display();

        auto elapsed = Clock::now() - frameStart;
        if (elapsed < TARGET_FRAME_DURATION)
            std::this_thread::sleep_for(TARGET_FRAME_DURATION - elapsed);
    }

    void Core::runGame()
    {
        IDisplayModule *display = _displayLoader.get();
        IGameModule    *game    = _gameLoader.get();

        auto now = Clock::now();
        float dt = std::chrono::duration<float>(now - _prevTime).count();
        _prevTime = now;

        auto frameStart = now;

        std::vector<Event> gameEvents;
        Event event;
        while (display->pollEvent(event)) {
            switch (event.key) {
                case KeyCode::ESCAPE: _running = false;                     return;
                case KeyCode::M:      _gameLoader.unload(); _inGame = false; return;
                case KeyCode::R:      game->stop(); game->init(display);      break;
                case KeyCode::F1:     switchDisplay(-1);
                                      display = _displayLoader.get();
                                      game    = _gameLoader.get();            break;
                case KeyCode::F2:     switchDisplay(+1);
                                      display = _displayLoader.get();
                                      game    = _gameLoader.get();            break;
                case KeyCode::F3:     switchGame(-1);
                                      game    = _gameLoader.get();            break;
                case KeyCode::F4:     switchGame(+1);
                                      game    = _gameLoader.get();            break;
                default:              gameEvents.push_back(event);            break;
            }
        }

        bool keepRunning = game->update(dt, gameEvents);

        display->clear();
        game->render(display);
        display->drawText("F1/F2:display  F3/F4:game  R:restart  M:menu  ESC:quit",
                          {0, 0}, {100, 100, 100, 255});
        display->display();

        if (!keepRunning) {
            _gameOverFrames++;
            if (_gameOverFrames >= GAME_OVER_LINGER_FRAMES) {
                _gameLoader.unload();
                _inGame = false;
            }
        }

        auto elapsed = Clock::now() - frameStart;
        if (elapsed < TARGET_FRAME_DURATION)
            std::this_thread::sleep_for(TARGET_FRAME_DURATION - elapsed);
    }

    void Core::run()
    {
        while (_running) {
            if (_inGame)
                runGame();
            else
                runMenu();
        }
        if (_displayLoader.isLoaded())
            _displayLoader.get()->close();
    }

} // namespace arcade
