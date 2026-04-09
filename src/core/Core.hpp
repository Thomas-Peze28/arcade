#pragma once

#include "DLLoader.hpp"
#include "../shared/IDisplayModule.hpp"
#include "../shared/IGameModule.hpp"
#include "../shared/LibType.hpp"
#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace arcade {

    struct LibInfo {
        std::string name;
        std::string path;
    };

    class Core {
    public:
        explicit Core(const std::string &initialDisplayPath);
        ~Core() = default;

        Core(const Core &) = delete;
        Core &operator=(const Core &) = delete;

        void run();

    private:
        void        scanLibraries();
        LibType     probeLibType(const std::string &path) const;
        std::string probeLibName(const std::string &path, const std::string &factorySymbol) const;

        void        loadDisplay(size_t index);
        void        loadGame(size_t index);
        void        switchDisplay(int direction);
        void        switchGame(int direction);
        IGameModule *activeGame() const;

        void runMenu();
        void runGame();

        DLLoader<IDisplayModule>                                    _displayLoader;
        std::map<size_t, std::unique_ptr<DLLoader<IGameModule>>>    _gameSlots;

        std::vector<LibInfo> _displayLibs;
        std::vector<LibInfo> _gameLibs;
        size_t _displayIndex = 0;
        size_t _gameIndex    = 0;

        bool _running    = true;
        bool _inGame     = false;
        int  _menuCursor = 0;

        using Clock     = std::chrono::steady_clock;
        using TimePoint = std::chrono::time_point<Clock>;
        TimePoint _prevTime;

        // Frames to linger on screen after game over before returning to menu.
        static constexpr int GAME_OVER_LINGER_FRAMES = 90;
        int _gameOverFrames = 0;
    };

} // namespace arcade
