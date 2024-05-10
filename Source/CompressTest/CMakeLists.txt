cmake_minimum_required(VERSION 3.10)
#set(CMAKE_BUILD_TYPE Debug) # Debug Release
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")   # AddressSanitizer
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")  # AddressSanitizer
# 设置项目名称
project(test_compress)

# 添加gtest子目录
add_subdirectory(/Users/admin/Downloads/googletest /Users/admin/Downloads/googletest/build)

# 启用测试
enable_testing()

# 包含gtest库
include_directories(/Users/admin/Downloads/googletest/include)

# 创建测试可执行文件
add_executable(test_compress test_compress.cpp
../astcenc_averages_and_directions.cpp
../astcenc_block_sizes.cpp
../astcenc_color_quantize.cpp
../astcenc_color_unquantize.cpp
../astcenc_compress_symbolic.cpp
../astcenc_compute_variance.cpp
../astcenc_decompress_symbolic.cpp
../astcenc_diagnostic_trace.cpp
../astcenc_entry.cpp
../astcenccli_platform_dependents.cpp
../astcenc_compress_image.cpp
../add.cpp
../astcenc_find_best_partitioning.cpp
../astcenc_ideal_endpoints_and_weights.cpp
../astcenc_image.cpp
../astcenc_integer_sequence.cpp
../astcenc_mathlib.cpp
../astcenc_mathlib_softfloat.cpp
../astcenc_partition_tables.cpp
../astcenc_percentile_tables.cpp
../astcenc_pick_best_endpoint_format.cpp
../astcenc_quantization.cpp
../astcenc_symbolic_physical.cpp
../astcenc_weight_align.cpp
../astcenc_weight_quant_xfer_tables.cpp
)


# 添加gtest库的头文件路径
target_include_directories(test_compress PRIVATE /Users/admin/Downloads/googletest/include)

# 链接gtest库
target_link_libraries(test_compress gtest gtest_main)