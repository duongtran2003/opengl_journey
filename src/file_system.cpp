#include "learn_opengl/file_system.hpp"
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

FileSystem& FileSystem::get_instance()
{
    static FileSystem instance;
    return instance;
}

void FileSystem::set_root_marker(const std::string p_root_marker)
{
    root_marker = p_root_marker;
    find_root();
}

FileSystem::FileSystem()
{
    find_root();
}

void FileSystem::find_root()
{
    std::filesystem::path current = std::filesystem::current_path();
    while (!current.empty() && current.has_relative_path())
    {
        if (std::filesystem::exists(current / root_marker))
        {
            project_root = current;
            std::cout << project_root << std::endl;
            return;
        }

        current = current.parent_path();
    }

    throw std::runtime_error("Could not find project root (missing " + root_marker + ")");
}

std::filesystem::path FileSystem::get_root_path() const
{
    return project_root;
}

std::filesystem::path FileSystem::get_path(const std::string path) const
{
    std::filesystem::path p(path);

    return project_root / p;
}
