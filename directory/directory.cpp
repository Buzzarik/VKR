#include "directory.h"
#include <algorithm>
#include <ranges>
//====================== Directory =======================
//                      Constructor
Directory::Directory()
:_status(StatusDirectory::MAX_STATUS){}

Directory::Directory(const std::filesystem::path &path)
{
    set_path(path);
}
//                        Getters
std::filesystem::path Directory::get_directory() const{
    return _path.string();
}

uint32_t Directory::enclosure_count_files(const std::filesystem::path &path) const{
    uint32_t count = 0;
    if (std::filesystem::is_directory(path)) {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_directory()) {
                count += enclosure_count_files(entry.path());
            }
            else {
                if (entry.path().filename().string()[0] != '.') {
                    count++;
                }
            }
        }
    }
    return count;
}

std::string Directory::enclosure_directory(const std::filesystem::path &path, uint32_t enclosure) const{
    std::string files;
    Directory root(path);
    if (!root.is_directory())
        return "";
    if (std::filesystem::is_directory(path)) {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_directory()) {
                files += std::string(enclosure, ' ') + entry.path().filename().string() + "\n";
                files += enclosure_directory(entry.path(), enclosure + 1);
            }
            else {
                if (entry.path().filename().string()[0] != '.') {
                    files += std::string(enclosure, ' ') + entry.path().filename().string() + "\n";
                }
            }
        }
    }
    return files;
}

uint32_t Directory::get_count_files() const{
    if (!is_directory() && !is_file())
        return 0;
    if (is_file())
        return 1;
    uint32_t count = 0;

    for (const auto& entry : std::filesystem::directory_iterator(_path)) {
        if (entry.is_directory()) {
            count += enclosure_count_files(entry.path());
        }
        else {
            if (entry.path().filename().string()[0] != '.') {
                count++;
            }
        }
    }
    return count;
}

std::string Directory::get_list_files() const{
    if (is_file()){
        return _path.filename().string();
    }
    if (!is_directory())
        return "";
    std::string files;
    for (const auto& entry : std::filesystem::directory_iterator(_path)) {
        if (entry.is_directory()) {
            files += entry.path().filename().string() + "\n";
            files += enclosure_directory(entry.path(), 1);
        }
        else {
            if (entry.path().filename().string()[0] != '.') {
                files += entry.path().filename().string() + "\n";
            }
        }
    }
    return files;
}

std::vector<std::string> Directory::find_files(const std::string& filename) const{
    if ((is_directory() || is_file()) && _path.filename().string() == filename)
        return {_path.filename().string()};

    std::vector<std::string> paths;
    if (is_directory()) {
        for (const auto& entry : std::filesystem::directory_iterator(_path)) {
            if (entry.path().filename().string() == filename) {
                paths.push_back(entry.path().string());
            }
            if (entry.is_directory()) {
                enclosure_path_files(entry.path(), filename, paths);
            }
        }
    }
    return paths;
}

bool Directory::is_directory() const{
    return _status == StatusDirectory::IS_DIRECTORY;
}

bool Directory::is_file() const{
    return _status == StatusDirectory::IS_FILE;
}

StatusDirectory Directory::get_status() const{
    return _status;
}
//                        Setters
void Directory::enclosure_path_files(const std::filesystem::path &path, const std::string& filename, std::vector<std::string>& paths) const{
    if (std::filesystem::is_directory(path)) {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.path().filename().string() == filename) {
                paths.push_back(entry.path().string());
            }
            if (entry.is_directory()) {
                enclosure_path_files(entry.path(), filename, paths);
            }
        }
    }
}

void Directory::set_path(const std::filesystem::path &path){
    _path = path;
    if (std::filesystem::exists(_path)){
        if (std::filesystem::is_directory(_path)){
            _status = StatusDirectory::IS_DIRECTORY;
        }
        else{
            _status = StatusDirectory::IS_FILE;
        }
    }
    else{
        _status = StatusDirectory::MAX_STATUS;
    }
}