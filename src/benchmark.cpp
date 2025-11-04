#include <format>
#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "CompressorBenchmark.hpp"
#include "../utils/utils.hpp"
#include "../utils/root.hpp"
#include "../utils/cli.hpp"

void writeJSON(const Args& args, const std::string& branch, const BenchmarkResult& result) {
    std::cout << timeMessage(std::format("Writing results to {}", args.resultsFile)) << std::endl;

    // Create JSON object
    nlohmann::json newRecord;

    // Save file-safe timestamp
    newRecord["timestamp"] = getTimestamp(true);

    // Save host info
    newRecord["host"] = getHost();

    // Save args
    newRecord["args"]["dataFile"] = tokenize(args.dataFile, '/').back();
    newRecord["args"]["treename"] = args.treename;
    newRecord["args"]["branch"] = branch;
    newRecord["args"]["chunkSize"] = args.chunkSize;
    newRecord["args"]["compressor"] = args.compressor;
    newRecord["args"]["compressionOptions"] = args.compressionOptions;
    newRecord["args"]["writeDecompressed"] = args.writeDecompressed;
    newRecord["args"]["decompFile"] = args.decompFile;

    // Save benchmark results
    newRecord["results"]["compressionThroughputMBps"] = result.compressionThroughputMBps;
    newRecord["results"]["decompressionThroughputMBps"] = result.decompressionThroughputMBps;
    newRecord["results"]["compressionRatio"] = result.compressionRatio;
    newRecord["results"]["MSE"] = result.MSE; 
    newRecord["results"]["PSNR"] = result.PSNR;
    newRecord["results"]["meanRelError"] = result.meanRelError;
    newRecord["results"]["maxRelError"] = result.maxRelError;
    newRecord["results"]["meanAbsError"] = result.meanAbsError;
    newRecord["results"]["maxAbsError"] = result.maxAbsError;

    // Load existing records
    nlohmann::json allRecords;
    std::ifstream inFile(args.resultsFile);
    if (inFile) {
        inFile >> allRecords;
        if (!allRecords.is_array()) {
            allRecords = nlohmann::json::array();
        }
    } else {
        allRecords = nlohmann::json::array();
    }
    inFile.close();

    // Append new record
    allRecords.push_back(newRecord);

    // Write all records back to file
    std::ofstream outFile(args.resultsFile);
    outFile << std::setw(4) << allRecords << std::endl;
    outFile.close();
}

int main(int argc, char* argv[]) {
    Args args = parseArgs(argc, argv);
    // printArgs(args);

    // Iterate over args.branches
    for (const std::string& branch : args.branches) {
        // Read data from input file
        std::vector<std::vector<float>> rawData = readVectorFloatBranch(args.dataFile, args.treename, branch);

        // Flatten data
        std::vector<float> flattenedData;
        for (const auto& vec : rawData) {
            flattenedData.insert(flattenedData.end(), vec.begin(), vec.end());
        }

        // Create benchmark
        CompressorBenchmark benchmark(args.chunkSize, args.compressor, args.compressionOptions);

        // Run benchmark
        BenchmarkResult result = benchmark.run(flattenedData);

        // Write results to JSON
        writeJSON(args, branch, result);
        std::cout << std::endl;

        // Optionally write decompressed data to file
        if (args.writeDecompressed) {
            // writeDecompressedDataToRootFile(args, branch, result);
        }
    }

    return 0;
}