
/**
 * @file cli.cpp
 * @brief Command-line argument parsing implementation for the utils module.
 *
 * Provides a function to parse command-line arguments into an Args struct.
 */
#include <iostream>
#include <stdexcept>

#include "cli.hpp"

std::vector<std::string> tokenize(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    while (start < str.size()) {
        size_t pos = str.find(delimiter, start);
        if (pos == std::string::npos) {
            tokens.push_back(str.substr(start));
            break;
        } else {
            tokens.push_back(str.substr(start, pos - start));
            start = pos + 1;
        }
    }
    return tokens;
}

std::map<std::string, std::string> parseBitTruncationOptions(std::vector<std::string> optionsList) {
    // BitTruncation takes two options
    // 1: Number of mantissa bits to keep
    // 2: Compression level for zlib
    if (optionsList.size() != 3) {
        throw std::runtime_error("BitTruncation requires exactly two options: mantissaBits and compressionLevel");
    }

    std::map<std::string, std::string> optionsMap;
    optionsMap["mantissaBits"] = optionsList[1];
    optionsMap["compressionLevel"] = optionsList[2];

    return optionsMap;
}

std::map<std::string, std::string> parseSZ3Options(std::vector<std::string> optionsList) {
    // SZ3 takes three options
    // 1: Algorithm (0=interp+lorenzo, 1=interp+regression, 2=lorenzo only, 3=regression only)
    // 2: Error bound mode (0=absolute, 1=relative)
    // 3: Error bound value
    if (optionsList.size() != 4) {
        throw std::runtime_error("SZ3 requires exactly three options: algorithm, errorBoundMode, errorBoundValue");
    }

    std::map<std::string, std::string> optionsMap;
    optionsMap["algorithm"] = optionsList[1];
    optionsMap["errorBoundMode"] = optionsList[2];
    optionsMap["errorBoundValue"] = optionsList[3];

    return optionsMap;
}

/**
 * @brief Parse command-line arguments into an Args struct.
 * @param argc Number of command-line arguments.
 * @param argv Array of argument strings.
 * @return Args struct populated with parsed values.
 * @throws std::runtime_error if required values are missing or unknown arguments are provided.
 */
const Args parseArgs(int argc, char* argv[]) {
    Args args;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--dataFile" && i + 1 < argc) {
            args.dataFile = argv[++i];
        } else if (arg == "--tree" && i + 1 < argc) {
            args.treename = argv[++i];
        } else if (arg == "--branches" && i + 1 < argc) {
            // Comma-separated list of branches
            // i.e. --branches branch1,branch2,branch3
            std::string branchesList = argv[++i];
            std::vector<std::string> branches = tokenize(branchesList, ',');
            args.branches = branches;
        } else if (arg == "--chunkSize" && i + 1 < argc) {
            args.chunkSize = std::stoul(argv[++i]);
        } else if (arg == "--compressor" && i + 1 < argc) {
            // Comma-separated compressor name and options
            // i.e. --compressor BitTruncation,12,1
            std::vector<std::string> compressorList = tokenize(argv[++i], ',');
            args.compressor = compressorList[0];
                
            if (args.compressor == "BitTruncation") {
                args.compressionOptions = parseBitTruncationOptions(compressorList);
            } else if (args.compressor == "SZ3") {
                args.compressionOptions = parseSZ3Options(compressorList);
            }else {
                throw std::runtime_error("Unsupported compressor: " + args.compressor);
            }
        } else if (arg == "--resultsFile" && i + 1 < argc) {
            args.resultsFile = argv[++i];
        } else if (arg == "--writeDecompressed" && i + 1 < argc) {
            args.writeDecompressed = true;
            args.decompFile = argv[++i];
        } else {
            throw std::runtime_error("Unknown or incomplete argument: " + arg);
        }
    }

    // Check usage
    if (args.dataFile.empty() || args.treename.empty() || 
        args.branches.empty() || args.chunkSize == 0 || args.compressor.empty() ||
        args.resultsFile.empty()) 
    {
        usage();
        exit(1);
    }

    return args;
}

void usage() {
    std::cout << "Usage: program "
                "--dataFile <file> "
                "--tree <name> "
                "--branches <branch1,branch2,...> "
                "--chunkSize <number> "
                "--compressor <name,option1,option2,...> "
                "--resultsFile <file> "
                "[--writeDecompressed <file>]"
                "\n";
    std::cout << "Example: program "
                "--dataFile data.root "
                "--tree CollectionTree "
                "--branches AnalysisJetsAuxDyn.pt,AnalysisJetsAuxDyn.eta "
                "--chunkSize 1024 "
                "--compressor BitTruncation,12,1\n";
    std::cout << "Supported compressors:\n";
    std::cout << "  --compressor BitTruncation,<mantissaBits>,<compressionLevel>\n";
    std::cout << "    where <mantissaBits>: number of mantissa bits to keep (0-23 for float)\n";
    std::cout << "          <compressionLevel>: zlib compression level (0-9)\n";
    std::cout << "  --compressor SZ3,<algorithm>,<errorBoundMode>,<errorBoundValue>\n";
    std::cout << "    where <algorithm>: 0=interp+lorenzo, 1=interp+regression, 2=lorenzo only, 3=regression only\n";
    std::cout << "          <errorBoundMode>: 0=absolute, 1=relative\n";
    std::cout << "          <errorBoundValue>: float\n";
}

void printArgs(const Args& args) {
    std::cout << "---------- Command-Line Arguments ----------" << std::endl;
    std::cout << "Data file: " << args.dataFile << std::endl;
    std::cout << "Tree name: " << args.treename << std::endl;
    std::cout << "Branches: " << std::endl;
    for (const auto& branch : args.branches) {
        std::cout << "\t" << branch << std::endl;
    }

    std::cout << "Chunk size: " << args.chunkSize << std::endl;

    std::cout << "Compressor: " << args.compressor << std::endl;
    std::cout << "Compression options: " << std::endl;
    for (const auto& [key, value] : args.compressionOptions) {
        std::cout << "\t" << key << ": " << value << std::endl;
    }   

    std::cout << "Results will be written to: " << args.resultsFile << std::endl;

    if (args.writeDecompressed) {
        std::cout << "Decompressed data will be written to: " << args.decompFile << std::endl;
    } else {
        std::cout << "Decompressed data will NOT be written to file." << std::endl;
    }

    std::cout << "--------------------------------------------" << std::endl;
}