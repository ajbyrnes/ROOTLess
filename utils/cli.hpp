
/**
 * @file cli.hpp
 * @brief Declarations for command-line argument parsing and argument struct.
 */
#pragma once

#include <map>
#include <string>
#include <vector>

/**
 * @struct Args
 * @brief Structure to hold parsed command-line arguments and their default values.
 */
struct Args {
    std::string dataFile{};
    std::string treename{};
    std::vector<std::string> branches{};

    size_t chunkSize{};
    std::string compressor{};
    std::map<std::string, std::string> compressionOptions{};

    std::string resultsFile{};
    
    bool writeDecompressed{false};
    std::string decompFile{};
};

std::vector<std::string> tokenize(const std::string& str, char delimiter);

std::map<std::string, std::string> parseBitTruncationOptions(std::vector<std::string> optionsList);
std::map<std::string, std::string> parseSZ3Options(std::vector<std::string> optionsList);

/**
 * @brief Parse command-line arguments into an Args struct.
 * @param argc Number of command-line arguments.
 * @param argv Array of argument strings.
 * @return Args struct populated with parsed values.
 */
const Args parseArgs(int argc, char* argv[]);

void usage();
void printArgs(const Args& args);