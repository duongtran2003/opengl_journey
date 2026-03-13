#pragma once

#include <string>
#include <filesystem>

class FileSystem
{
  public:
    static FileSystem&    get_instance();
    std::filesystem::path get_root_path() const;
    std::filesystem::path get_path(const std::string path) const;
    void                  set_root_marker(const std::string root_marker);

  private:
    FileSystem();
    void find_root();

    std::filesystem::path project_root;
    std::string           root_marker;
};
