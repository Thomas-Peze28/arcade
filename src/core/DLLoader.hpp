#pragma once

#include <dlfcn.h>
#include <stdexcept>
#include <string>

namespace arcade {

    template <typename T>
    class DLLoader {
    public:
        using FactoryFn = T *(*)();

        DLLoader()  = default;
        ~DLLoader() { unload(); }

        DLLoader(const DLLoader &) = delete;
        DLLoader &operator=(const DLLoader &) = delete;

        void load(const std::string &path, const std::string &symbol)
        {
            unload();
            _handle = dlopen(path.c_str(), RTLD_NOW);
            if (!_handle)
                throw std::runtime_error(std::string("dlopen failed: ") + dlerror());
            void *sym = dlsym(_handle, symbol.c_str());
            if (!sym) {
                dlclose(_handle);
                _handle = nullptr;
                throw std::runtime_error(std::string("dlsym failed: ") + dlerror());
            }
            _instance = reinterpret_cast<FactoryFn>(sym)();
        }

        void unload()
        {
            delete _instance;
            _instance = nullptr;
            if (_handle) {
                dlclose(_handle);
                _handle = nullptr;
            }
        }

        bool isLoaded() const { return _instance != nullptr; }
        T   *get()      const { return _instance; }

    private:
        void *_handle   = nullptr;
        T    *_instance = nullptr;
    };

} // namespace arcade
