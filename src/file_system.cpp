#include "learn_opengl/file_system.hpp"
#include <filesystem>
#include <iostream>
#include <string>

FileSystem& FileSystem::get_instance()
{
    static FileSystem instance;
    return instance;
}
