/* Copyright (c) 2018 PaddlePaddle Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#pragma once

#include <stdint.h>
#include <cstddef>
#include <iostream>
#include <limits>

namespace paddle {
namespace zynqmp {

typedef int16_t half;

#define IMAGE_ALIGNMENT 16           // Aligned to 16
#define FILTER_NUM_ALIGNMENT 32      // Filter number aligned to 32
#define FILTER_ELEMENT_ALIGNMENT 16  // Filter element number aligned to 16
#define BS_NUM_ALIGNMENT 8
#define BIAS_NUM_ALIGNMENT 16

enum DDataType {
  DATA_TYPE_FP32 = 1,
  DATA_TYPE_FP16 = 0,
};

enum DLayoutType {
  LAYOUT_CHW = 1,
  LAYOUT_HWC = 0,
};

struct VersionArgs {
  void* buffer;
};

struct DeviceInfo {
  uint32_t filter_cap;
};

struct MemoryCopyArgs {
  void* src;
  void* dest;
  size_t size;
};

struct MemoryCacheArgs {
  void* address;
  size_t size;
};

struct MemoryBarrierArgs {};

struct BNArgs {
  bool enabled;
  void* bias_address;
  void* scale_address;
};

/**
Conv and Pooling kernel
*/
struct KernelArgs {
  uint32_t width;
  uint32_t height;
  uint32_t stride_w;
  uint32_t stride_h;
};

struct ImageInputArgs {
  void* address;        // input featuremap virtual address
  void* scale_address;  // input scale address;
  uint32_t channels;
  uint32_t width;  // featuremap width
  uint32_t height;
  uint32_t pad_width;  // padding width;
  uint32_t pad_height;
};

struct ImageOutputArgs {
  void* address;         // output result address;
  float* scale_address;  // output scale address;
};

struct ConvArgs {
  bool relu_enabled;
  void* sb_address;  // scale and bias are interlaced;
  void* filter_address;
  void* filter_scale_address;
  uint32_t filter_num;
  uint32_t group_num;

  struct KernelArgs kernel;
  struct ImageInputArgs image;  // input image;
  struct ImageOutputArgs output;
};

struct DWconvArgs {
  bool relu_enabled;
  void* bias_address;
  void* filter_address;
  struct KernelArgs kernel;
  struct ImageInputArgs image;
  struct ImageOutputArgs output;
  uint16_t out_width;
  uint16_t out_height;
  uint16_t sub_conv_num;
};

struct PoolingArgs {
  uint16_t mode;
  uint16_t kernel_reciprocal;
  struct KernelArgs kernel;
  struct ImageInputArgs image;  // input image;
  struct ImageOutputArgs output;
  uint16_t out_width;
  uint16_t out_height;
};

// elementwise add arguments
struct EWAddArgs {
  bool relu_enabled;

  uint32_t const0;  // output0 = const0 x input0 + const1 x input1;
  uint32_t const1;
  struct ImageInputArgs image0;
  struct ImageInputArgs image1;
  struct ImageOutputArgs output;
};

struct BypassArgs {
  enum DDataType input_data_type;
  enum DDataType output_data_type;
  enum DLayoutType input_layout_type;
  enum DLayoutType output_layout_type;
  struct ImageInputArgs image;
  struct ImageOutputArgs output;
};

struct ScaleArgs {
  void* scale_address;
  void* bias_address;
  uint32_t wc_alignment;
  uint32_t channel_alignment;

  struct ImageInputArgs image;
  struct ImageOutputArgs output;
};

struct NormalizeArgs {
  void* input_image_address;
  void* output_image_address;
  uint32_t image_width;
  uint32_t image_height;
  uint32_t image_channel;
  uint32_t* output_scale_address;
};

struct ResizeArgs {
  void* input_image_address;
  void* output_image_address;
  uint32_t input_width;
  uint32_t input_height;
  uint32_t image_channel;
  uint32_t output_width;
  uint32_t output_height;
  uint32_t height_ratio;
  uint32_t width_ratio;
  uint32_t* output_scale_address;
};

struct PowerParameterArgs {
  uint16_t shift;
  uint16_t scale;
  uint16_t power;
};

struct NormalizeParameterArgs {
  uint32_t channel;
  uint32_t hight_width;
};

struct InplaceArgs {
  bool relu_enable;
  bool power_enable;
  bool normalize_enable;
};

struct FpgaRegWriteArgs {
  uint64_t address;  //
  uint64_t value;
};

struct FpgaRegReadArgs {
  uint64_t address;
  uint64_t value;
};

struct FpgaResetArgs {};

#define IOCTL_FPGA_MAGIC (('F' + 'P' + 'G' + 'A') / 4)

#define IOCTL_VERSION _IOW(IOCTL_FPGA_MAGIC, 01, struct VersionArgs)
#define IOCTL_DEVICE_INFO _IOW(IOCTL_FPGA_MAGIC, 100, struct DeviceInfo)

#define IOCTL_SEPARATOR_0 10

