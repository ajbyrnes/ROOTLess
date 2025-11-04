
/**
 * @file SZ3Compressor.cpp
 * @brief Implementation of SZ3Compressor for scientific data compression using SZ3 library.
 */
#include "SZ3Compressor.hpp"
#include <SZ3/api/sz.hpp>

SZ3Compressor::SZ3Compressor(SZ3::ALGO algorithm, SZ3::EB errorBoundMode, double errorBound)
    : algorithm_(algorithm), errorBoundMode_(errorBoundMode), errorBound_(errorBound)
{
    setAlgorithm(algorithm);
    setErrorBoundMode(errorBoundMode);
    setErrorBound(errorBound);
}

SZ3Compressor::SZ3Compressor(const std::map<std::string, std::string>& options) {
    // Parse options
    auto it = options.find("algorithm");
    if (it != options.end()) {
        setAlgorithm(static_cast<SZ3::ALGO>(std::stoi(it->second)));
    } else {
        throw std::invalid_argument("algorithm is required in SZ3Compressor config");
    }

    it = options.find("errorBoundMode");
    if (it != options.end()) {
        setErrorBoundMode(static_cast<SZ3::EB>(std::stoi(it->second)));
    } else {
        throw std::invalid_argument("errorBoundMode is required in SZ3Compressor config");
    }

    it = options.find("errorBoundValue");
    if (it != options.end()) {
        setErrorBound(std::stod(it->second));
    } else {
        throw std::invalid_argument("errorBoundValue is required in SZ3Compressor config");
    }
}

void SZ3Compressor::setAlgorithm(SZ3::ALGO algorithm) {
    switch(algorithm) {
        case SZ3::ALGO_LORENZO_REG:
        case SZ3::ALGO_INTERP_LORENZO:
        case SZ3::ALGO_INTERP:
        case SZ3::ALGO_NOPRED:
        case SZ3::ALGO_LOSSLESS:
            break;
        default:
            throw std::invalid_argument("Invalid or unsupported algorithm value: " + std::to_string(algorithm));
    }
    algorithm_ = algorithm;
}

SZ3::ALGO SZ3Compressor::getAlgorithm() const {
    return algorithm_;
}

void SZ3Compressor::setErrorBoundMode(SZ3::EB errorBoundMode) {
    switch(errorBoundMode) {
        case SZ3::EB_ABS:
        case SZ3::EB_REL:
            break;
        default:
            throw std::invalid_argument("Invalid or unsupported errorBoundMode value: " + std::to_string(errorBoundMode));
    }
    errorBoundMode_ = errorBoundMode;
}

SZ3::EB SZ3Compressor::getErrorBoundMode() const {
    return errorBoundMode_;
}

void SZ3Compressor::setErrorBound(double errorBound) {
    if (errorBound <= 0) {
        throw std::invalid_argument("Error bound must be positive");
    }
    errorBound_ = errorBound;
}

double SZ3Compressor::getErrorBound() const {
    return errorBound_;
}

std::string SZ3Compressor::toString() const {
    return std::format("SZ3Compressor({},{},{})", static_cast<int>(algorithm_), static_cast<int>(errorBoundMode_), errorBound_);
}

std::map<std::string, std::string> SZ3Compressor::getConfig() const {
    return {
        {"algorithm", std::to_string(static_cast<int>(algorithm_))},
        {"errorBoundMode", std::to_string(static_cast<int>(errorBoundMode_))},
        {"errorBoundValue", std::to_string(errorBound_)}
    };
}

CompressedData SZ3Compressor::compress(const std::vector<float>& data) {
    // Make config
    SZ3::Config config = makeConfig({data.size()});

    // Compress
    size_t cmpSize = 0;
    char* cmpData{};

    cmpData = SZ_compress(
        config,
        data.data(),
        cmpSize
    );

    if (!cmpData) {
        throw std::runtime_error("SZ_compress failed to allocate output buffer");
    }

    // Move compressed data to CompressedData struct
    std::vector<uint8_t> compressedData(cmpData, cmpData + cmpSize);

    // Free the compressed data pointer
    free(cmpData);

    return CompressedData{std::move(compressedData), data.size(), getConfig()};
}

std::vector<float> SZ3Compressor::decompress(const CompressedData& compressed) {
    // Make config
    SZ3::Config config = makeConfig({compressed.numFloats});

    // Allocate output buffer
    float* dec_data_p = nullptr;

    // Call SZ_decompress
    SZ_decompress(
        config,
        reinterpret_cast<const char*>(compressed.data.data()),
        compressed.data.size(), // Pass compressed buffer size in bytes
        dec_data_p
    );

    if (!dec_data_p) {
        throw std::runtime_error("SZ_decompress failed to allocate output buffer");
    }

    std::vector<float> dec_data(dec_data_p, dec_data_p + compressed.numFloats);
    free(dec_data_p); // Free the decompressed data pointer

    return dec_data;
}

SZ3::Config SZ3Compressor::makeConfig(std::vector<size_t> dims) {
    SZ3::Config config = SZ3::Config({dims[0]});
    
    config.dataType = SZ_FLOAT;
    config.cmprAlgo = algorithm_;
    config.errorBoundMode = errorBoundMode_;
    if (errorBoundMode_ == SZ3::EB_ABS) {
        config.absErrorBound = errorBound_;
    } else {
        config.relErrorBound = errorBound_;
    }

    return config;
}
