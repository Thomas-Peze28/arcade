#include "Core.hpp"
#include <exception>
#include <iostream>

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cerr << "Usage: ./arcade <path_to_graphics_lib>" << std::endl;
        return 84;
    }
    try {
        arcade::Core core(argv[1]);
        core.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 84;
    }
    return 0;
}