#define IOCTL_MEM_COPY _IOW(IOCTL_FPGA_MAGIC, 11, struct MemoryCopyArgs)
#define IOCTL_MEMCACHE_INVAL _IOW(IOCTL_FPGA_MAGIC, 12, struct MemoryCacheArgs)
#define IOCTL_MEMCACHE_FLUSH _IOW(IOCTL_FPGA_MAGIC, 13, struct MemoryCacheArgs)
#define IOCTL_MEMORY_BARRIER \
  _IOW(IOCTL_FPGA_MAGIC, 14, struct MemoryBarrierArgs)

#define IOCTL_SEPARATOR_1 20

#define IOCTL_CONFIG_CONV _IOW(IOCTL_FPGA_MAGIC, 21, struct ConvArgs)
#define IOCTL_CONFIG_POOLING _IOW(IOCTL_FPGA_MAGIC, 22, struct PoolingArgs)
#define IOCTL_CONFIG_EW _IOW(IOCTL_FPGA_MAGIC, 23, struct EWAddArgs)
#define IOCTL_CONFIG_BYPASS _IOW(IOCTL_FPGA_MAGIC, 24, struct BypassArgs)
#define IOCTL_CONFIG_SCALE _IOW(IOCTL_FPGA_MAGIC, 25, struct ScaleArgs)
#define IOCTL_CONFIG_NORMALIZE _IOW(IOCTL_FPGA_MAGIC, 26, struct NormalizeArgs)
#define IOCTL_CONFIG_RESIZE _IOW(IOCTL_FPGA_MAGIC, 30, struct ResizeArgs)

#define IOCTL_CONFIG_DWCONV _IOW(IOCTL_FPGA_MAGIC, 31, struct DWconvArgs)

#define IOCTL_CONFIG_INPLACE _IOW(IOCTL_FPGA_MAGIC, 40, struct InplaceArgs)
#define IOCTL_CONFIG_POWER_PARAMETER \
  _IOW(IOCTL_FPGA_MAGIC, 41, struct PowerParameterArgs)
#define IOCTL_CONFIG_NORMALIZE_PARAMETER \
  _IOW(IOCTL_FPGA_MAGIC, 42, struct NormalizeParameterArgs)
#define IOCTL_FPGA_REG_READ _IOW(IOCTL_FPGA_MAGIC, 50, struct FpgaRegReadArgs)
#define IOCTL_FPGA_REG_WRITE _IOW(IOCTL_FPGA_MAGIC, 51, struct FpgaRegWriteArgs)
#define IOCTL_FPGA_RESET _IOW(IOCTL_FPGA_MAGIC, 52, struct FpgaResetArgs)

//============================== API =============================

struct DeconvArgs {
  uint32_t sub_conv_num;
  uint32_t group_num;
  uint32_t filter_num;
  uint32_t omit_size;
  uint32_t sub_output_width;
  uint32_t sub_output_height;
  struct ImageOutputArgs output;
  struct SplitConvArgs* split_conv_args;
};

struct SplitArgs {
  uint32_t image_num;
  int16_t* image_in;
  float* scale_in;
  void** images_out;
  float** scales_out;
  uint32_t* out_channel_nums;
  uint32_t height;
  uint32_t width;
};

struct ConcatArgs {
  uint32_t image_num;
  half** images_in;
  float** scales_in;
  void* image_out;
  float* scale_out;
  uint32_t* channel_num;
  uint32_t height;
  uint32_t width;
};

struct SplitConvArgs {
  uint32_t split_num;
  uint32_t group_num;
  uint32_t filter_num;
  struct ImageOutputArgs output;
  struct ConvArgs* conv_arg;
  struct ConcatArgs concat_arg;
};

struct GroupConvArgs {
  uint32_t group_num;
  uint32_t filter_num;
  struct ImageOutputArgs output;
  struct SplitConvArgs* conv_args;
  struct ConcatArgs concat_arg;
};

inline int align_to_x(int num, int x) { return (num + x - 1) / x * x; }
int open_device();
void close_device();
void reset_device();

void* fpga_malloc(size_t size);
void fpga_free(void* ptr);
size_t fpga_get_memory_size(void* ptr);
size_t fpga_get_memory_size_max();
size_t fpga_diagnose_memory(int detailed);

void fpga_copy(void* dst, const void* src, int size);

int fpga_flush(void* address, size_t size);
int fpga_invalidate(void* address, size_t size);

int get_device_info(const struct DeviceInfo& args);

int perform_bypass(const struct BypassArgs& args);
int compute_fpga_conv_basic(const struct ConvArgs& args);
int compute_fpga_conv(const struct SplitConvArgs& args);
int compute_fpga_pool(const struct PoolingArgs& args);
int compute_fpga_ewadd(const struct EWAddArgs& args);
int compute_fpga_scale(const struct ScaleArgs& args);
int compute_fpga_concat(const struct ConcatArgs& args);
int compute_fpga_resize(const struct ResizeArgs& args);

int config_power(const struct PowerArgs& args);
int compute_fpga_dwconv(const struct DWconvArgs& args);
int config_norm_param(const struct NormalizeParameterArgs& args);
int compute_norm(const struct NormalizeArgs& args);

int config_inplace(const struct InplaceArgs& args);

int flush_cache(void* addr, int size);
int invalidate_cache(void* addr, int size);

int16_t fp32_2_fp16(float fp32_num);
float fp16_2_fp32(int16_t fp16_num);
}  // namespace zynqmp
}  // namespace paddle
