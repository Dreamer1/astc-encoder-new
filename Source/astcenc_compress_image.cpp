#include "astcenc.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define WUFFS_IMPLEMENTATION
#include "wuffs-v0.3.c"
#include <chrono>
#include <fstream>
#include "astcenccli_internal.h"
#include <vector>
#include <cstring>
#include <cstdlib>
#include <iostream>

#define ASTCENC_COMPRESS_PUBLIC extern "C" __attribute__ ((visibility ("default")))


ASTCENC_COMPRESS_PUBLIC astcenc_error compress_astc(uint8_t *input_image_raw,size_t image_len,char* format,
                                       uint8_t *output_image_raw, uint32_t width,
                                       uint32_t height, uint32_t block_width,
                                       uint32_t block_height, float quality,uint32_t is_srgb,unsigned int repeat_count)
{

 // 在函数开始时声明变量
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;
    std::chrono::duration<double, std::milli> diff;
    std::ofstream log_file;

  astcenc_profile profile = is_srgb ? ASTCENC_PRF_LDR_SRGB : ASTCENC_PRF_LDR;
  astcenc_config config{};
  astcenc_error result = astcenc_config_init(
      profile, block_width, block_height, 1,quality, 0, &config);
  if (result != ASTCENC_SUCCESS)
  {
    return result;
  }

  astcenc_context* codec_context;
//  astcenc_context *codec_context;
  result = astcenc_context_alloc(&config, repeat_count, &codec_context);
  if (result != ASTCENC_SUCCESS)
  {
    return result;
  }

   start = std::chrono::high_resolution_clock::now();
  astcenc_image* uncompressed_image = load_ncimage_from_memory(format, input_image_raw, image_len, 1);
   end = std::chrono::high_resolution_clock::now();
  diff = end-start;
    log_file.open("log.txt", std::ios_base::app);// append instead of overwrite
    log_file << "uncompressed_image took " << diff.count() << " milli seconds.\n size is " << image_len << "\n";

//  return ASTCENC_ERR_BAD_BLOCK_SIZE;
//  astcenc_image uncompressed_image{width, height, 1, ASTCENC_TYPE_U8,
//                                   reinterpret_cast<void **>(&input_image_raw)};
  astcenc_swizzle swz_encode{ASTCENC_SWZ_R, ASTCENC_SWZ_G, ASTCENC_SWZ_B,
                             ASTCENC_SWZ_A};

 unsigned int xblocks = (width + block_width - 1) / block_width;
 unsigned int yblocks = (height + block_height - 1) / block_height;
 unsigned int zblocks = 1;
  size_t comp_len = xblocks * yblocks * zblocks * 16;// 计算所需的输出缓冲区大小

//  std::vector<uint8_t> res(sizeof(astcenc_header) + comp_len);
  uint8_t* comp_data = output_image_raw + sizeof(astcenc_header);
   start = std::chrono::high_resolution_clock::now();

    compression_workload work;
    work.context = codec_context;
    work.image = uncompressed_image;
    work.swizzle = swz_encode;
    work.data_out = comp_data;
    work.data_len = comp_len;
    work.error = ASTCENC_SUCCESS;
   log_file << "repeat_count is " << repeat_count << "\n";

    if (repeat_count > 1){

       log_file.open("log.txt", std::ios_base::app);// append instead of overwrite
       log_file << "launch_threads is " << repeat_count <<  "\n";
        launch_threads("Compression", repeat_count, compression_workload_runner_new, &work);
        result = work.error;
//   			return result;
    }else{
        log_file.open("log.txt", std::ios_base::app);// append instead of overwrite
          log_file << "astcenc_compress_image is " << repeat_count <<  "\n";
          result = astcenc_compress_image(work.context, work.image, &work.swizzle,
          work.data_out, work.data_len, 0);
     }
     log_file.open("log.txt", std::ios_base::app);// append instead of overwrite
      end = std::chrono::high_resolution_clock::now();
      diff = end-start;
       log_file << "astcenc_compress_image took " << diff.count() << " milli seconds.\n size is " << image_len << "\n";

      start = std::chrono::high_resolution_clock::now();
       astcenc_context_free(codec_context);
       end = std::chrono::high_resolution_clock::now();
        diff = end-start;
     //   log_file.open("log.txt", std::ios_base::app);// append instead of overwrite
        log_file << "astcenc_context_free took " << diff.count() << " milli seconds.\n size is " << image_len << "\n";

    // 获取header头信息
    astcenc_header &hdr = *reinterpret_cast<astcenc_header *>(output_image_raw);
    hdr = get_astcenc_header(uncompressed_image, block_width, block_height, 1);

  return result;
}

 astcenc_header get_astcenc_header(astcenc_image* image, unsigned int block_x, unsigned int block_y, unsigned int block_z) {
    astcenc_header hdr;
    hdr.magic[0] =  ASTCENC_MAGIC_ID        & 0xFF;
    hdr.magic[1] = (ASTCENC_MAGIC_ID >>  8) & 0xFF;
    hdr.magic[2] = (ASTCENC_MAGIC_ID >> 16) & 0xFF;
    hdr.magic[3] = (ASTCENC_MAGIC_ID >> 24) & 0xFF;

    hdr.block_x = static_cast<uint8_t>(block_x);
    hdr.block_y = static_cast<uint8_t>(block_y);
    hdr.block_z = static_cast<uint8_t>(block_z);

    hdr.dim_x[0] =  image->dim_x        & 0xFF;
    hdr.dim_x[1] = (image->dim_x >>  8) & 0xFF;
    hdr.dim_x[2] = (image->dim_x >> 16) & 0xFF;

    hdr.dim_y[0] =  image->dim_y       & 0xFF;
    hdr.dim_y[1] = (image->dim_y >>  8) & 0xFF;
    hdr.dim_y[2] = (image->dim_y >> 16) & 0xFF;

    hdr.dim_z[0] =  image->dim_z        & 0xFF;
    hdr.dim_z[1] = (image->dim_z >>  8) & 0xFF;
    hdr.dim_z[2] = (image->dim_z >> 16) & 0xFF;

    return hdr;
}

 astcenc_image* load_image_from_memory(
    const uint8_t* buffer,
    size_t size,
    bool y_flip
   ) {
    int dim_x, dim_y;

//    if (stbi_is_hdr_from_memory(buffer, size))
//    {
////        float* data = stbi_loadf_from_memory(buffer, size, &dim_x, &dim_y, nullptr, STBI_rgb_alpha);
////        if (data)
////        {
////            astcenc_image* img = astc_img_from_floatx4_array(data, dim_x, dim_y, y_flip);
////            stbi_image_free(data);
//////            is_hdr = true;
//////            component_count = 4;
////            return img;
////        }
//    }
//    else
//    {
        auto start = std::chrono::high_resolution_clock::now();
        uint8_t* data = stbi_load_from_memory(buffer, (int)size, &dim_x, &dim_y, nullptr, STBI_rgb_alpha);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> diff = end-start;
        std::ofstream log_file("log.txt", std::ios_base::app); // append instead of overwrite
        log_file << "stbi_load_from_memory took " << diff.count() << " milli seconds.\n size is " << size << "\n";
        if (data)
        {
            astcenc_image* img = astc_img_from_unorm8x4_array(data, dim_x, dim_y, y_flip);
            free(data);
            return img;
        }
//    }

    return nullptr;
   }
 astcenc_image* load_png_with_wuffs_from_memory(
    const uint8_t* buffer,
    size_t size,
    bool y_flip
   ) {

    wuffs_png__decoder *dec = wuffs_png__decoder__alloc();
    if (!dec)
    {
        return nullptr;
    }

    wuffs_base__image_config ic;
    uint8_t* non_const_buffer = const_cast<uint8_t*>(buffer);
    wuffs_base__io_buffer src = wuffs_base__ptr_u8__reader(non_const_buffer, size, true);
    wuffs_base__status status = wuffs_png__decoder__decode_image_config(dec, &ic, &src);
    if (status.repr)
    {
        return nullptr;
    }

    uint32_t dim_x = wuffs_base__pixel_config__width(&ic.pixcfg);
    uint32_t dim_y = wuffs_base__pixel_config__height(&ic.pixcfg);
    size_t num_pixels = dim_x * dim_y;
    if (num_pixels > (SIZE_MAX / 4))
    {
        return nullptr;
    }

    // Override the image's native pixel format to be RGBA_NONPREMUL
    wuffs_base__pixel_config__set(
        &ic.pixcfg,
        WUFFS_BASE__PIXEL_FORMAT__RGBA_NONPREMUL,
        WUFFS_BASE__PIXEL_SUBSAMPLING__NONE,
        dim_x, dim_y);

    // Configure the work buffer
    size_t workbuf_len = wuffs_png__decoder__workbuf_len(dec).max_incl;
    if (workbuf_len > SIZE_MAX)
    {
        return nullptr;
    }

    wuffs_base__slice_u8 workbuf_slice = wuffs_base__make_slice_u8((uint8_t*)malloc(workbuf_len), workbuf_len);
    if (!workbuf_slice.ptr)
    {
        return nullptr;
    }

    wuffs_base__slice_u8 pixbuf_slice = wuffs_base__make_slice_u8((uint8_t*)malloc(num_pixels * 4), num_pixels * 4);
    if (!pixbuf_slice.ptr)
    {
        return nullptr;
    }

    wuffs_base__pixel_buffer pb;
    status = wuffs_base__pixel_buffer__set_from_slice(&pb, &ic.pixcfg, pixbuf_slice);
    if (status.repr)
    {
        return nullptr;
    }

    // Decode the pixels
    status = wuffs_png__decoder__decode_frame(dec, &pb, &src, WUFFS_BASE__PIXEL_BLEND__SRC, workbuf_slice, NULL);
    if (status.repr)
    {
        return nullptr;
    }

    astcenc_image* img = astc_img_from_unorm8x4_array(pixbuf_slice.ptr, dim_x, dim_y, y_flip);

    free(pixbuf_slice.ptr);
    free(workbuf_slice.ptr);
    free(dec);

    return img;
   }

