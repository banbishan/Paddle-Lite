// Copyright (c) 2019 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once
#include <string>

// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
#define PADDLE_LITE_HELPER_DLL_IMPORT __declspec(dllimport)
#define PADDLE_LITE_HELPER_DLL_EXPORT __declspec(dllexport)
#define PADDLE_LITE_HELPER_DLL_LOCAL
#else
#if __GNUC__ >= 4
#define PADDLE_LITE_HELPER_DLL_IMPORT __attribute__((visibility("default")))
#define PADDLE_LITE_HELPER_DLL_EXPORT __attribute__((visibility("default")))
#else
#define PADDLE_LITE_HELPER_DLL_IMPORT
#define PADDLE_LITE_HELPER_DLL_EXPORT
#endif
#endif

#ifdef LITE_ON_TINY_PUBLISH
#define LITE_API PADDLE_LITE_HELPER_DLL_EXPORT
#define LITE_API_IMPORT PADDLE_LITE_HELPER_DLL_IMPORT
#else
#define LITE_API
#define LITE_API_IMPORT
#endif

namespace paddle {
namespace lite_api {

enum class TargetType : int {
  kUnk = 0,
  kHost = 1,
  kX86 = 2,
  kCUDA = 3,
  kARM = 4,
  kOpenCL = 5,
  kFPGA = 7,
  kNPU = 8,
  kAny = 6,  // any target
  NUM = 9,   // number of fields.
};
enum class PrecisionType : int {
  kUnk = 0,
  kFloat = 1,
  kInt8 = 2,
  kFP16 = 5,
  kInt32 = 3,
  kAny = 4,  // any precision
  kBool = 6,
  NUM = 7,  // number of fields.
};
enum class DataLayoutType : int {
  kUnk = 0,
  kNCHW = 1,
  kNHWC = 3,
  kAny = 2,  // any data layout
  NUM = 4,   // number of fields.
};

typedef enum {
  LITE_POWER_HIGH = 0,
  LITE_POWER_LOW = 1,
  LITE_POWER_FULL = 2,
  LITE_POWER_NO_BIND = 3,
  LITE_POWER_RAND_HIGH = 4,
  LITE_POWER_RAND_LOW = 5
} PowerMode;

enum class ActivationType : int {
  kIndentity = 0,
  kRelu = 1,
  kRelu6 = 2,
  kPRelu = 3,
  kLeakyRelu = 4,
  kSigmoid = 5,
  kTanh = 6,
  kSwish = 7
};

static size_t PrecisionTypeLength(PrecisionType type) {
  switch (type) {
    case PrecisionType::kFloat:
      return 4;
    case PrecisionType::kInt8:
      return 1;
    case PrecisionType::kInt32:
      return 4;
    case PrecisionType::kFP16:
      return 2;
    default:
      return 4;
  }
}

#define TARGET(item__) paddle::lite_api::TargetType::item__
#define PRECISION(item__) paddle::lite_api::PrecisionType::item__
#define DATALAYOUT(item__) paddle::lite_api::DataLayoutType::item__

const std::string& TargetToStr(TargetType target);

const std::string& PrecisionToStr(PrecisionType precision);

const std::string& DataLayoutToStr(DataLayoutType layout);

const std::string& TargetRepr(TargetType target);

const std::string& PrecisionRepr(PrecisionType precision);

const std::string& DataLayoutRepr(DataLayoutType layout);

/*
 * Place specifies the execution context of a Kernel or input/output for a
 * kernel. It is used to make the analysis of the MIR more clear and accurate.
 */
struct LITE_API Place {
  TargetType target{TARGET(kUnk)};
  PrecisionType precision{PRECISION(kUnk)};
  DataLayoutType layout{DATALAYOUT(kUnk)};
  int16_t device{0};  // device ID

  Place() = default;
  Place(TargetType target,
        PrecisionType precision = PRECISION(kFloat),
        DataLayoutType layout = DATALAYOUT(kNCHW),
        int16_t device = 0)
      : target(target), precision(precision), layout(layout), device(device) {}

  bool is_valid() const {
    return target != TARGET(kUnk) && precision != PRECISION(kUnk) &&
           layout != DATALAYOUT(kUnk);
  }

  size_t hash() const;

  bool operator==(const Place& other) const {
    return target == other.target && precision == other.precision &&
           layout == other.layout && device == other.device;
  }

  bool operator!=(const Place& other) const { return !(*this == other); }

  friend bool operator<(const Place& a, const Place& b);

  std::string DebugString() const;
};

}  // namespace lite_api
}  // namespace paddle
