# benchmarking/CMakeLists.txt

add_library(compressorbench STATIC
    CompressorBenchmark.cpp
    CompressorBenchmark.hpp
    Compressor.hpp
    TruncCompressor.cpp
    TruncCompressor.hpp
    SZ3Compressor.cpp
    SZ3Compressor.hpp
)
target_link_libraries(compressorbench utils ZLIB::ZLIB SZ3::SZ3)

# add_executable(benchmark-TruncCompressor benchmark-TruncCompressor.cpp)
# target_link_libraries(benchmark-TruncCompressor benchmarking)

# add_executable(benchmark-SZ3Compressor benchmark-SZ3Compressor.cpp)
# target_link_libraries(benchmark-SZ3Compressor benchmarking)

add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark compressorbench)