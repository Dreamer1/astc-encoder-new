#include "gtest/gtest.h"
#include "../astcenc.h"// Assuming the compress_astc function is declared in this header
#include <fstream>
#include <vector>
#include <iterator>
//#include <filesystem>

TEST(CompressAstcTest, BasicTest) {
    // Load the image
    std::string filename = "/Users/admin/Downloads/astc-encoder-4.7.0/Source/CompressTest/1.jpeg";
//    if (!std::filesystem::exists(filename)) {
//        std::cerr << "ERROR: File " << filename << " does not exist\n";
//        return;
//    }
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file)
    {
        std::cerr << "Unable to open file for writing.\n";
        return;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    file.read((char*)buffer.data(), size);

    // Prepare the input and output buffers
    uint8_t* input_image_raw = buffer.data();
    size_t image_len = size;
    char* format = const_cast<char*>("JPEG");
    uint8_t* output_image_raw = new uint8_t[8480];
    uint32_t width = 180;
    uint32_t height = 180;
    uint32_t block_width = 8;
    uint32_t block_height = 8;
    float quality = 30;
    uint32_t is_srgb = 0;
    unsigned int repeat_count = 4;

    // Call the function
    astcenc_error result = compress_astc(input_image_raw, image_len, format, output_image_raw, width, height, block_width, block_height, quality, is_srgb, repeat_count);

    std::ofstream output_file("/Users/admin/Downloads/astc-encoder-4.7.0/Source/CompressTest/output.astc", std::ios::binary);
    if (!output_file) {
        std::cerr << "Unable to open file for writing.\n";
        return;
    }
    // Check the result
    EXPECT_EQ(result, ASTCENC_SUCCESS);

    output_file.write(reinterpret_cast<char*>(output_image_raw), 8480);
    output_file.close();

    // Check the output data
    EXPECT_NE(output_image_raw[0], 0);

    // Clean up
//    delete[] input_image_raw;
    delete[] output_image_raw;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

