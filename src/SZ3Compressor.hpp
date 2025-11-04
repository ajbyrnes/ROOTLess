
/**
 * @file SZ3Compressor.hpp
 * @brief SZ3Compressor class for lossy and lossless compression using SZ3 library.
 */

#pragma once

#include "Compressor.hpp"
#include <vector>
#include <memory>
#include <SZ3/utils/Config.hpp>
// #include <SZ3/api/sz.hpp>

/**
 * @class SZ3Compressor
 * @brief Compressor using the SZ3 library for scientific data.
 */
class SZ3Compressor : public Compressor {
public:
    /**
     * @brief Construct an SZ3Compressor.
     */
    SZ3Compressor(SZ3::ALGO algorithm, SZ3::EB errorBoundMode, double errorBound);

    SZ3Compressor(const std::map<std::string, std::string>& options);

    /** Setters and getters. */
    void setAlgorithm(SZ3::ALGO algorithm);
    SZ3::ALGO getAlgorithm() const;
    void setErrorBoundMode(SZ3::EB errorBoundMode);
    SZ3::EB getErrorBoundMode() const;
    void setErrorBound(double errorBound);
    double getErrorBound() const;

    std::string toString() const override;
    std::map<std::string, std::string> getConfig() const override;

    /**
     * @brief Compress input data.
     * @param data Uncompressed data to compress.
     * @return CompressedData containing compressed result.
     */
    CompressedData compress(const std::vector<float>& data) override;

    /**
     * @brief Decompress input data.
     * @param compressed Compressed data to decompress.
     * @return std::vector<float> containing decompressed result.
     */
    std::vector<float> decompress(const CompressedData& compressed) override;

private:
    SZ3::EB errorBoundMode_;        ///< Error bound mode
    SZ3::ALGO algorithm_;           ///< SZ3 algorithm
    SZ3::INTERP_ALGO interpAlgo_;   ///< Interpolation algorithm
    double errorBound_;             ///< Error bound value

    SZ3::Config makeConfig(std::vector<size_t> dims);
};