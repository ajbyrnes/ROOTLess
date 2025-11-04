
/**
 * @file TruncCompressor.hpp
 * @brief TruncCompressor class for lossy float compression using mantissa truncation and zlib.
 */

#pragma once

#include <vector>
#include <stdexcept>
#include <cstdint>
#include <cstring>
#include <zlib.h>
#include "Compressor.hpp"

/**
 * @class TruncCompressor
 * @brief Compressor that truncates mantissa bits of floats and compresses with zlib.
 */
class TruncCompressor : public Compressor {
public:
    /**
     * @brief Construct a TruncCompressor given values.
     * @param compressionLevel zlib compression level.
     * @param mantissaBits Number of mantissa bits to keep.
     */
    TruncCompressor(int compressionLevel, int mantissaBits);

    /**
     * @brief Construct a TruncCompressor from configuration map.
     * @param config Map of configuration options.
     * Keys: 
     *  "compressionLevel" - zlib compression level (int).
     *  "mantissaBits" - number of mantissa bits to keep (int).
     */
    TruncCompressor(const std::map<std::string, std::string>& config);


    /** Setters and getters for mantissa bits and compression level. */
    void setMantissaBits(int mantissaBits);
    int getMantissaBits() const;
    void setCompressionLevel(int level);
    int getCompressionLevel() const;

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
     * @param compressedData Compressed data to decompress.
     * @return Decompressed float vector containing decompressed result.
     */
    std::vector<float> decompress(const CompressedData& compressedData) override;

private:
    int mantissaBits_ = 8; ///< Number of mantissa bits to keep (0-23 for float)
    int compressionLevel_ = Z_BEST_COMPRESSION; ///< zlib compression level

    /**
     * @brief Truncate mantissa of floats to mantissaBits bits, with rounding.
     * @param values Vector of float values.
     * @param mantissaBits Number of mantissa bits to keep.
     * @return Vector of truncated float values.
     */
    static std::vector<float> truncate_mantissas(const std::vector<float>& values, int mantissaBits);
};