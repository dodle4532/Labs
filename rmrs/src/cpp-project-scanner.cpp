#include <filesystem>
#include <iostream>
#include <format>
#include <optional>
#include <vector>
#include <algorithm>
#include <ranges>
#include <fstream>

namespace fs = std::filesystem;

struct Dir {
    fs::path path;
    size_t line_count;
    size_t file_size;
};

std::vector<std::string> cpp_extensions = {
        ".cpp", ".cc", ".cxx", ".h", ".hh", ".hpp", ".hxx"
    };

bool is_cpp_file(const fs::path& path) {
    if (!fs::is_regular_file(path)) return false;
    std::string extension = path.extension().string();
    for (const auto& ext : cpp_extensions) {
        if (extension == ext) return true;
    }
    return false;
}

std::optional<Dir> analyze_file(const fs::path& path) {
    if (!is_cpp_file(path)) return std::nullopt;
    Dir stats;
    stats.path = path;
    stats.file_size = fs::file_size(path);
    std::ifstream file(path);
    if (!file.is_open()) return std::nullopt;
    stats.line_count = std::count(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>(),
        '\n'
    );
    return stats;
}

void print_statistics(const std::vector<Dir>& stats,
                            size_t total_files, size_t total_lines, 
                            size_t total_size) {
    if (stats.empty()) {
        std::cout << "Total C++ files: 0\nTotal lines of code: 0\nTotal size: 0 bytes";
        return;
    }
    auto largest_file = std::ranges::max_element(stats, {}, &Dir::file_size);
    auto longest_file = std::ranges::max_element(stats, {}, &Dir::line_count);
    std::cout << std::format("Total C++ files: {}\n", total_files);
    std::cout << std::format("Total lines of code: {}\n", total_lines);
    std::cout << std::format("Total size: {} bytes\n", total_size);
    if (largest_file != stats.end()) {
        std::cout << std::format("Largest file: {} ({} bytes)\n", 
                                largest_file->path.filename().string(),
                                largest_file->file_size);
    }
    if (longest_file != stats.end()) {
        std::cout << std::format("Longest file: {} ({} lines)\n",
                                longest_file->path.filename().string(), 
                                longest_file->line_count);
    }
}

void analyze_directory(const fs::path& directory) {
    std::vector<Dir> all_stats;
    size_t total_files = 0;
    size_t total_lines = 0;
    size_t total_size = 0;
    for (const auto& entry : fs::recursive_directory_iterator(
        directory, fs::directory_options::skip_permission_denied)) {
        if (auto stats = analyze_file(entry.path()); stats) {
            all_stats.push_back(*stats);
            total_files++;
            total_lines += stats->line_count;
            total_size += stats->file_size;
        }
    }

    print_statistics(all_stats, total_files, total_lines, total_size);
}


int main(int argc, char* argv[]) {
    fs::path directory;
    if (argc == 1) {
        directory = fs::current_path();
    }
    else {
        directory = argv[1];
    }
    
    if (!fs::exists(directory)) {
        std::cerr << std::format("Directory {} does not exist\n", directory.string());
        return 1;
    }

    if (!fs::is_directory(directory)) {
        std::cerr << std::format("{} is not a directory\n", directory.string());
        return 1;
    }

    analyze_directory(directory);
    return 0;
}
