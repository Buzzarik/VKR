#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <string>
#include <filesystem>


enum StatusDirectory{
    IS_DIRECTORY,
    IS_FILE,
    MAX_STATUS
};

class Directory{
public:
    Directory();
    Directory(const std::filesystem::path& path);

    std::filesystem::path get_directory() const;
    uint32_t get_count_files() const;
    std::string get_list_files() const;
    std::vector<std::string> find_files(const std::string& filename) const;
    StatusDirectory get_status() const;
    bool is_directory() const;
    bool is_file() const;

    void set_path(const std::filesystem::path& path);
private:
    uint32_t enclosure_count_files(const std::filesystem::path& path) const;
    void enclosure_path_files(const std::filesystem::path& path, const std::string& filename, std::vector<std::string>& paths) const;
    std::string enclosure_directory(const std::filesystem::path& root, uint32_t enclosure) const;
    std::filesystem::path _path;
    StatusDirectory _status;
};

#endif