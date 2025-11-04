
/**
 * @file CompressorBenchmark.cpp
 * @brief Implementation of CompressorBenchmark for benchmarking data compressors and recording results.
 */
#include <iostream>
#include <format>
#include <cmath>
#include <numeric>
#include <optional>

#include "CompressorBenchmark.hpp"
#include "../utils/utils.hpp"

BenchmarkResult CompressorBenchmark::run(const std::vector<float>& data, bool returnDecompressed) {
    if (!compressor_) {
        throw std::runtime_error("Compressor not initialized");
    }

    std::cout << timeMessage(std::format(
        "Running benchmark for compressor '{}' with chunkSize {} bytes", 
        compressor_->toString(), chunkSize_)
    ) << std::endl;
    
    // Set up accumulators
    double totalCompressionTimeMs = 0.0;
    double totalDecompressionTimeMs = 0.0;
    size_t totalCompressedBytes = 0;

    // Perform compression in chunks
    // Chunk size is in bytes
    std::vector<float> decompressedData;
    size_t totalBytes = data.size() * sizeof(float);
    int numChunks = std::ceil(static_cast<double>(totalBytes) / chunkSize_);

    for (int chunkInx = 0; chunkInx < numChunks; ++chunkInx) {
        // Get next chunk
        size_t offset = chunkInx * chunkSize_;
        size_t end = std::min(offset + chunkSize_, totalBytes);
        size_t numFloats = (end - offset) / sizeof(float);
        std::vector<float> chunk(data.begin() + (offset / sizeof(float)), data.begin() + (offset / sizeof(float)) + numFloats);

        // Compress chunk
        auto startCompression = std::chrono::high_resolution_clock::now();
        CompressedData compressedChunk = compressor_->compress(chunk);
        auto endCompression = std::chrono::high_resolution_clock::now();

        // Record compression time and compressed size
        double chunkCompressionTimeMs = std::chrono::duration<double, std::milli>(endCompression - startCompression).count();
        totalCompressionTimeMs += chunkCompressionTimeMs;

        totalCompressedBytes += compressedChunk.data.size();

        // Decompress chunk
        auto startDecompression = std::chrono::high_resolution_clock::now();
        std::vector<float> decompressedChunk = compressor_->decompress(compressedChunk);
        auto endDecompression = std::chrono::high_resolution_clock::now();

        // Record decompression time and decompressed data
        double chunkDecompressionTimeMs = std::chrono::duration<double, std::milli>(endDecompression - startDecompression).count();
        totalDecompressionTimeMs += chunkDecompressionTimeMs;

        std::move(decompressedChunk.begin(), decompressedChunk.end(), std::back_inserter(decompressedData));
    }

    // Calculate overall compression ratio
    double compressionRatio = totalBytes / static_cast<double>(totalCompressedBytes);

    // Calculate compression and decompression throughput in MB/s
    double compressionThroughputMBps = totalBytes / (totalCompressionTimeMs * 1e-3) / (1024 * 1024);
    double decompressionThroughputMBps = totalBytes / (totalDecompressionTimeMs * 1e-3) / (1024 * 1024);

    // Calculate MSE and PSNR
    double mse = 0.0;
    if (decompressedData.size() == data.size()) {
        mse = std::inner_product(data.begin(), data.end(), decompressedData.begin(), 0.0, 
            std::plus<>(),
            [](float a, float b) { return (a - b) * (a - b); }
        ) / data.size();
    } else {
        std::cerr << "Warning: Decompressed data size does not match original data size. MSE and PSNR will be set to NaN." << std::endl;
        mse = std::nan("");
    }

    // Calculate as 20log_10(MAX_I - 10log_10(MSE))
    double valueRange = *std::max_element(data.begin(), data.end()) - *std::min_element(data.begin(), data.end());
    double psnr = (mse > 0.0) ? 20.0 * std::log10(valueRange - 10.0 * std::log10(mse)) : std::nan("");

    // Calculate mean and max relative and absolute error
    std::vector<double> absErrors;
    std::vector<double> relErrors;
    if (decompressedData.size() == data.size()) {
        absErrors.resize(data.size());
        relErrors.resize(data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            absErrors[i] = std::abs(data[i] - decompressedData[i]);
            relErrors[i] = (data[i] != 0.0f) ? absErrors[i] * 100.0 / std::abs(data[i]) : 0.0;
        }
    } else {
        absErrors = {std::nan("")};
        relErrors = {std::nan("")};
    }

    double meanAbsError = (absErrors.size() > 0) ? std::accumulate(absErrors.begin(), absErrors.end(), 0.0) / absErrors.size() : std::nan("");
    double maxAbsError = (absErrors.size() > 0) ? *std::max_element(absErrors.begin(), absErrors.end()) : std::nan("");
    double meanRelError = (relErrors.size() > 0) ? std::accumulate(relErrors.begin(), relErrors.end(), 0.0) / relErrors.size() : std::nan("");
    double maxRelError = (relErrors.size() > 0) ? *std::max_element(relErrors.begin(), relErrors.end()) : std::nan("");

    // Calculate distribution metrics (KL divergence, JS divergence, Wasserstein distance, KS statistic)
    // double klDiv = computeKLDivergence(data, decompressedData);
    // double jsDiv = computeJSDivergence(data, decompressedData);
    // double wassersteinDist = computeWassersteinDistance(data, decompressedData);
    // double ksStat = computeKSStatistic(data, decompressedData);

    // Return results
    return {
        .decompressedData = returnDecompressed ? decompressedData : std::vector<float>{},
        .compressionThroughputMBps = compressionThroughputMBps,
        .decompressionThroughputMBps = decompressionThroughputMBps,
        .compressionRatio = compressionRatio,
        .MSE = mse,
        .PSNR = psnr,
        .meanRelError = meanRelError,
        .maxRelError = maxRelError,
        .meanAbsError = meanAbsError,
        .maxAbsError = maxAbsError
    };
}

// double CompressorBenchmark::computeKLDivergence(const std::vector<float>& original, const std::vector<float>& compressed) {
//     if (original.size() != compressed.size()) {
//         throw std::invalid_argument("Original and compressed data must have the same size for KL divergence calculation.");
//     }

//     double klDiv = 0.0;
// }

// double CompressorBenchmark::computeJSDivergence(const std::vector<float>& original, const std::vector<float>& compressed) {
//     // Placeholder implementation
//     return 0.0;
// }

// double CompressorBenchmark::computeWassersteinDistance(const std::vector<float>& original, const std::vector<float>& compressed) {
//     // Placeholder implementation
//     return 0.0;
// }

// double CompressorBenchmark::computeKSStatistic(const std::vector<float>& original, const std::vector<float>& compressed) {
//     // Placeholder implementation
//     return 0.0;
// }