//loader_desc loader_descs_astc[] = {
//           // LDR formats
//           {".png",   ".PNG",  load_png_with_wuffs_from_memory},
//           // Container formats
//           // Generic catch all; this one must be last in the list
//           { NULL, NULL, load_image_from_memory }
//       };
// const int loader_descr_count_astc = sizeof(loader_descs_astc) / sizeof(loader_descs_astc[0]);


 astcenc_image* load_ncimage_from_memory(
            char* eptr,
           	uint8_t* data,
           	size_t size,
           	int y_flip
           ) {
           	// Get the file extension
           	// In this case, we can't get the file extension from the byte stream.
           	// We need to modify the logic to determine the appropriate loader function.
            astcenc_image* result = NULL;
            bool y_flip_bool = y_flip == 0  ? false : true;
            auto start = std::chrono::high_resolution_clock::now();
           	// Scan through descriptors until a matching loader is found
           	if (strcmp(eptr, "PNG") == 0){
           	    result = load_png_with_wuffs_from_memory(data, size, y_flip_bool);
           	}else{
           	    result = load_image_from_memory(data, size, y_flip_bool);
            }
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> diff = end-start;
            std::ofstream log_file("log.txt", std::ios_base::app); // append instead of overwrite
            log_file << "load_ncimage_from_memory took " << diff.count() << " milli seconds.\n size is " << size << "\n";

//           	for (unsigned int i = 0; i < loader_descr_count_astc; i++)
//           	{
//           		// We need to modify the condition to choose the appropriate loader function.
//           		// In this case, we can't compare the file extension with the ending1 and ending2.
//           		// We need to modify the loader_descs_astc structure and the condition here.
//           		if (loader_descs_astc[i].ending1 == NULL || strcmp(eptr, loader_descs_astc[i].ending1) == 0)
//           		{
//
//           			result = loader_descs_astc[i].loader_func(data, size, y_flip_bool);
//           			break;
//           		}
//           	}
            // Convert bool back to int
           	// Should never reach here - stb_image provides a generic handler
           	return result;
           }
 astcenc_image* astc_img_from_unorm8x4_array(
           	const uint8_t* data,
           	unsigned int dim_x,
           	unsigned int dim_y,
           	bool y_flip
           ) {
           	astcenc_image* img = alloc_image(8, dim_x, dim_y, 1);

           	for (unsigned int y = 0; y < dim_y; y++)
           	{
           		uint8_t* data8 = static_cast<uint8_t*>(img->data[0]);
           		unsigned int y_src = y_flip ? (dim_y - y - 1) : y;
           		const uint8_t* src = data + 4 * dim_x * y_src;

           		for (unsigned int x = 0; x < dim_x; x++)
           		{
           			data8[(4 * dim_x * y) + (4 * x    )] = src[4 * x    ];
           			data8[(4 * dim_x * y) + (4 * x + 1)] = src[4 * x + 1];
           			data8[(4 * dim_x * y) + (4 * x + 2)] = src[4 * x + 2];
           			data8[(4 * dim_x * y) + (4 * x + 3)] = src[4 * x + 3];
           		}
           	}

           	return img;
           }

 astcenc_image *alloc_image(
	unsigned int bitness,
	unsigned int dim_x,
	unsigned int dim_y,
	unsigned int dim_z
) {
	astcenc_image *img = new astcenc_image;
	img->dim_x = dim_x;
	img->dim_y = dim_y;
	img->dim_z = dim_z;

	void** data = new void*[dim_z];
	img->data = data;

	if (bitness == 8)
	{
		img->data_type = ASTCENC_TYPE_U8;
		for (unsigned int z = 0; z < dim_z; z++)
		{
			data[z] = new uint8_t[dim_x * dim_y * 4];
		}
	}
	else if (bitness == 16)
	{
		img->data_type = ASTCENC_TYPE_F16;
		for (unsigned int z = 0; z < dim_z; z++)
		{
			data[z] = new uint16_t[dim_x * dim_y * 4];
		}
	}
	else // if (bitness == 32)
	{
		assert(bitness == 32);
		img->data_type = ASTCENC_TYPE_F32;
		for (unsigned int z = 0; z < dim_z; z++)
		{
			data[z] = new float[dim_x * dim_y * 4];
		}
	}

	return img;
}


/**
 * @brief Runner callback function for a compression worker thread.
 *
 * @param thread_count   The number of threads in the worker pool.
 * @param thread_id      The index of this thread in the worker pool.
 * @param payload        The parameters for this thread.
 */
 void compression_workload_runner_new(
	int thread_count,
	int thread_id,
	void* payload
) {
	(void)thread_count;

	std::ofstream log_file;
            	log_file.open("log.txt", std::ios_base::app);// append instead of overwrite
                    log_file << "compression_workload_runner_new thread_id " << thread_id << "\n";
                    log_file << "compression_workload_runner_new thread_count " << thread_count << "\n";
                    log_file << "compression_workload_runner_new payload " << payload << "\n";
	compression_workload* work = static_cast<compression_workload*>(payload);
	log_file << "compression_workload_runner_new work " << work << "\n";
	astcenc_error error = astcenc_compress_image(
	                       work->context, work->image, &work->swizzle,
	                       work->data_out, work->data_len, thread_id);
log_file << "compression_workload_runner_new error " << error << "\n";
	// This is a racy update, so which error gets returned is a random, but it
	// will reliably report an error if an error occurs
	if (error != ASTCENC_SUCCESS)
	{
		work->error = error;
	}
}
