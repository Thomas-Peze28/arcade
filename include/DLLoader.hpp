/*
** EPITECH PROJECT, 2026
** Project
** File description:
** DLLoader
*/

#ifndef DLLOADER_HPP_
#define DLLOADER_HPP_

#include <dlfcn.h>
#include <iostream>
#include <memory>
#include <string>

template <typename T>
class DLLoader {
    public:
        DLLoader(const std::string &filepath) : handle(nullptr) {
            handle = dlopen(filepath.c_str(), RTLD_LAZY);
            if (!handle) {
                std::cerr << "Cannot open library: " << dlerror() << '\n';
            }
        }

        ~DLLoader() {
            if (handle) {
                dlclose(handle);
            }
        }

        T *getInstance(const std::string &entryPointName) {
            if (!handle) {
                return nullptr;
            }
            void *sym = dlsym(handle, entryPointName.c_str());
            if (!sym) {
                std::cerr << "Cannot load symbol '" << entryPointName << "': " << dlerror() << '\n';
                return nullptr;
            }
            T *(*entryPointFn)() = reinterpret_cast<T *(*)()>(sym);
            return entryPointFn();
        }

        bool isOpen() const {
            return handle != nullptr;
        }

        std::string getLibraryType() {
            if (!handle)
                return "unknown";
            
            void *function = dlsym(handle, "getType");
            if (!function) {
                return "unknown"; 
            }
            
            const char *(*type)() = reinterpret_cast<const char *(*)()>(function);
            return std::string(type());
        }

    private:
        void *handle;
};

#endif /* !DLLOADER_HPP_ */