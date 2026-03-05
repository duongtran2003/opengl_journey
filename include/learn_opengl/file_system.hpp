#pragma once

#include <string>
class FileSystem
{
  public:
    static FileSystem& get_instance();

    void        set_root(const std::string& path);
    std::string get_absolute_path(const std::string& path);

  private:
    FileSystem();
    std::string root;
};
