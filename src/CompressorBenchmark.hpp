
 /**
 * @file CompressorBenchmark.hpp
 * @brief Classes and structures for benchmarking data compressors.
 */
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <fstream>
#include <optional>

#include "Compressor.hpp"
#include "TruncCompressor.hpp"
#include "SZ3Compressor.hpp"
#include "../utils/utils.hpp"

struct BenchmarkResult {
    std::vector<float> decompressedData{};

    double compressionThroughputMBps{};
    double decompressionThroughputMBps{};

    double compressionRatio{};
    double MSE{};
    double PSNR{};
    double meanRelError{};
    double maxRelError{};
    double meanAbsError{};
    double maxAbsError{};
    double KLdivergence{};
    double JSdivergence{};
    double WassersteinDistance{};
    double KSstatistic{};
};

/**
 * @class CompressorBenchmark
 * @brief Class for running and recording benchmarks of data compressors.
 */
class CompressorBenchmark {
public:
    /**
     * @brief Construct a CompressorBenchmark.
     * @param compressor Shared pointer to compressor to benchmark.
     */
    CompressorBenchmark(int chunkSize, const std::string& compressorName,
                        const std::map<std::string, std::string>& compressorOptions)
        :  chunkSize_(chunkSize)
    {
        if (compressorName == "BitTruncation") {
            compressor_ = std::make_shared<TruncCompressor>(compressorOptions);
        } else if (compressorName == "SZ3") {
            compressor_ = std::make_shared<SZ3Compressor>(compressorOptions);
        } else {
            throw std::invalid_argument("Unknown compressor: " + compressorName);
        }
    }

    /**
     * @brief Run the benchmark and record results.
     *
     * @param data Input data to compress.
     * @param returnDecompressed If true, return the decompressed data in the result.
     */
    BenchmarkResult run(const std::vector<float>& data, bool returnDecompressed = false);


private:
    std::shared_ptr<Compressor> compressor_;    ///< Compressor to benchmark
    int chunkSize_;                             ///< Size of chunks that get compressed

    double computeKLDivergence(const std::vector<float>& original, const std::vector<float>& compressed);
    double computeJSDivergence(const std::vector<float>& original, const std::vector<float>& compressed);
    double computeWassersteinDistance(const std::vector<float>& original, const std::vector<float>& compressed);
    double computeKSStatistic(const std::vector<float>& original, const std::vector<float>& compressed);
};